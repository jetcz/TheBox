void updateThingSpeak(DataSet ds){
	/* This produces nice string for ThingSpeak like 1=21.5&2=51.8&3=..... etc depending on what array you put in.
	The values in array MUST be sorted exactly like ThingSpeak fields go one by one: {teperature, humidity, humidex etc....} */
	String s;
	for (int i = 0; i < ds.Size; i++)
	{
		s += intToString(i + 1) + "=" + floatToString(ds.Data[i]);
		if (i < ds.Size - 1) {
			s += "&";
		}
	}
	//update thingspeak
	Serial.print(F("Connecting to ThingSpeak..."));
	ledLight(3, 'b');
	if (client.connect(cThingSpeakAddress, 80)) //string, int
	{
		Serial.println(F("connected"));
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
			ledLight(3, 'g');
			Serial.println(F("Sending data... "));
			Serial.println(s);
			iFailedCounter = 0;
		}
		else
		{
			if (iFailedCounter > iRestartEthernetThreshold) {
				ledLight(3, 'r');
			}
			else
				ledLight(3, 'y');
			iFailedCounter++;
			Alarm.disable(printLcdAlarm);
			lcd.clear();
			lcdBacklight();
			lcd.setCursor(0, 0);
			lcd.print(F("client.connected()"));
			lcd.setCursor(0, 1);
			lcd.print(F("failed "));
			lcd.print(intToString(iFailedCounter));
			lcd.print(F(" times"));
			Serial.print(F("client.connected() failed "));
			Serial.print(intToString(iFailedCounter));
			Serial.println(F(" times"));
			Serial.println();
		}
	}
	else
	{

		if (iFailedCounter > iRestartEthernetThreshold) {
			ledLight(3, 'r');
		}
		else
			ledLight(3, 'y');
		iFailedCounter++;

		Alarm.disable(printLcdAlarm);
		lcd.clear();
		lcd.backlight();
		lcd.setCursor(0, 0);
		lcd.print(F("client.connect()"));
		lcd.setCursor(0, 1);
		lcd.print(F("failed "));
		lcd.print(intToString(iFailedCounter));
		lcd.print(F(" times"));
		Serial.print(F("client.connect() failed "));
		Serial.print(intToString(iFailedCounter));
		Serial.println(F(" times"));
		Serial.println();
	}
}