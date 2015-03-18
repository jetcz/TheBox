
float getSysTemperature(sensors_event_t event) {
	float sysTemp;
	bmp.getEvent(&event);
	bmp.getTemperature(&sysTemp);
	rmSysTemp.add(sysTemp += fSysTempOffset);
	return rmSysTemp.getAverage();
}

float getPressure(sensors_event_t event) {
	bmp.getEvent(&event);
	rmPressure.add(event.pressure + iPressureOffset);
	return rmPressure.getAverage();
}


float getMainTemperature() {
	float t;
	t = dht.readTemperature();
	if (!isnan(t)) {
		rmMainTemp.add(t + fMainTempOffset);
	}
	return rmMainTemp.getAverage();
}

float getMainHumidity() {
	float h;
	h = dht.readHumidity();
	if (!isnan(h)) {
		rmMainHumidity.add(h);
	}
	return rmMainHumidity.getAverage();
}

float getMainHumidex() {
	return (dht.computeHeatIndex(rmMainTemp.getAverage()*1.8 + 32, rmMainHumidity.getAverage()) - 32)*0.556;
}

boolean getMainPir() {
	return digitalRead(PIR_PIN);
}


