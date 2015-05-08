void printDebug() {
	for (int i = 0; i < 4; i++)
	{
		if (Sched[i].Variable != 0)
		{

			Serial.print("current val ");
			Serial.println(*TargetVarPtr[Sched[i].Variable]);
			Serial.print("target val min ");
			Serial.println(Sched[i].Value[Sched[i].CurrentInterval][0]);
			Serial.print("target val max ");
			Serial.println(Sched[i].Value[Sched[i].CurrentInterval][1]);
		}
	}
	Serial.println();
}

void system() {
	receiveData();
	RemoteDS.Valid = isRemoteDataSetValid();
	sNow = getDateTimeString(now());
	sMainUptime = getUptimeString(getUptime());
	enableDisableAlarms();
	lcdBacklight();

	//scheduler part
	for (int i = 0; i < 4; i++)
	{
		if (Settings.RelayMode[i] > 1)
		{
			serviceSchedulers(i);
		}
	}
}

void prepareDataSetArrays() {

	sensors_event_t event;	//event for bmp180

	SystemDS.Data[0] = getSysTemperature(event);
	SystemDS.Data[1] = getUptime().totalseconds();

	for (int i = 0; i < 4; i++)
	{
		SystemDS.Data[i + 2] = getRelayState(i);
	}

	MainDS.Data[0] = getMainTemperature();
	MainDS.Data[1] = getMainHumidity();
	MainDS.Data[2] = getMainHumidex();
	MainDS.Data[3] = getMainPir();
	MainDS.Data[4] = getPressure(event);

	MainDS.Timestamp = now();

}

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
		Serial.println(intToString(freeRam()) + "b (" + floatToString(float(freeRam()) / 8192 * 100) + "%)");
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
	}
	else Serial.println(F("Remote Unit DataSet invalid!"));
}

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

void thingSpeak(){
	//before we update thingspeak, check if the dataset is valid 
	if (!DataSetPtr[byCurrentDataSet]->Valid)
	{

#if DEBUG
		Serial.println();
		Serial.println(F("DataSet not valid, aborting upload!"));
#endif
		byCurrentDataSet++;
		return; //cancel thingspeak update
	}

	//if remote dataset is invalid, cut the system dataset because we have remote voltage and remote uptime in last two floats
	if (RemoteDS.Valid == false) SystemDS.Size = 6;
	else SystemDS.Size = 8;

	//close previous connnection
	client.flush();
	client.stop();
	int iTimeout = 0;
	while (client.status() != 0 && iTimeout < 5000) {
		Alarm.delay(5);
		iTimeout += 5;
	}
	if (iTimeout > 5000)
	{
#if DEBUG
		Serial.println();
		Serial.println(F("Couldn't close connection"));
#endif

		ledLight(3, 'r');
	}
	else
#if DEBUG
		Serial.println();
	Serial.println(F("Connection closed"));
	Serial.println();
#endif

	// Update ThingSpeak
	if (!client.connected())
	{ // iterate through our array of pointers to our dataset arrays 
		if (byCurrentDataSet > 2) {
			byCurrentDataSet = 0;
		}
#if DEBUG
		Serial.print(F("Update ThingSpeak with dataset "));
		Serial.println(intToString(byCurrentDataSet));
#endif
		updateThingSpeak(*DataSetPtr[byCurrentDataSet]);
		byCurrentDataSet++;
	}
	// Check if Ethernet needs to be restarted
	if ((nFailedCounter % Settings.RestartEthernetThreshold) == 0 && nFailedCounter != 0){
		ledLight(3, 'r');
#if DEBUG
		Serial.println(F("Ethernet Shield needs to be restarted!"));
		Serial.println();
#endif
		Alarm.disable(printLcdAlarm);
		lcd.clear();
		lcd.setCursor(0, 0);
		lcd.print(F("Ethernet Shield"));
		lcd.setCursor(0, 1);
		lcd.print(F("must be restarted!"));
		setupEthernet();
	}
	// Check if Arduino needs to be restarted
	if ((nFailedCounter % Settings.RestartArduinoThreshold) == 0 && nFailedCounter != 0) {
		ledLight(1, 'r');
		ledLight(3, 'r');
#if DEBUG
		Serial.println(F("Arduino needs to be restarted!"));
		Serial.println();
#endif
		Alarm.disable(printLcdAlarm);
		lcd.clear();
		lcdBacklight();
		lcd.setCursor(0, 0);
		lcd.print(F("Arduino needs to"));
		lcd.setCursor(0, 1);
		lcd.print(F("be restarted"));
		Alarm.delay(3000);
		resetFunc(); //reboot arduino
	}
	}


void enableDisableAlarms() {
	//enable disable thingspeak accoring to system settings
	if (Settings.TSenabled && !Alarm.active(updateTSAlarm))
	{
		Alarm.enable(updateTSAlarm);
	}
	if (!Settings.TSenabled && Alarm.active(updateTSAlarm))
	{
		Alarm.disable(updateTSAlarm);
	}

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