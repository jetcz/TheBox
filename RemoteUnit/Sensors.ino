void prepareDataSetArrays() {


	fRemoteUnitDataSet[0] = getAirTemperature();				//remoteTemperature
	fRemoteUnitDataSet[1] = getAirHumidity();					//remoteHumidity
	fRemoteUnitDataSet[2] = getAirHumidex();					//remoteHumidex

	powerSensors(true);
	fRemoteUnitDataSet[7] = readVcc();
	fRemoteUnitDataSet[3] = getSoilTemperature();				//remoteSoilTemperature

	fRemoteUnitDataSet[4] = getSoilHumidity();					//remoteSoilHumidity
	fRemoteUnitDataSet[5] = getLight();							//remoteLight
	powerSensors(false);

	fRemoteUnitDataSet[6] = getRainPerHour();					//rain
	fRemoteUnitDataSet[8] = getUptime();						//uptime

}

void printSensorData() {
	Serial.print(F("Air temp: "));
	Serial.print(fRemoteUnitDataSet[0], 1);
	Serial.println(F("C"));

	Serial.print(F("Air hum: "));
	Serial.print(fRemoteUnitDataSet[1], 0);
	Serial.println(F("%RH"));

	Serial.print(F("Humidex: "));
	Serial.print(fRemoteUnitDataSet[2], 1);
	Serial.println(F("C"));

	Serial.print(F("Soil temp: "));
	Serial.print(fRemoteUnitDataSet[3], 1);
	Serial.println(F("C"));

	Serial.print(F("Soil hum: "));
	Serial.print(fRemoteUnitDataSet[4], 0);
	Serial.println(F("%"));

	Serial.print(F("Light: "));
	Serial.print(fRemoteUnitDataSet[5], 0);
	Serial.println(F("%"));

	Serial.print(F("Rain: "));
	Serial.print(fRemoteUnitDataSet[6], 1);
	Serial.println(F("mm/h"));

	Serial.print(F("Vcc: "));
	Serial.print(fRemoteUnitDataSet[7], 0);
	Serial.println(F("mV"));

	Serial.print(F("Uptime: "));
	Serial.print(fRemoteUnitDataSet[8], 0);
	Serial.println("s");

	Serial.print(F("Free ram: "));
	Serial.print(freeRam());
	Serial.println("b");

	Serial.println();
}

void powerSensors(bool state) {
	if (state)
	{
		digitalWrite(PHOTORESISTOR_PWR_PIN, HIGH);
		digitalWrite(HUMIDITY_PWR_PIN, HIGH);
	}
	else
	{
		digitalWrite(PHOTORESISTOR_PWR_PIN, LOW);
		digitalWrite(HUMIDITY_PWR_PIN, LOW);
	}
}

float getAirTemperature() {
	float t = dht.readTemperature();
	if (isnan(t)) {
#if debug
		Serial.print(F("Failed to read from DHT sensor! "));
#endif
		return -255;
	}
	else {
		AirTemp.addValue(t + fAirTemperatureOffset);
		return AirTemp.getAverage();
	}

}

float getAirHumidity(){
	float h = dht.readHumidity();
	if (isnan(h) || (fRemoteUnitDataSet[0] == -255)) {
#if debug
		Serial.print(F("Failed to read from DHT sensor! "));
#endif
		return -255;
	}
	else {
		AirHum.addValue(h);
		return AirHum.getAverage();
	};
}

float getAirHumidex() {
	if ((fRemoteUnitDataSet[0] == -255) || (fRemoteUnitDataSet[1] == -255))
	{
		return -255;
	}
	else
		return (dht.computeHeatIndex(AirTemp.getAverage()*1.8 + 32, AirHum.getAverage()) - 32)*0.556;
}

byte getLight() {
	float l = analogRead(PHOTORESISTOR_DATA_PIN)*(*Vcc) / 1023;
	byte light = ((l / *Vcc) * 100);
	Light.addValue(light);
	return Light.getAverage();
}

float getSoilTemperature() {
	ds.requestTemperatures();
	float t = ds.getTempCByIndex(0);
	if (t == 85) return -255;
	SoilTemp.addValue(t + fSoilTemperatureOffset);
	return SoilTemp.getAverage();
}

//returns soil humidity percentage 0 = air, 100 = salt water
byte getSoilHumidity() {
	float h = analogRead(HUMIDITY_DATA_PIN)*(*Vcc) / 1023;
	byte hum = ((h / *Vcc - 1)*-115);
	SoilHum.addValue(hum);
	return SoilHum.getAverage();
}

float getRainPerHour() {

	if (getUptime() - previousSec > interval) {
		previousSec = getUptime();
		noInterrupts();
		nRainTips = 0;
		interrupts();
	}
	noInterrupts();
	float fRainPerHour = float(nRainTips) * 0.3;
	interrupts();

	return fRainPerHour;
}

void ISRTipCnt() {
	nRainTips++;
}
