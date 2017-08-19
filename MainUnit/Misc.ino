/// <summary>
/// Performs hardware reset of the ethernet shield
/// </summary>
void resetEthShield()
{
#if DEBUG
	Serial.println(F("Reseting Ethernet Shield"));
#endif
	digitalWrite(RESET_ETH_SHIELD_PIN, LOW);
	delay(1);
	digitalWrite(RESET_ETH_SHIELD_PIN, HIGH);
	delay(250);
}

/// <summary>
/// Performs hardware reset of the wifi router
/// </summary>
void resetWifi()
{
	//wdt_disable();
	ledLight(3, 'k');
#if DEBUG
	Serial.println(F("Reseting Wifi"));
#endif
	if (Alarm.active(updateTSAlarm)) Alarm.disable(updateTSAlarm);
	digitalWrite(RESET_WIFI_PIN, LOW);
	Alarm.delay(100);
	digitalWrite(RESET_WIFI_PIN, HIGH);
	Alarm.delay(36000);
	setupEthernet();
	if (Settings.TSenabled) Alarm.enable(updateTSAlarm);
	//wdt_enable(WDTO_8S);
}


/// <summary>
/// if we lost IP address, ethernet shield most likely died and needs to be reinitialized
/// </summary>
void ethShieldFreezeDetect()
{
	Alarm.disable(ethShieldFreezeDetectAlarm);
#if DEBUG
	Serial.print(F("Checking Ethernet Shield..."));
#endif // DEBUG
	if (Ethernet.localIP() == INADDR_NONE) //ethernet shield froze
	{
#if DEBUG
		Serial.println(F("Freeze!"));
#endif
		setupEthernet();
	}
	else
	{
#if DEBUG
		Serial.println(F("OK"));
#endif // DEBUG
	}
}

/// <summary>
/// Check whether ethernet shield and arduino needs to be restared
/// </summary>
void needRestart()
{
	// Check if Ethernet needs to be restarted
	if (nFailedNetworkOps % Settings.RestartEthernetThreshold == 0 && nFailedNetworkOps != 0)
	{
		ledLight(1, 'k');
		ledLight(3, 'r');
#if DEBUG
		Serial.println(F("Ethernet Shield needs to be restarted!"));
		Serial.println();
#endif
		bLCDRefreshing = false;
		lcd.clear();
		lcd.backlight();
		lcd.setCursor(0, 0);
		lcd.print(F("Ethernet Shield"));
		lcd.setCursor(0, 1);
		lcd.print(F("needs to be"));
		lcd.setCursor(0, 2);
		lcd.print(F("restarted!"));
		setupEthernet();
	}

	// Check if Wifi needs to be restarted
	if ((nFailedNetworkOps % Settings.RestartWifiThreshold) == 0 && nFailedNetworkOps != 0)
	{
		ledLight(1, 'k');
		ledLight(3, 'r');
#if DEBUG
		Serial.println(F("Wifi needs to be restarted!"));
		Serial.println();
#endif
		bLCDRefreshing = false;
		lcd.clear();
		lcd.backlight();
		lcd.setCursor(0, 0);
		lcd.print(F("Wifi needs to be"));
		lcd.setCursor(0, 1);
		lcd.print(F("restarted!"));
		resetWifi();
	}

	// Check if Arduino needs to be restarted
	if ((nFailedNetworkOps % Settings.RestartArduinoThreshold) == 0 && nFailedNetworkOps != 0)
	{
		ledLight(1, 'r');
		ledLight(2, 'r');
		ledLight(3, 'r');
#if DEBUG
		Serial.println(F("Arduino needs to be restarted!"));
		Serial.println();
#endif
		bLCDRefreshing = false;
		lcd.clear();
		lcd.backlight();
		lcd.setCursor(0, 0);
		lcd.print(F("Arduino needs"));
		lcd.setCursor(0, 1);
		lcd.print(F("to be restarted!"));
		Alarm.delay(3000);
		ledLight(1, 'k');
		ledLight(2, 'k');
		ledLight(3, 'k');
		resetFunc(); //reboot arduino
	}

}

/// <summary>
/// Whether or not is the remote dataset valid, also lights up led accordingly
/// </summary>
/// <param name="t">Current time</param>
/// <returns></returns>
bool isRemoteDataSetValid(DateTime t)
{
	bool bValid;
	unsigned long lDiff = t.unixtime() - RemoteDS.TimeStamp.unixtime();
	if (lDiff <= Settings.RadioMsgInterval + 2)
	{
		bValid = true;
		ledLight(2, 'g');
	}
	else if (lDiff > Settings.RadioMsgInterval + 2 && lDiff <= Settings.RemoteDataSetTimeout)
	{
		bValid = true;
		ledLight(2, 'y');
	}
	else if (lDiff > Settings.RemoteDataSetTimeout)
	{
		bValid = false;
		ledLight(2, 'r');
	}

	return bValid;
}

/// <summary>
/// Amount of free RAM in Bytes
/// </summary>
/// <returns></returns>
int freeRam()
{
	extern int __heap_start, *__brkval;
	int _nVal;
	return (int)&_nVal - (__brkval == 0 ? (int)&__heap_start : (int)__brkval);
}

