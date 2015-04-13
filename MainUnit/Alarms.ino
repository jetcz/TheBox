
void printDebug() {
	time_t ntp = time_t(ntpUnixTime(udp));
	if (ntp != 0)
	{
		rtc.adjust(DateTime(ntp));
	}

	time_t local = myTZ.toLocal(now());
	Serial.println("LOCAL" + getDateTimeString(local));
}


void system() {
	sNow = getDateTimeString(myTZ.toLocal(now()));
	enableDisableAlarms();
	lcdBacklight();
	sMainUptime = getUptimeString(getUptime());
	RemoteDS.Valid = isRemoteDataSetValid();
	receiveData();
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
	if (SystemDS.Data[0] == -255)
	{
		MainDS.Data[4] = -255; //pressure
	}
	else MainDS.Data[4] = getPressure(event);


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
		Serial.println(weather[forecast]);
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
	static byte byLastScreen;
	if (digitalRead(LCD_SWITCH[0]) == 0)
	{
		if (byLastScreen != 1) {
			lcd.clear();
		}
		printLcdScreen1();
		byLastScreen = 1;
	}
	else if (digitalRead(LCD_SWITCH[1]) == 0)
	{
		if (byLastScreen != 2) {
			lcd.clear();
		}
		printLcdScreen2();
		byLastScreen = 2;
	}
	else if (digitalRead(LCD_SWITCH[2]) == 0)
	{
		if (byLastScreen != 3) {
			lcd.clear();
		}
		printLcdScreen3();
		byLastScreen = 3;
	}
}


void thingSpeak(){
	//before we update thingspeak, check if the dataset is valid 
	if (!DataSetPointer[iCurrentDataSet]->Valid)
	{

		Serial.println();
		Serial.println(F("DataSet not valid, aborting upload!"));
		iCurrentDataSet++;
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
		Serial.println();
		Serial.println(F("Couldn't close connection"));
		ledLight(3, 'r');
	}
	else
		Serial.println();
	Serial.println(F("Connection closed"));
	Serial.println();

	// Update ThingSpeak
	if (!client.connected())
	{ // iterate through our array of pointers to our dataset arrays 
		if (iCurrentDataSet > 2) {
			iCurrentDataSet = 0;
		}
		Serial.print(F("Update ThingSpeak with dataset "));
		Serial.println(intToString(iCurrentDataSet));
		updateThingSpeak(*DataSetPointer[iCurrentDataSet]);
		iCurrentDataSet++;
	}
	// Check if Ethernet needs to be restarted
	if ((iFailedCounter % iRestartEthernetThreshold) == 0 && iFailedCounter != 0){
		ledLight(3, 'r');
		Serial.println(F("Ethernet Shield needs to be restarted!"));
		Serial.println();
		Alarm.disable(printLcdAlarm);
		lcd.clear();
		lcd.setCursor(0, 0);
		lcd.print(F("Ethernet Shield"));
		lcd.setCursor(0, 1);
		lcd.print(F("must be restarted!"));
		setupEthernet();
	}
	// Check if Arduino needs to be restarted
	if ((iFailedCounter % iRestartArduinoThreshold) == 0 && iFailedCounter != 0) {
		ledLight(1, 'r');
		ledLight(3, 'r');
		Serial.println(F("Arduino needs to be restarted!"));
		Serial.println();
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


// disable thingspeak updating if we do not have ip address
void enableDisableAlarms() {

	// enable or disable thingspeak depending on obtained dhcp lease (its no necesary but if we dont have connectioon on startup, its good to disable thingspeak)
	if (bConnectivityCheck && !Alarm.active(updateTSAlarm) && bDhcp)
	{
		Serial.println(F("Enabling ThingSpeak functionality"));
		Alarm.enable(updateTSAlarm);
	}
	else
		if (!bConnectivityCheck && Alarm.active(updateTSAlarm) && bDhcp)
		{
			Serial.print(F("Disabling ThingSpeak functionality"));
			Alarm.disable(updateTSAlarm);
		}


	//enable lcd refreshing after some msg shows up for x sec
	if (!Alarm.active(printLcdAlarm) && byLcdMsgTimeoutCnt > byLcdMsgTimeout)
	{
		lcd.clear();
		Alarm.enable(printLcdAlarm);
		byLcdMsgTimeoutCnt = 0;
	}
	else if (!Alarm.active(printLcdAlarm) && byLcdMsgTimeoutCnt <= byLcdMsgTimeout)
	{
		byLcdMsgTimeoutCnt++;
	}
}

void weatherForecastTimer() {
	forecast = weatherForecast();
}

void syncRTCwithNTP() {
	Alarm.disable(printLcdAlarm);
	lcd.clear();
	lcd.setCursor(0, 0);

	unsigned long ntp = ntpUnixTime(udp);
	if (ntp != 0)
	{
		rtc.adjust(DateTime(ntp));
		lcd.print(F("NTP timesync success"));
		Serial.println(F("NTP time sync success"));
	}
	else
	{
		lcd.print(F("NTP timesync failed!"));
		Serial.println(F("NTP time sync failed!"));
	}
}

void dhcp() {
	if (Ethernet.maintain() % 2 == 1) {  //renew dhcp lease, if failed, set flag
		bConnectivityCheck = false;
		Alarm.disable(printLcdAlarm);
		Serial.println();
		Serial.println(F("Failed to obtain DHCP lease!"));
		lcd.clear();
		lcd.setCursor(0, 0);
		lcd.print(F("Failed to obtain"));
		lcd.setCursor(0, 1);
		lcd.print(F("DHCP lease!"));
		ledLight(1, 'm');
	}
	else {
		bConnectivityCheck = true;
		Serial.println();
		Serial.println(F("Obtained DHCP lease"));
		Serial.print(F("IP: "));
		Serial.println(Ethernet.localIP());
		Serial.print(F("GW: "));
		Serial.println(Ethernet.gatewayIP());
		Serial.print(F("DNS: "));
		Serial.println(Ethernet.dnsServerIP());
		ledLight(1, 'g');
	}
}

void writeSD() {
		writeSDRelaySettings(relays);
}