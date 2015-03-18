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
		Serial.print(F("Failed to read from DHT sensor! "));
		return -255;
	}
	else {
		return t;
	}

}

float getAirHumidity(){
	float h = dht.readHumidity();
	if (isnan(h)) {
		Serial.print(F("Failed to read from DHT sensor! "));
		return -255;
	}
	else{
		return h;
	};
}

float getLight() {
	float l = analogRead(PHOTORESISTOR_DATA_PIN)*readVcc() / 1024;
	return l;
}

float getSoilTemperature() {
	ds.requestTemperatures();
	return ds.getTempCByIndex(0);
}

float getSoilHumidity() {
	float h = analogRead(HUMIDITY_DATA_PIN)*readVcc() / 1024;
	return h;
}
