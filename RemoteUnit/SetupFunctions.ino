void setupPins() {
	//setup led
	for (int i = 0; i < 3; i++)			//setup led pins and switch pins
	{
		pinMode(LED[i], OUTPUT);
	}
	//ledLightDigital('k');

	//setup power pins	
	digitalWrite(DHT22_PWR_PIN, LOW);
	digitalWrite(PHOTORESISTOR_PWR_PIN, LOW);
	digitalWrite(DS_PWR_PIN, LOW);
	digitalWrite(HUMIDITY_PWR_PIN, LOW);
	digitalWrite(RADIO_PWR_PIN, LOW);
	pinMode(DHT22_PWR_PIN, OUTPUT);
	pinMode(PHOTORESISTOR_PWR_PIN, OUTPUT);
	pinMode(DS_PWR_PIN, OUTPUT);
	pinMode(HUMIDITY_PWR_PIN, OUTPUT);
	pinMode(RADIO_PWR_PIN, OUTPUT);


	//setup data pins
	//pinMode(DHT22_DATA_PIN, INPUT);
	pinMode(PHOTORESISTOR_DATA_PIN, INPUT);
	pinMode(DS_DATA_PIN, INPUT);
	pinMode(HUMIDITY_DATA_PIN, INPUT);
	digitalWrite(RADIO_DATA_PIN, LOW);
	pinMode(RADIO_DATA_PIN, OUTPUT);

}
