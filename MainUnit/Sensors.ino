
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
		return (dht.computeHeatIndex(MainDS.Data[0] * 1.8 + 32, MainDS.Data[1]) - 32)*0.556;
}

bool getMainPir() {
	return digitalRead(PIR_PIN);
}


