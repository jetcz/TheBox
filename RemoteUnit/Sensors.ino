void powerSensors(boolean state) {
	if (state)
	{
		digitalWrite(DHT22_PWR_PIN, HIGH);
		digitalWrite(PHOTORESISTOR_PWR_PIN, HIGH);
		digitalWrite(HUMIDITY_PWR_PIN, HIGH);
	}
	else
	{
		digitalWrite(DHT22_PWR_PIN, LOW);
		digitalWrite(PHOTORESISTOR_PWR_PIN, LOW);
		digitalWrite(HUMIDITY_PWR_PIN, LOW);
	}
}

float getAirTemperature() {
	float t = dht.readTemperature();
	if (isnan(t)) {
		Serial.print(F("Failed to read from DHT sensor! "));
		return -255;
	}
	else {
		AirTemp.addValue(t + fAirTemperatureOffset);
		return AirTemp.getAverage();
	}

}

float getAirHumidity(){
	float h = dht.readHumidity();
	if (isnan(h)) {
		Serial.print(F("Failed to read from DHT sensor! "));
		return -255;
	}
	else{
		AirHum.addValue(h);
		return AirHum.getAverage();
	};
}

float getAirHumidex() {
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
	SoilTemp.addValue(ds.getTempCByIndex(0) + fSoilTemperatureOffset);
	return SoilTemp.getAverage();
}

//returns soil humidity percentage 0 = air, 100 = salt water
byte getSoilHumidity() {
	float h = analogRead(HUMIDITY_DATA_PIN)*(*Vcc) / 1023;
	byte hum = ((h / *Vcc - 1)*-115);
	SoilHum.addValue(hum);
	return SoilHum.getAverage();
}

void ISRTipCnt() {
	nRainTicks++;
}
