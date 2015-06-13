
float getSysTemperature(sensors_event_t event) {
	static RunningAverage _raSysTemp(6);
	float _fSysTemp;
	bmp.getEvent(&event);
	bmp.getTemperature(&_fSysTemp);
	if (_fSysTemp == 85) return -255;
	_raSysTemp.addValue(_fSysTemp);
	return _raSysTemp.getAverage();
}

float getPressure(sensors_event_t event) {
	if (SystemDS.Data[0] == -255) return -255;
	static RunningAverage _raPressure(6);
	bmp.getEvent(&event);
	_raPressure.addValue(event.pressure);
	return _raPressure.getAverage();
}


float getMainTemperature() {
	static RunningAverage _raMainTemp(6);
	float _fTemp;
	_fTemp = dht.readTemperature();
	if (isnan(_fTemp)) {
		return -255;
	}
	else
		_raMainTemp.addValue(_fTemp);
	return _raMainTemp.getAverage();
}

float getMainHumidity() {
	static RunningAverage _raMainHumidity(6);
	float _fHum;
	_fHum = dht.readHumidity();
	if (isnan(_fHum)) {
		return -255;
	}
	else if (MainDS.Data[0] == -255)
	{
		return -255;
	}
	else
		_raMainHumidity.addValue(_fHum);;
	return _raMainHumidity.getAverage();
}

float getMainHumidex() {
	if ((MainDS.Data[0] == -255) || (MainDS.Data[1] == -255))
	{
		return -255;
	}
	else
	{
		float e;
		e = (6.112 * pow(10, (7.5 * MainDS.Data[0] / (237.7 + MainDS.Data[0]))) *  MainDS.Data[1] / 100); //vapor pressure
		float humidex = MainDS.Data[0] + 0.55555555 * (e - 10.0); //humidex
		return humidex;
	}
}

bool getMainPir() {
	return digitalRead(PIR_PIN);
}

float getPower(int relay){
	static RunningAverage _raCurr0(6);
	static RunningAverage _raCurr3(6);

	if (relay == 0)
	{
		if (getRelayState(0) == 0) _raCurr0.clear();
		else if (emon.realPower1 < 0 || emon.powerFactor1 < 0) {}
		else
			_raCurr0.addValue(emon.realPower1);
		return _raCurr0.getAverage();
	}

	if (relay == 3)
	{
		if (getRelayState(3) == 0) _raCurr3.clear();
		else if (emon.realPower2 < 0 || emon.powerFactor2 < 0) {}
		else
			_raCurr3.addValue(emon.realPower2);
		return _raCurr3.getAverage();
	}
	return 0;
}

float getVoltage(){
	static RunningAverage _raVoltage(6);
	_raVoltage.addValue((emon.Vrms));
	return _raVoltage.getAverage();
}

