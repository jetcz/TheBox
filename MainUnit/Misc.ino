
//************************************
// Method:   	 resetEthShield
// Description:  Performs hardware reset of the ethernet shield
// Access:   	 public 
// Returns:  	 void
// Qualifier:	
// Parameter:	 int pin
//************************************
void resetEthShield(int pin) {
	pinMode(pin, OUTPUT);
#if DEBUG
	Serial.println(F("Reseting Ethernet Shield"));
#endif
	digitalWrite(pin, LOW);
	delay(1);
	digitalWrite(pin, HIGH);
	delay(250);
}


//************************************
// Method:   	 isRemoteDataSetValid
// Description:  Whether or not is the remote dataset valid, also lights up led accordingly
// Access:   	 public 
// Returns:  	 bool
// Qualifier:	
//************************************
bool isRemoteDataSetValid(DateTime t) {
	bool _bValid;
	TimeSpan _tsDiff = t - RemoteDS.Timestamp;
	if (_tsDiff.totalseconds() < 65)
	{
		_bValid = true;
		ledLight(2, 'g');
	}

	if (_tsDiff.totalseconds() >= 65 && _tsDiff.totalseconds() <= Settings.RemoteDataSetTimeout)
	{
		_bValid = true;
		ledLight(2, 'y');
	}

	if (_tsDiff.totalseconds() > Settings.RemoteDataSetTimeout)
	{
		_bValid = false;
		ledLight(2, 'r');
	}
	return _bValid;
}

//************************************
// Method:   	 freeRam
// Description:  Amount of free RAM in Bytes
// Access:   	 public 
// Returns:  	 int
// Qualifier:	
//************************************
int freeRam() {
	extern int __heap_start, *__brkval;
	int _nVal;
	return (int)&_nVal - (__brkval == 0 ? (int)&__heap_start : (int)__brkval);
}


//************************************
// Method:   	 readVcc
// Description:  Exact measurement of 5V
// Access:   	 public 
// Returns:  	 long
// Qualifier:	
//************************************
long readVcc() {
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
	delay(2); // Wait for Vref to settle
	ADCSRA |= _BV(ADSC); // Start conversion
	while (bit_is_set(ADCSRA, ADSC)); // measuring
	uint8_t low = ADCL; // must read ADCL first - it then locks ADCH
	uint8_t high = ADCH; // unlocks both
	long result = (high << 8) | low;
	//result = 1125300L / result; // Calculate Vcc (in mV); 1125300 = 1.1*1023*1000
	result = 1122500L / result;
	return result; // Vcc in millivolts
}


//************************************
// Method:   	 getDateTimeString
// Description:  Creates string of supplied DateTime
// Access:   	 public 
// Returns:  	 String
// Qualifier:	
// Parameter:	 DateTime t
//************************************
String getDateTimeString(DateTime t)
{
	sprintf(cBuff1, "%02d.%02d.%04d  %02d:%02d:%02d", t.day(), t.month(), t.year(), t.hour(), t.minute(), t.second());
	return cBuff1;
}

//************************************
// Method:   	 getUptime
// Description:  Returns uptime of the system
// Access:   	 public 
// Returns:  	 TimeSpan
// Qualifier:	
//************************************
TimeSpan getUptime(DateTime t){
	return t - dtSysStart;
}

//************************************
// Method:   	 getUptimeString
// Description:  Returns uptime of the system as a string
// Access:   	 public 
// Returns:  	 String
// Qualifier:	
// Parameter:	 TimeSpan ts
//************************************
String getUptimeString(TimeSpan ts) {
	sprintf(cBuff1, "%dd %02d:%02d:%02d", ts.days(), ts.hours(), ts.minutes(), ts.seconds());
	return cBuff1;
}

time_t syncProvider()     //this does the same thing as RTC_DS1307::get() + gets the local time (respecting timezone and dalylight saving time)
{
	return myTZ.toLocal(rtc.now().unixtime(), &tcr);
}
