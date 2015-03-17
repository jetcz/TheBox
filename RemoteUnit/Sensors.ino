void powerSensors(boolean state) {
	if (state)
	{
		digitalWrite(DHT22_PWR_PIN, HIGH);
		digitalWrite(PHOTORESISTOR_PWR_PIN, HIGH);
		digitalWrite(DS_PWR_PIN, HIGH);
		digitalWrite(HUMIDITY_PWR_PIN, HIGH);
	}
	else
	{
		digitalWrite(DHT22_PWR_PIN, LOW);
		digitalWrite(PHOTORESISTOR_PWR_PIN, LOW);
		//digitalWrite(DS_PWR_PIN, LOW);  // we cant just turn off the sensor
		digitalWrite(HUMIDITY_PWR_PIN, LOW);
	}
}

float getAirTemperature() {
	float t = dht.readTemperature();
	if (isnan(t)) {
		Serial.print("Failed to read from DHT sensor! ");
		return -255;
	}
	else {
		rmAirTemperature.add(t);
		return rmAirTemperature.getAverage();
	}

}

float getAirHumidity(){
	float h = dht.readHumidity();
	if (isnan(h)) {
		Serial.print("Failed to read from DHT sensor! ");
		return -255;
	}
	else{
		rmAirHumidity.add(h);
		return rmAirHumidity.getAverage();
	};
}

float getLight() {
	float l = analogRead(PHOTORESISTOR_DATA_PIN)*readVcc() / 1024;
	rmLight.add(l);
	return rmLight.getAverage();
}

float getSoilTemperature() {
	ds.requestTemperatures();
	rmSoilTemperature.add(ds.getTempCByIndex(0));
	return rmSoilTemperature.getAverage();
}

float getSoilHumidity() {
	float h = analogRead(HUMIDITY_DATA_PIN)*readVcc() / 1024;
	rmSoilHumidity.add(h);
	return rmSoilHumidity.getAverage();
}
