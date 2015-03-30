void setupPins() {
	//setup led
	for (int i = 0; i < 3; i++)			//setup led pins and switch pins
	{
		pinMode(LED[i], OUTPUT);
	}


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

	pinMode(17, OUTPUT);
	digitalWrite(17, LOW);
	pinMode(16, OUTPUT);
	digitalWrite(16, LOW);
	pinMode(9, OUTPUT);
	digitalWrite(9, LOW);
	
	//tipping bucket
	pinMode(2, INPUT_PULLUP);
	pinMode(7, OUTPUT);
	digitalWrite(7, LOW);

	//setup data pins
	pinMode(DHT22_DATA_PIN, INPUT);
	pinMode(PHOTORESISTOR_DATA_PIN, INPUT);
	pinMode(DS_DATA_PIN, INPUT);
	pinMode(HUMIDITY_DATA_PIN, INPUT);
	digitalWrite(RADIO_TX_PIN, LOW);
	pinMode(RADIO_TX_PIN, OUTPUT);

	//these stupid sensor must be on all the time otherwise they produce shitty readings
	digitalWrite(DS_PWR_PIN, HIGH);
	digitalWrite(DHT22_PWR_PIN, HIGH);

}