/// <summary>
/// Exact measurement of reference voltage (5V)
/// </summary>
/// <returns></returns>
inline float readVcc()
{
	// Read 1.1V reference against AVcc
	// set the reference to Vcc and the measurement to the internal 1.1V reference
#if defined(__AVR_ATmega32U4__) || defined(__AVR_ATmega1280__) || defined(__AVR_ATmega2560__)
	ADMUX = _BV(REFS0) | _BV(MUX4) | _BV(MUX3) | _BV(MUX2) | _BV(MUX1);
#elif defined (__AVR_ATtiny24__) || defined(__AVR_ATtiny44__) || defined(__AVR_ATtiny84__)
	ADMUX = _BV(MUX5) | _BV(MUX0);
#elif defined (__AVR_ATtiny25__) || defined(__AVR_ATtiny45__) || defined(__AVR_ATtiny85__)
	ADMUX = _BV(MUX3) | _BV(MUX2);
#else
	ADMUX = _BV(REFS0) | _BV(MUX3) | _BV(MUX2) | _BV(MUX1);
#endif
	delay(1); // Wait for Vref to settle
	ADCSRA |= _BV(ADSC); // Start conversion
	while (bit_is_set(ADCSRA, ADSC)); // measuring
	uint8_t low = ADCL; // must read ADCL first - it then locks ADCH
	uint8_t high = ADCH; // unlocks both
	float result = (high << 8) | low;
	//result = 1125300L / result; // Calculate Vcc (in mV); 1125300 = 1.1*1023*1000

	return lVccCalibration / result;
}

/// <summary>
/// Creates string from supplied DateTime
/// </summary>
/// <param name="t">Current date time</param>
/// <returns></returns>
String getDateTimeString(DateTime t)
{
	sprintf(cBuff1, "%02d.%02d.%04d  %02d:%02d:%02d", t.day(), t.month(), t.year(), t.hour(), t.minute(), t.second());
	return cBuff1;
}

/// <summary>
/// Returns uptime of the system
/// </summary>
/// <param name="t"></param>
/// <returns>Datetime</returns>
TimeSpan getUptime(DateTime t)
{
	return t - dtSysStart;
}

/// <summary>
/// Returns uptime of the system
/// </summary>
/// <param name="ts"></param>
/// <returns>String</returns>
String getUptimeString(TimeSpan ts)
{
	sprintf(cBuff1, "%dd %02d:%02d:%02d", ts.days(), ts.hours(), ts.minutes(), ts.seconds());
	return cBuff1;
}

/// <summary>
/// This does the same thing as RTC_DS1307::get() + gets the local time (respecting timezone and dalylight saving time)
/// </summary>
/// <returns></returns>
time_t syncProvider()
{
	return myTZ.toLocal(rtc.now().unixtime(), &tcr);
}

/// <summary>
/// Resolves hostname into IP address. The resolved address should be stored inside the caller function for future reuse.
/// Also handles led, lcd and serial.
/// </summary>
/// <param name="addr">where tu put resolved address</param>
/// <param name="host">host to resolve</param>
/// <returns>true if resolved successfull</returns>
bool resolveHost(IPAddress &addr, char &host)
{
	ledLight(3, 'c');
	DNSClient dns;
	bool bSuccess;
	dns.begin(Ethernet.dnsServerIP());
	if (dns.getHostByName(&host, addr) == 1)
	{
#if DEBUG
		Serial.print(F("Resolved hostname: "));
		Serial.print(&host);
		Serial.print(F(" into IP: "));
		Serial.println(addr);
#endif // DEBUG		
		ledLight(3, 'g');
		nFailedNetworkOps = 0;
		bSuccess = true;
	}
	else
	{
#if DEBUG
		Serial.println(F("Failed to resolve hostname: "));
		Serial.print(&host);
#endif // DEBUG
		ledLight(3, 'r');
		bLCDRefreshing = false;
		lcd.clear();
		lcd.setCursor(0, 0);
		lcd.print(F("Failed to resolve"));
		lcd.setCursor(0, 1);
		lcd.print(&host);
		nFailedNetworkOps++;
		bSuccess = false;

	}
	return bSuccess;
}

#if DEBUG

void ShowSockStatus()
{
	Serial.println();
	byte socketStat[MAX_SOCK_NUM];
	for (int i = 0; i < MAX_SOCK_NUM; i++) {
		Serial.print(F("Socket#"));
		Serial.print(i);
		uint8_t s = W5100.readSnSR(i);
		socketStat[i] = s;
		Serial.print(F(":0x"));
		Serial.print(s, 16);
		Serial.print(F(" "));
		Serial.print(W5100.readSnPORT(i));
		Serial.print(F(" D:"));
		uint8_t dip[4];
		W5100.readSnDIPR(i, dip);
		for (int j = 0; j < 4; j++) {
			Serial.print(dip[j], 10);
			if (j < 3) Serial.print(".");
		}
		Serial.print(F("("));
		Serial.print(W5100.readSnDPORT(i));
		Serial.println(F(")"));
	}
}

#endif
