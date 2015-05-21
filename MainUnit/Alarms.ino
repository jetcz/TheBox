void printDebug() {
	Serial.println(readVcc());
}

//************************************
// Method:   	 system
// Description:  Helper method - service call which could be in main loop(), but executing every seconds is good enough
// Access:   	 public 
// Returns:  	 void
// Qualifier:	
//************************************
void system() {
	DateTime _dtNow = now();
	RemoteDS.Valid = isRemoteDataSetValid(_dtNow);
	sNow = getDateTimeString(_dtNow);
	sMainUptime = getUptimeString(getUptime(_dtNow));
	enableDisableAlarms();
	lcdBacklight();
	for (int i = 0; i < 4; i++)
	{
		if (Settings.RelayMode[i] > 1) serviceSchedulers(_dtNow, i);
	}
	receiveData();
}

//************************************
// Method:   	 prepareDataSetArrays
// Description:  Fill datasets and apply offsets
// Access:   	 public 
// Returns:  	 void
// Qualifier:	
//************************************
void prepareDataSetArrays() {
	DateTime _dtNow = now();
	sensors_event_t event;	//event for bmp180
	//system DS
	float _fVal;
	_fVal = getSysTemperature(event);
	SystemDS.Data[0] = (_fVal == -255) ? _fVal : _fVal + Settings.SysTempOffset;
	SystemDS.Data[1] = getUptime(_dtNow).totalseconds();
	for (int i = 0; i < 4; i++)
	{
		SystemDS.Data[i + 2] = getRelayState(i);
	}
	nMainFreeRam = freeRam();

	//main DS
	_fVal = getMainTemperature();
	MainDS.Data[0] = (_fVal == -255) ? _fVal : _fVal + Settings.MainTempOffset;
	MainDS.Data[1] = getMainHumidity();
	MainDS.Data[2] = getMainHumidex();
	MainDS.Data[3] = getMainPir();
	_fVal = getPressure(event);
	MainDS.Data[4] = (_fVal == -255) ? _fVal : _fVal + Settings.PressureOffset;

	MainDS.Timestamp = _dtNow;
	SystemDS.Timestamp = _dtNow;
}

//************************************
// Method:   	 printSensorDataSerial
// Description:  Print some sensor data to serial port, disabled if DEBUG=false
// Access:   	 public 
// Returns:  	 void
// Qualifier:	
//************************************
void printSensorDataSerial(){
	if (MainDS.Valid == true)
	{
		Serial.println();
		Serial.println(F("Main Unit"));
		Serial.print(F("Temperature "));
		Serial.print(MainDS.Data[0], 1);
		Serial.println(F("C"));
		Serial.print(F("Humidity "));
		Serial.print(MainDS.Data[1], 0);
		Serial.println(F("%RH"));
		Serial.print(F("Humidex "));
		Serial.print(MainDS.Data[2], 1);
		Serial.println(F("C"));
		Serial.print(F("Weather forecast: "));
		Serial.println(cWeather[byForecast]);
		Serial.print(F("SysTemperature "));
		Serial.print(SystemDS.Data[0], 1);
		Serial.println(F("C"));
		Serial.print(F("Pressure "));
		Serial.print(MainDS.Data[4], 1);
		Serial.println(F("hPa"));
		Serial.println(sNow);
		Serial.print(F("Uptime "));
		Serial.println(sMainUptime);
		Serial.print(F("Free ram "));
		Serial.println(intToString(nMainFreeRam) + "B (" + floatToString(float(nMainFreeRam) / 8192 * 100) + "%)");
		Serial.println();
	}
	else Serial.println(F("Main Unit DataSet invalid!"));

	if (RemoteDS.Valid == true)
	{
		Serial.println(F("Remote Unit"));
		Serial.print(F("Temperature "));
		Serial.print(RemoteDS.Data[0], 1);
		Serial.println(F("C"));
		Serial.print(F("Humidity "));
		Serial.print(RemoteDS.Data[1], 0);
		Serial.println(F("%RH"));
		Serial.print(F("Humidex "));
		Serial.print(RemoteDS.Data[2], 1);
		Serial.println(F("C"));
		Serial.print(F("SoilTemperature "));
		Serial.print(RemoteDS.Data[3], 1);
		Serial.println(F("C"));
		Serial.print(F("SoilHumidity "));
		Serial.print(RemoteDS.Data[4], 0);
		Serial.println(F("%RH"));
		Serial.print(F("Light "));
		Serial.print(RemoteDS.Data[5], 0);
		Serial.println(F("%"));
		Serial.print(F("Rain "));
		Serial.print(RemoteDS.Data[6], 1);
		Serial.println(F("mm/h"));
		Serial.print(F("Voltage "));
		Serial.print(SystemDS.Data[6], 0);
		Serial.println(F("mV"));
		Serial.print(F("Uptime "));
		Serial.println(getUptimeString(TimeSpan(SystemDS.Data[7])));
		Serial.print(F("Free ram "));
		Serial.println(intToString(nRemoteFreeRam) + "B (" + floatToString(float(nRemoteFreeRam) / 2048 * 100) + "%)");
	}
	else Serial.println(F("Remote Unit DataSet invalid!"));
}

