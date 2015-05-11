//************************************
// Method:   	 updateThingSpeak
// Description:  Creates string suitable for upload to Thingspeak, the actualy uploads it. Handles also fail counters, leds, and debug msgs.
// Access:   	 public 
// Returns:  	 void
// Qualifier:	
// Parameter:	 DataSet ds
//************************************
void updateThingSpeak(DataSet ds){
	/* This produces nice string for ThingSpeak like 1=21.5&2=51.8&3=..... etc depending on what array you put in.
	The values in array MUST be sorted exactly like ThingSpeak fields go one by one: {teperature, humidity, humidex etc....} */
	String s;
	for (int i = 0; i < ds.Size; i++)
	{
		if (ds.Data[i] > -100) //in case we get some broken values which are -255
		{
			s += intToString(i + 1) + "=" + floatToString(ds.Data[i]);

			if (i < ds.Size - 1) {
				s += "&";
			}
		}
	}
	//update thingspeak
#if DEBUG
	Serial.print(F("Connecting to ThingSpeak..."));
#endif
	ledLight(3, 'b');
	if (client.connect(Settings.ThingSpeakAddress, 80)) //string, int
	{
		ledLight(3, 'g');
#if DEBUG
		Serial.println(F("connected"));
#endif
		//POST update to thingspeak, print line by line
		client.print(F("POST /update HTTP/1.1\n"));
		client.print(F("Host: api.thingspeak.com\n"));
		client.print(F("Connection: close\n"));
		client.print(F("X-THINGSPEAKAPIKEY: "));
		client.print(ds.APIkey + "\n");
		client.print(F("Content-Type: application/x-www-form-urlencoded\n"));
		client.print(F("Content-Length: "));
		client.print(intToString(s.length()) + "\n\n");
		client.println(s);

		if (client.connected())
		{
#if DEBUG
			Serial.println(F("Sending data... "));
			Serial.println(s);
#endif
			nFailedCounter = 0;
		}
		else
		{
			if (nFailedCounter > Settings.RestartEthernetThreshold) {
				ledLight(3, 'r');
			}
			else
				ledLight(3, 'y');
			nFailedCounter++;
			nFailedCntTSTotal++;
			Alarm.disable(printLcdAlarm);
			lcd.clear();
			lcd.setCursor(0, 0);
			lcd.print(F("client.connected()"));
			lcd.setCursor(0, 1);
			lcd.print(F("failed "));
			lcd.print(intToString(nFailedCounter));
			lcd.print(F(" times"));
#if DEBUG
			Serial.print(F("client.connected() failed "));
			Serial.print(intToString(nFailedCounter));
			Serial.println(F(" times"));
			Serial.println();
#endif
		}
	}
	else
	{

		if (nFailedCounter > Settings.RestartEthernetThreshold) {
			ledLight(3, 'r');
		}
		else
			ledLight(3, 'y');
		nFailedCounter++;
		nFailedCntTSTotal++;

		Alarm.disable(printLcdAlarm);
		lcd.clear();
		lcd.setCursor(0, 0);
		lcd.print(F("client.connect()"));
		lcd.setCursor(0, 1);
		lcd.print(F("failed "));
		lcd.print(intToString(nFailedCounter));
		lcd.print(F(" times"));
#if DEBUG
		Serial.print(F("client.connect() failed "));
		Serial.print(intToString(nFailedCounter));
		Serial.println(F(" times"));
		Serial.println();
#endif
	}
}