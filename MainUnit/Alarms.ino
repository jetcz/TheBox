void printDebug() {

}

/// <summary>
/// Helper method - service call which could be in main loop(), but executing every seconds is good enough
/// </summary>
void system() {
	wdt_reset();	//reset watchdog
	DateTime _dtNow = now();
	RemoteDS.isValid = ((millis() / 1000 < Settings.RadioMsgInterval) && !bReceivedRadioMsg) ? false : isRemoteDataSetValid(_dtNow);
	sNow = getDateTimeString(_dtNow);
	sMainUptime = getUptimeString(getUptime(_dtNow));
	for (int relay = 0; relay < 4; relay++)
	{
		if (Settings.RelayMode[relay] > 1) serviceSchedulers(_dtNow, relay);
	}
	printLcd();
}
/// <summary>
/// Fill datasets and apply offsets
/// </summary>
void prepareDataSetArrays() {
	static unsigned int nDHTFailures;
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
	if (DHT.read22(DHT22_PIN) == 0)
	{
		*MainDS.Temperature = getMainTemperature();
		*MainDS.Humidity = getMainHumidity();
		*MainDS.Humidex = getMainHumidex();

		nDHTFailures = 0;
		ledLight(1, 'g');
	}
	else
	{
#if DEBUG
		Serial.println(F("Failed to read from DHT22!"));
#endif
		nDHTFailures++;
		if (nDHTFailures == 2) ledLight(1, 'y');
		else if (nDHTFailures > 2)
		{
			ledLight(1, 'r');
			*MainDS.Temperature = *MainDS.Humidity = *MainDS.Humidex = -255;
		}
	}

	MainDS.Data[3] = getMainPir();
	_fVal = getPressure(event);
	MainDS.Data[4] = (_fVal == -255) ? _fVal : _fVal + Settings.PressureOffset;

	MainDS.TimeStamp = _dtNow;
	SystemDS.TimeStamp = _dtNow;
}


/// <summary>
/// Get power data (mains voltage, power consumption of appliances connected to sockets)
/// </summary>
void getPWRData(){
	float _fVal;
	fVcc = readVcc();
	_fVal = getPower(0);
	MainDS.Data[5] = (_fVal < 1) ? 0 : _fVal;
	_fVal = getPower(3);
	MainDS.Data[6] = (_fVal < 1) ? 0 : _fVal;
	MainDS.Data[7] = getVoltage();
}

/// <summary>
/// Print some sensor data to serial port, disabled if DEBUG=false
/// </summary>
void printSensorDataSerial(){
	if (MainDS.isValid)
	{
		Serial.println();
		Serial.println(F("Main Unit"));
		Serial.print(F("Temperature "));
		Serial.print(*MainDS.Temperature, 1);
		Serial.println(F("C"));
		Serial.print(F("Humidity "));
		Serial.print(*MainDS.Humidity, 0);
		Serial.println(F("%RH"));
		Serial.print(F("Humidex "));
		Serial.print(*MainDS.Humidex, 1);
		Serial.println(F("C"));
		Serial.print(F("Weather forecast: "));
		Serial.println(cWeather[byForecast]);
		Serial.print(F("SysTemperature "));
		Serial.print(*SystemDS.Temperature, 1);
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

	if (RemoteDS.isValid)
	{
		Serial.println(F("Remote Unit"));
		Serial.print(F("Temperature "));
		Serial.print(*RemoteDS.Temperature, 1);
		Serial.println(F("C"));
		Serial.print(F("Humidity "));
		Serial.print(*RemoteDS.Humidity, 0);
		Serial.println(F("%RH"));
		Serial.print(F("Humidex "));
		Serial.print(*RemoteDS.Humidex, 1);
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

/// <summary>
/// Responsible for printing stuff to LCD every second
/// </summary>
void printLcd() {
	static byte _byLastScreen;
	static byte _bySecCnt = 0;

	//do not refresh lcd for specified time if there is some message
	if (!bLCDRefreshing && _bySecCnt < Settings.LcdMsgTimeout)
	{
		_bySecCnt++;
		return;
	}
	else if (!bLCDRefreshing && _bySecCnt >= Settings.LcdMsgTimeout)
	{
		_bySecCnt = 0;
		bLCDRefreshing = true;
		lcd.clear();
	}
	
	//refresh lcd only if PIR sees someone
	if (getMainPir())
	{
		lcd.backlight();

		//which screen to display
		if (!digitalRead(LCD_SWITCH[0]))
		{
			if (_byLastScreen != 1) {
				lcd.clear();
			}
			printLcdScreen1();
			_byLastScreen = 1;
		}
		else if (!digitalRead(LCD_SWITCH[1]))
		{
			if (_byLastScreen != 2) {
				lcd.clear();
			}
			printLcdScreen2();
			_byLastScreen = 2;
		}
		else if (!digitalRead(LCD_SWITCH[2]))
		{
			if (_byLastScreen != 3) {
				lcd.clear();
			}
			printLcdScreen3();
			_byLastScreen = 3;
		}
	}
	else  lcd.noBacklight();
}


/// <summary>
/// Helper method to call Thingspeak upload, rotate datasets and restart arduino if there problems uploading
/// </summary>
void thingSpeak(){
	if (millis() < 41000) return;	//return if time is less than 0:40 (boot time of the wifi router)
	static unsigned int _nCnt;
	byte _byCurrentDS = _nCnt % 3;

	//before we update thingspeak, check if the dataset is valid 
	if (!DataSetPtr[_byCurrentDS]->isValid)
	{
#if DEBUG
		Serial.println();
		Serial.print(F("DS "));
		Serial.print(_byCurrentDS);
		Serial.println(F(" not valid, aborting upload!"));
#endif
		_nCnt++;
		return; //cancel thingspeak update
	}
	//if remote dataset is invalid, cut the system dataset because we have remote voltage and remote uptime in last two floats
	SystemDS.Size = (RemoteDS.isValid) ? 8 : 6;
	DataSetPtr[_byCurrentDS]->GetTSString();

#if DEBUG
	Serial.println();
	Serial.print(F("Update ThingSpeak with DS "));
	Serial.println(_byCurrentDS);
#endif
	updateThingSpeak(*DataSetPtr[_byCurrentDS]);
	_nCnt++;
	needRestart();
}

/// <summary>
/// Helper method to call NTP sync
/// </summary>
void syncRTCwithNTP() {
	bLCDRefreshing = false;
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


/// <summary>
/// Maintain DHCP lease if used
/// </summary>
void dhcp() {
	if (Ethernet.maintain() % 2 == 1) {  //renew dhcp lease
		bLCDRefreshing = false;
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


/// <summary>
/// Helper method to call with delay, writes relay settings to SD card
/// </summary>
void writeSD() {
	bool _bSucces = writeSDRelaySettings();
	if (!_bSucces)
	{
#if DEBUG
		Serial.println(F("Writing relay settings to SD card failed!"));
#endif
		bLCDRefreshing = false;
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