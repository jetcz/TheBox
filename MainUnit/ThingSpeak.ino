/// <summary>
/// Upload ThingSpeak data. Handles also fail counters, leds, and debug msgs.
/// </summary>
/// <param name="ds">dataset</param>
void updateThingSpeak(DataSet ds)
{
	wdt_disable();
	static IPAddress TSIP;
	if (TSIP == INADDR_NONE)
	{
		if (!resolveHost(TSIP, *Settings.ThingSpeakAddress))
		{
			nFailedNetworkOps++;
			nFailedNetowkOpsTotal++;
			return;
		};
	}

	ledLight(3, 'b');
	client.flush();
	//connect to thingspeak
	unsigned static long lLastCnn;
	if (!client.connected() || now() - lLastCnn > Settings.TSCnnTimeout) //for some reason the connection doesn't last past 500 sec, so we need to close and reopen it manualy for maximum reliability
	{
		ledLight(3, 'c');
		client.stop();
#if DEBUG
		Serial.print(F("Connecting to ThingSpeak..."));
#endif		

		if (client.connect(TSIP, 80)) lLastCnn = now();

	}
	//update thingspeak
	if (client.connected())
	{
		nFailedNetworkOps = 0;
#if DEBUG
		Serial.println(F("Connected to ThingSpeak, sending data..."));
		Serial.println(ds.ThingSpeakStr);
#endif

		sendData(ds);

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

		wdt_enable(WDTO_8S);
}


/// <summary>
/// Upload ThingSpeak data.
/// </summary>
/// <param name="ds">dataset</param>
void sendData(DataSet ds)
{

	if (Settings.Method == POST)
	{
#if DEBUG
		Serial.println("POST");
#endif // DEBUG

		client.print(F("POST /update HTTP/1.1\n"));
		client.print(F("Host: "));
		client.print(Settings.ThingSpeakAddress);
		client.print(F("\nConnection: Keep-Alive\n"));
		client.print(F("X-THINGSPEAKAPIKEY: "));
		client.print(ds.APIkey);
		client.print(F("\nheaders: false\n"));
		client.print(F("Content-Type: application/x-www-form-urlencoded\n"));
		client.print(F("Content-Length: "));
		client.print(intToString(ds.ThingSpeakStr.length()) + "\n\n");
		client.println(ds.ThingSpeakStr);
	}
	else if (Settings.Method == GET)
	{
#if DEBUG
		Serial.println("GET");
#endif // DEBUG

		client.print(F("GET /update?key="));
		client.print(ds.APIkey);
		client.print(F("&"));
		client.print(ds.ThingSpeakStr);
		client.print(F(" HTTP/1.1\r\n"));
		client.print(F("Host: "));
		client.print(Settings.ThingSpeakAddress);
		client.print(F("\r\nAccept: */*\r\n"));
		client.print(F("headers: false\r\n"));
		client.print(F("\r\n"));
	}
}
