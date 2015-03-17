void setupPins() {

	//setup power pins	
	pinMode(PHOTORESISTOR_PWR_PIN, OUTPUT);
	pinMode(DS_PWR_PIN, OUTPUT);
	pinMode(HUMIDITY_PWR_PIN, OUTPUT);
	pinMode(RADIO_PWR_PIN, OUTPUT);
	digitalWrite(DHT22_PWR_PIN, LOW);
	digitalWrite(PHOTORESISTOR_PWR_PIN, LOW);
	digitalWrite(DS_PWR_PIN, LOW);
	digitalWrite(HUMIDITY_PWR_PIN, LOW);
	digitalWrite(RADIO_PWR_PIN, LOW);

	//setup data pins
	//pinMode(DHT22_DATA_PIN, INPUT);
	pinMode(PHOTORESISTOR_DATA_PIN, INPUT);
	pinMode(DS_DATA_PIN, INPUT);
	pinMode(HUMIDITY_DATA_PIN, INPUT);
	pinMode(RADIO_DATA_PIN, OUTPUT);
	digitalWrite(RADIO_DATA_PIN, LOW);

	//setup led
	pinMode(LED[0], OUTPUT);
	pinMode(LED[1], OUTPUT);
	pinMode(LED[2], OUTPUT);
	digitalWrite(LED[0], LOW);
	digitalWrite(LED[1], LOW);
	digitalWrite(LED[2], LOW);

}
