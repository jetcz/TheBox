/* reset ethernet shield */
void resetEthShield(int pin) {
	pinMode(pin, OUTPUT);
	Serial.println(F("Reseting Ethernet Shield"));
	digitalWrite(pin, LOW);
	delay(1);
	digitalWrite(pin, HIGH);
	delay(250);
}

boolean isRemoteDataSetValid() {
	if (now() - RemoteDS.Timestamp.unixtime() > iRemoteDataSetTimeout)
	{
		ledLight(2, 'y');
		if ((now() - RemoteDS.Timestamp.unixtime()) > (2 * iRemoteDataSetTimeout))
		{
			ledLight(2, 'r');
		}
		return false;
	}
	else {
		ledLight(2, 'g');
		return true;
	}
}


int freeRam() {
	extern int __heap_start, *__brkval;
	int v;
	return (int)&v - (__brkval == 0 ? (int)&__heap_start : (int)__brkval);
}
/* we dont need this for now
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
result = 1125300L / result; // Calculate Vcc (in mV); 1125300 = 1.1*1023*1000
//result = 1122500L / result;
return result; // Vcc in millivolts
}
*/


String getDateTimeString(DateTime t)
{
	sprintf(charVal, "%04d-%02d-%02d  %02d:%02d:%02d", t.year(), t.month(), t.day(), t.hour(), t.minute(), t.second());
	return charVal;
}

TimeSpan getUptime(){
	return DateTime(now()) - sysStart;
}

String getUptimeString(TimeSpan ts) {
	sprintf(charVal, "%dd %02d:%02d:%02d", ts.days(), ts.hours(), ts.minutes(), ts.seconds());
	return charVal;
}

time_t syncProvider()     //this does the same thing as RTC_DS1307::get()
{
	return rtc.now().unixtime();
}
