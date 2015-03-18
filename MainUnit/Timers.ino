void system() {
	enableDisableAlarms();
	lcdBacklight();
	sNow = getDateTimeString(now());
	//sNow = getDateTimeString(rtc.now());
	sUptime = getUptimeString(getUptime());
}

void printDebug(){


}

void prepareDataSetArrays() {

	sensors_event_t event;	//event for bmp180

	fSysDataSet[0] = getSysTemperature(event);				//sysTemperature
	fSysDataSet[1] = getUptime().totalseconds();			//uptime
	fSysDataSet[2] = getRelayState(0);						//relay1
	fSysDataSet[3] = getRelayState(1);						//relay2
	fSysDataSet[4] = getRelayState(2);						//relay3
	fSysDataSet[5] = getRelayState(3);						//relay4

	fMainUnitDataSet[0] = getMainTemperature();				//mainTemperature
	fMainUnitDataSet[1] = getMainHumidity();				//mainHumidity
	fMainUnitDataSet[2] = getMainHumidex();					//mainHumidex
	fMainUnitDataSet[3] = getMainPir();						//mainPir
	fMainUnitDataSet[4] = getPressure(event);				//pressure

	/*
	fRemoteUnitDataSet[0] = 0;								//remoteTemperature
	fRemoteUnitDataSet[1] = 0;								//remoteHumidity
	fRemoteUnitDataSet[2] = 0;								//remoteHumidex
	fRemoteUnitDataSet[3] = 0;								//remoteSoilTemperature
	fRemoteUnitDataSet[4] = 0;								//remoteSoilHumidity
	fRemoteUnitDataSet[5] = 0;								//remoteLight
	fRemoteUnitDataSet[6] = 0;								//rain
	*/

}

void printSensorDataSerial(){
	Serial.println();
	Serial.println("MainTemperature " + floatToString(fMainUnitDataSet[0]) + "C");
	Serial.println("MainHumidity " + floatToString(fMainUnitDataSet[1]) + "%RH");
	Serial.println("MainHumidex " + floatToString(fMainUnitDataSet[2]) + "C");
	Serial.print("Weather forecast: ");
	Serial.println(weather[forecast]);
	Serial.println("SysTemperature " + floatToString(fSysDataSet[0]) + "C");
	Serial.println("Pressure " + floatToString(fMainUnitDataSet[4]) + "hPa");
	Serial.println(sNow);
	Serial.println("Uptime " + sUptime);
	Serial.println("Free ram " + intToString(freeRam()) + "b (" + floatToString(float(freeRam()) / 8192 * 100) + "%)");
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
	//before we update thingspeak, check if we are supposed to sent RemoteDataSet and if it is still valid, if not, return from this funct.
	if (iCurrentDataSet == 1) {
		if (!isRemoteDataSetValid())
		{
			Serial.println();
			Serial.println(F("Remote DataSet timeout, aborting upload!"));
			iCurrentDataSet++;
			return; //cancel thingspeak update
		}
	}

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
	{ /* iterate through our array of pointers to our dataset arrays */
		if (iCurrentDataSet > 2) {
			iCurrentDataSet = 0;
		}
		Serial.print(F("Update ThingSpeak with dataset "));
		Serial.println(intToString(iCurrentDataSet));
		/* make nice strings, parameters are pointer to array and size of the array pointing to */
		updateThingSpeak(prepareString(fDataSetPointer[iCurrentDataSet], arrSizes[iCurrentDataSet]), sAPIkeys[iCurrentDataSet]);
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

/* disable thingspeak updating if we do not have ip address*/
void enableDisableAlarms() {

	// enable or disable thingspeak depending on obtained dhcp lease (its no necesary but if we dont have connectioon on startup, its good to disable thingspeak)
	if (bConnectivityCheck && !bAlarmEnabled[3])
	{
		Serial.println(F("Enabling ThingSpeak functionality"));
		Alarm.enable(byAlarm[3]);
		bAlarmEnabled[3] = true;
	}
	else
		if (!bConnectivityCheck && bAlarmEnabled[3])
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

