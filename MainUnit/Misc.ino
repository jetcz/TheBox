/// <summary>
/// Performs hardware reset of the ethernet shield
/// </summary>
void resetEthShield() {
#if DEBUG
	Serial.println(F("Reseting Ethernet Shield"));
#endif
	digitalWrite(RESET_ETH_SHIELD_PIN, LOW);
	Alarm.delay(1);
	digitalWrite(RESET_ETH_SHIELD_PIN, HIGH);
	Alarm.delay(250);
}

/// <summary>
/// Performs hardware reset of the wifi router
/// </summary>
void resetWifi() {
#if DEBUG
	Serial.println(F("Reseting Wifi"));
#endif
	digitalWrite(RESET_WIFI_PIN, LOW);
	Alarm.delay(100);
	digitalWrite(RESET_WIFI_PIN, HIGH);
	Alarm.delay(35000);
	setupEthernet();
}

/// <summary>
/// Check whether ethernet shiled and arduino needs to be restared
/// </summary>
void needRestart() {
	// Check if Ethernet needs to be restarted
	if (nFailedCounter % Settings.RestartEthernetThreshold == 0 && nFailedCounter != 0) {
		ledLight(1, 'r');
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
	if ((nFailedCounter % Settings.RestartWifiThreshold) == 0 && nFailedCounter != 0) {
		ledLight(1, 'r');
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
	if ((nFailedCounter % Settings.RestartArduinoThreshold) == 0 && nFailedCounter != 0) {
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
		resetFunc(); //reboot arduino
	}
}

/// <summary>
/// Whether or not is the remote dataset valid, also lights up led accordingly
/// </summary>
/// <param name="t">Current time</param>
/// <returns></returns>
bool isRemoteDataSetValid(DateTime t) {
	bool _bValid;
	unsigned long _lDiff = t.unixtime() - RemoteDS.TimeStamp.unixtime();
	if (_lDiff <= Settings.RadioMsgInterval)
	{
		_bValid = true;
		ledLight(2, 'g');
	}

	if (_lDiff > Settings.RadioMsgInterval && _lDiff <= Settings.RemoteDataSetTimeout)
	{
		_bValid = true;
		ledLight(2, 'y');
	}

	if (_lDiff > Settings.RemoteDataSetTimeout)
	{
		_bValid = false;
		ledLight(2, 'r');
	}
	return _bValid;
}

/// <summary>
/// Amount of free RAM in Bytes
/// </summary>
/// <returns></returns>
int freeRam() {
	extern int __heap_start, *__brkval;
	int _nVal;
	return (int)&_nVal - (__brkval == 0 ? (int)&__heap_start : (int)__brkval);
}

/// <summary>
/// Exact measurement of reference voltage (5V)
/// </summary>
/// <returns></returns>
float readVcc() {
	static RunningAverage _raVcc(6);
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
	result = lVccCalibration / result;
	_raVcc.addValue(int(result));
	return _raVcc.getAverage();
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
TimeSpan getUptime(DateTime t) {
	return t - dtSysStart;
}

/// <summary>
/// Returns uptime of the system
/// </summary>
/// <param name="ts"></param>
/// <returns>String</returns>
String getUptimeString(TimeSpan ts) {
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