
float getSysTemperature(sensors_event_t event) {
	static RunningAverage rmSysTemp(6);
	float sysTemp;
	bmp.getEvent(&event);
	bmp.getTemperature(&sysTemp);
	if (sysTemp == 85) return -255;
	rmSysTemp.addValue(sysTemp += fSysTempOffset);
	return rmSysTemp.getAverage();
}

float getPressure(sensors_event_t event) {
	static RunningAverage rmPressure(6);
	bmp.getEvent(&event);
	rmPressure.addValue(event.pressure + iPressureOffset);
	return rmPressure.getAverage();
}


float getMainTemperature() {
	static RunningAverage rmMainTemp(6);
	float t;
	t = dht.readTemperature();
	if (isnan(t)) {
		return -255;
	}
	else
		rmMainTemp.addValue(t + fMainTempOffset);
	return rmMainTemp.getAverage();
}

float getMainHumidity() {
	static RunningAverage rmMainHumidity(6);
	float h;
	h = dht.readHumidity();
	if (isnan(h)) {
		return -255;
	}
	else if (MainDS.Data[0] == -255)
	{
		return -255;
	}
	else
		rmMainHumidity.addValue(h);;
	return rmMainHumidity.getAverage();
}

float getMainHumidex() {
	if (MainDS.Data[0] || MainDS.Data[1] == -255)
	{
		return -255;
	}
	else
		return (dht.computeHeatIndex(MainDS.Data[0] * 1.8 + 32, MainDS.Data[1]) - 32)*0.556;
}

boolean getMainPir() {
	return digitalRead(PIR_PIN);
}


