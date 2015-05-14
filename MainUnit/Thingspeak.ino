//************************************
// Method:   	 updateThingSpeak
// Description:  Creates string suitable for upload to Thingspeak, then actually uploads it. Handles also fail counters, leds, and debug msgs.
// Access:   	 public 
// Returns:  	 void
// Qualifier:	
// Parameter:	 DataSet ds
//************************************
void updateThingSpeak(DataSet ds){
	ledLight(3, 'b');
	/* This produces nice string for ThingSpeak like 1=21.5&2=51.8&3=..... etc depending on what array you put in.
	The values in array MUST be sorted exactly like ThingSpeak fields go one by one: {teperature, humidity, humidex etc....} */
	String _sValues;
	for (int i = 0; i < ds.Size; i++)
	{
		if (ds.Data[i] > -100) //in case we get some broken values which are -255
		{
			_sValues += intToString(i + 1) + "=" + floatToString(ds.Data[i]);
			if (i < ds.Size - 1) _sValues += "&";
		}
	}
	//connect to thingspeak
	unsigned static long _lLastCnn;	
	if (!client.connected() || now() - _lLastCnn > 420) //for some reason the connection doesn't last past 500 sec, so we need to close and reopen it manualy for maximum reliability
	{
		ledLight(3, 'c');
		client.flush();
		client.stop();
#if DEBUG
		Serial.print(F("Connecting to ThingSpeak..."));
#endif		
		if (client.connect(Settings.ThingSpeakAddress, 80)) _lLastCnn = now();
	}
	//update thingspeak
	if (client.connected())	{
		nFailedCounter = 0;
#if DEBUG
		Serial.println(F("Connected to ThingSpeak"));
		Serial.println(F("Sending data... "));
		Serial.println(_sValues);
#endif
		client.print(F("POST /update HTTP/1.1\n"));
		client.print(F("Host: api.thingspeak.com\n"));
		client.print(F("Connection: Keep-Alive\n"));
		client.print(F("X-THINGSPEAKAPIKEY: "));
		client.print(ds.APIkey + "\n");
		client.print(F("headers: false\n"));
		client.print(F("Content-Type: application/x-www-form-urlencoded\n"));
		client.print(F("Content-Length: "));
		client.print(intToString(_sValues.length()) + "\n\n");
		client.println(_sValues);
		ledLight(3, 'g');
	}
	else
	{
#if DEBUG
		Serial.print(F("Connecting to ThingSpeak failed "));
		Serial.print(intToString(nFailedCounter));
		Serial.println(F(" times!"));
		Serial.println();
#endif
		client.stop();
		if (nFailedCounter > Settings.RestartEthernetThreshold)	ledLight(3, 'r');
		else ledLight(3, 'y');
		nFailedCounter++;
		nFailedCntTSTotal++;
		Alarm.disable(printLcdAlarm);
		lcd.clear();
		lcd.setCursor(0, 0);
		lcd.print(F("Connecting to"));
		lcd.setCursor(0, 1);
		lcd.print(F("ThingSpeak failed"));
		lcd.setCursor(0, 2);
		lcd.print(intToString(nFailedCounter));
		lcd.print(F(" times!"));
	}
}