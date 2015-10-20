/// <summary>
/// Upload ThingSpeak data. Handles also fail counters, leds, and debug msgs.
/// </summary>
/// <param name="ds">dataset</param>
void updateThingSpeak(DataSet ds) {
	static IPAddress TSIP;
	if (TSIP == INADDR_NONE)
	{
		if (!resolveHost(TSIP, *Settings.ThingSpeakAddress)) {
			nFailedNetworkOps++;
			nFailedNetowkOpsTotal++;
			return;
		};
	}

	ledLight(3, 'b');
	client.flush();
	//connect to thingspeak
	unsigned static long _lLastCnn;
	if (!client.connected() || now() - _lLastCnn > 420) //for some reason the connection doesn't last past 500 sec, so we need to close and reopen it manualy for maximum reliability
	{
		ledLight(3, 'c');
		client.stop();
#if DEBUG
		Serial.print(F("Connecting to ThingSpeak..."));
#endif		
		wdt_disable();
		if (client.connect(TSIP, 80)) _lLastCnn = now();
		wdt_enable(WDTO_8S);
	}
	//update thingspeak
	if (client.connected()) {
		nFailedNetworkOps = 0;
#if DEBUG
		Serial.println(F("Connected to ThingSpeak, sending data..."));
		Serial.println(ds.ThingSpeakStr);
#endif
		client.print(F("POST /update HTTP/1.1\n"));
		client.print(F("Host: api.thingspeak.com\n"));
		client.print(F("Connection: Keep-Alive\n"));
		client.print(F("X-THINGSPEAKAPIKEY: "));
		client.print(ds.APIkey + "\n");
		client.print(F("headers: false\n"));
		client.print(F("Content-Type: application/x-www-form-urlencoded\n"));
		client.print(F("Content-Length: "));
		client.print(intToString(ds.ThingSpeakStr.length()) + "\n\n");
		client.println(ds.ThingSpeakStr);
		ledLight(3, 'g');
	}
	else
	{
		client.stop();
		ledLight(3, 'y');
		nFailedNetworkOps++;
		nFailedNetowkOpsTotal++;
		bLCDRefreshing = false;
		lcd.clear();
		lcd.setCursor(0, 0);
		lcd.print(F("Connecting to"));
		lcd.setCursor(0, 1);
		lcd.print(F("ThingSpeak failed"));
		lcd.setCursor(0, 2);
		lcd.print(intToString(nFailedNetworkOps));
		lcd.print(F(" times!"));
#if DEBUG
		Serial.print(F("Connecting to ThingSpeak failed "));
		Serial.print(intToString(nFailedNetworkOps));
		Serial.println(F(" times!"));
		Serial.println();
#endif
	}
}
