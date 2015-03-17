float getAirTemperature() {
	//digitalWrite(DHT22_PWR_PIN, HIGH);
	float t = dht.readTemperature();
	//digitalWrite(DHT22_PWR_PIN, LOW);
	// Check if any reads failed and exit early (to try again).
	if (isnan(t)) {
		Serial.println("Failed to read from DHT sensor!");
		return -255;
	}
	else return t;
}

float getLight() {
	//	digitalWrite(PHOTORESISTOR_PWR_PIN, HIGH);
	float l = analogRead(PHOTORESISTOR_DATA_PIN)*readVcc() / 1024;
	//	digitalWrite(PHOTORESISTOR_PWR_PIN, LOW);
	return l;
}

float getSoilTemperature() {
	ds.requestTemperatures();
	return ds.getTempCByIndex(0);
}
