
void printDebug() {
	/*
	char chararr[] = "192 168 0 1";
	byte bytearr[4] = {0,0,0,0};
	chArrToByteArr(chararr, bytearr);
	for (int i = 0; i < 4; i++)
	{
	Serial.println(bytearr[i]);
	}
	*/

	}

void system() {
	sNow = getDateTimeString(now());
	enableDisableAlarms();
	lcdBacklight();
	sUptime = getUptimeString(getUptime());
	receiveData();
	RemoteDS.Valid = isRemoteDataSetValid();
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
		Serial.println(sUptime);
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
		Serial.print(F("RainTicks "));
		Serial.println(RemoteDS.Data[6], 0);
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
	//before we update thingspeak, check if we are supposed to send RemoteDataSet and if it is still valid, if not, return from this funct.
	if (!DataSetPointer[iCurrentDataSet]->Valid)
	{
		if (iCurrentDataSet == 1){	//we have remote voltage and remote uptime in system dataset, so if the remote data set is not valid, we must omit those values
			SystemDS.Size = 6;
		}
		Serial.println();
		Serial.println(F("DataSet not valid, aborting upload!"));
		iCurrentDataSet++;
		return; //cancel thingspeak update
	}
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
		Alarm.disable(byAlarm[5]);
		bAlarmEnabled[5] = false;
		lcd.clear();
		lcdBacklight();
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
		Alarm.disable(byAlarm[5]);
		bAlarmEnabled[5] = false;
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
	if (bConnectivityCheck && !bAlarmEnabled[3] && bDhcp)
	{
		Serial.println(F("Enabling ThingSpeak functionality"));
		Alarm.enable(byAlarm[3]);
		bAlarmEnabled[3] = true;
	}
	else
		if (!bConnectivityCheck && bAlarmEnabled[3] && bDhcp)
		{
			Serial.print(F("Disabling ThingSpeak functionality"));
			Alarm.disable(byAlarm[3]);
			bAlarmEnabled[3] = false;
		}


	//enable lcd refreshing after some msg shows up for x sec
	if (!bAlarmEnabled[5] && byLcdMsgTimeoutCnt > byLcdMsgTimeout)
	{
		lcd.clear();
		Alarm.enable(byAlarm[5]);
		bAlarmEnabled[5] = true;
		byLcdMsgTimeoutCnt = 0;
	}
	else if (!bAlarmEnabled[5] && byLcdMsgTimeoutCnt <= byLcdMsgTimeout)
	{
		byLcdMsgTimeoutCnt++;
	}
}

void weatherForecastTimer() {
	forecast = weatherForecast();
}

void dhcp() {
	if (Ethernet.maintain() % 2 == 1) {  //renew dhcp lease, if failed, set flag
		bConnectivityCheck = false;
		Alarm.disable(byAlarm[5]);
		bAlarmEnabled[5] = false;
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

