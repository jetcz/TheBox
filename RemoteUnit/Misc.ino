/// <summary>
/// Exact measurement of reference voltage (5V)
/// </summary>
/// <returns></returns>
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
	delay(1); // Wait for Vref to settle
	ADCSRA |= _BV(ADSC); // Start conversion
	while (bit_is_set(ADCSRA, ADSC)); // measuring
	uint8_t low = ADCL; // must read ADCL first - it then locks ADCH
	uint8_t high = ADCH; // unlocks both
	long result = (high << 8) | low;
	//result = 1125300L / result; // Calculate Vcc (in mV); 1125300 = 1.1*1023*1000
	result = lVccCalibration / result;
	return result; // Vcc in millivolts
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
///  Get the current millis() value for how long the microcontroller has been running
///	 To avoid any possiblity of missing the rollover, we use a bool toggle that gets flipped
///	 off any time during the first half of the total millis period and
///	 then on during the second half of the total millis period.
///	 This would work even if the function were only run once every 4.5 hours, though typically,
///	 the function should be called as frequently as possible to capture the actual moment of rollover.
///	 The rollover counter is good for over 35 years of runtime.
///  Rob Faludi http ://rob.faludi.com
/// </summary>
/// <returns></returns>
int millisRollover() {

	static int _nNumRollovers = 0; // variable that permanently holds the number of rollovers since startup
	static bool _bReadyToRoll = false; // tracks whether we've made it halfway to rollover
	unsigned long _lNow = millis(); // the time right now
	unsigned long _lHalfwayMillis = 2147483647; // this is halfway to the max millis value (17179868 for earlier versions of Arduino)

	if (_lNow > _lHalfwayMillis) { // as long as the value is greater than halfway to the max
		_bReadyToRoll = true; // you are ready to roll over
	}

	if (_bReadyToRoll == true && _lNow < _lHalfwayMillis) {
		// if we've previously made it to halfway
		// and the current millis() value is now _less_ than the halfway mark
		// then we have rolled over
		_nNumRollovers++; // add one to the count the number of rollovers
		_bReadyToRoll = false; // we're no longer past halfway
	}
	return _nNumRollovers;
}


/// <summary>
/// Returns uptime of the system in seconds
/// </summary>
/// <returns></returns>
unsigned long getUptime() {
	static unsigned long _lUptime;
	_lUptime = 4294967 * millisRollover() + round(millis() / 1000);
	return _lUptime;
}