//************************************
// Method:   	 printLcd
// Description:  Responsible for printing stuff to LCD every second
// Access:   	 public 
// Returns:  	 void
// Qualifier:	
//************************************
void printLcd() {
	static byte _byLastScreen;
	if (digitalRead(LCD_SWITCH[0]) == 0)
	{
		if (_byLastScreen != 1) {
			lcd.clear();
		}
		printLcdScreen1();
		_byLastScreen = 1;
	}
	else if (digitalRead(LCD_SWITCH[1]) == 0)
	{
		if (_byLastScreen != 2) {
			lcd.clear();
		}
		printLcdScreen2();
		_byLastScreen = 2;
	}
	else if (digitalRead(LCD_SWITCH[2]) == 0)
	{
		if (_byLastScreen != 3) {
			lcd.clear();
		}
		printLcdScreen3();
		_byLastScreen = 3;
	}
}


//************************************
// Method:   	 thingSpeak
// Description:  Helper method to call Thingspeak upload, rotate datasets adn restart arduino if there problems uploading
// Access:   	 public 
// Returns:  	 void
// Qualifier:	
//************************************
void thingSpeak(){
	static unsigned int _nCnt;
	byte _byCurrentDS = _nCnt % 3;
	if (updateTSAlarm == 0) updateTSAlarm = Alarm.timerRepeat(Settings.UpdateThingSpeakInterval, thingSpeak); //update ThingSpeak every x ms

	//before we update thingspeak, check if the dataset is valid 
	if (!DataSetPtr[_byCurrentDS]->Valid)
	{
#if DEBUG
		Serial.println();
		Serial.println(F("DataSet not valid, aborting upload!"));
#endif
		_nCnt++;
		return; //cancel thingspeak update
	}
	//if remote dataset is invalid, cut the system dataset because we have remote voltage and remote uptime in last two floats
	SystemDS.Size = (RemoteDS.Valid) ? SystemDS.Size = 8 : SystemDS.Size = 6;

#if DEBUG
	Serial.print(F("Update ThingSpeak with dataset "));
	Serial.println(_byCurrentDS);
#endif
	updateThingSpeak(*DataSetPtr[_byCurrentDS]);
	_nCnt++;
	needRestart();
}

//************************************
// Method:   	 enableDisableAlarms
// Description:  Helper method to temporarily disable alarms (currently only LCD alarm to display some message)
// Access:   	 public 
// Returns:  	 void
// Qualifier:	
//************************************
void enableDisableAlarms() {

	//enable lcd refreshing after some msg shows up for x sec
	static byte _bySecCnt = 0;
	if (!Alarm.active(printLcdAlarm) && _bySecCnt > Settings.LcdMsgTimeout)
	{
		lcd.clear();
		Alarm.enable(printLcdAlarm);
		_bySecCnt = 0;
	}
	else if (!Alarm.active(printLcdAlarm) && _bySecCnt <= Settings.LcdMsgTimeout)
	{
		_bySecCnt++;
	}
}

//************************************
// Method:   	 syncRTCwithNTP
// Description:  Helper method to call NTP sync
// Access:   	 public 
// Returns:  	 void
// Qualifier:	
//************************************
void syncRTCwithNTP() {
	Alarm.disable(printLcdAlarm);
	lcd.clear();
	lcd.setCursor(0, 0);

	unsigned long ntp = ntpUnixTime(udp);
	if (ntp != 0)
	{
		dtLastNTPsync = DateTime(now());
		rtc.adjust(DateTime(ntp));
		lcd.print(F("NTP timesync success"));
#if DEBUG
		Serial.println(F("NTP time sync success"));
#endif
	}
	else
	{
		lcd.print(F("NTP timesync failed!"));
#if DEBUG
		Serial.println(F("NTP time sync failed!"));
#endif
	}
}


//************************************
// Method:   	 dhcp
// Description:  Maintain DHCP lease if used
// Access:   	 public 
// Returns:  	 void
// Qualifier:	
//************************************
void dhcp() {
	if (Ethernet.maintain() % 2 == 1) {  //renew dhcp lease
		Alarm.disable(printLcdAlarm);
#if DEBUG
		Serial.println();
		Serial.println(F("Failed to obtain DHCP lease!"));
#endif
		lcd.clear();
		lcd.setCursor(0, 0);
		lcd.print(F("Failed to obtain"));
		lcd.setCursor(0, 1);
		lcd.print(F("DHCP lease!"));
		ledLight(1, 'm');
	}
	else {
#if DEBUG
		Serial.println();
		Serial.println(F("Obtained DHCP lease"));
		Serial.print(F("IP: "));
		Serial.println(Ethernet.localIP());
		Serial.print(F("GW: "));
		Serial.println(Ethernet.gatewayIP());
		Serial.print(F("DNS: "));
		Serial.println(Ethernet.dnsServerIP());
#endif
		ledLight(1, 'g');
	}
}


//************************************
// Method:   	 writeSD
// Description:  Helper method to call with delay, writes relay settings to SD card
// Access:   	 public 
// Returns:  	 void
// Qualifier:	
//************************************
void writeSD() {
	bool _bSucces = writeSDRelaySettings();
	if (!_bSucces)
	{
#if DEBUG
		Serial.println(F("Writing relay settings to SD card failed!"));
#endif
		Alarm.disable(printLcdAlarm);
		lcd.clear();
		lcd.setCursor(0, 0);
		lcd.print(F("Writing relay"));
		lcd.setCursor(0, 1);
		lcd.print(F("settings to SD card"));
		lcd.setCursor(0, 2);
		lcd.print(F("failed!"));
	}
	else
	{
#if DEBUG
		Serial.println(F("Writing relay settings to SD card ok"));
#endif
	}
}