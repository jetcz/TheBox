/// <summary>
/// Setup pins
/// </summary>
void setupPins() {
	//setup led
	for (int i = 0; i < 3; i++)			//setup led pins and switch pins
	{
		pinMode(LED[i], OUTPUT);
	}

	//setup power pins	
	digitalWrite(DHT22_PWR_PIN, LOW);
	digitalWrite(PHOTORESISTOR_PWR_PIN, LOW);
	digitalWrite(HUMIDITY_PWR_PIN, LOW);
	pinMode(DHT22_PWR_PIN, OUTPUT);
	pinMode(PHOTORESISTOR_PWR_PIN, OUTPUT);
	pinMode(HUMIDITY_PWR_PIN, OUTPUT);


	//tipping bucket
	pinMode(2, INPUT_PULLUP);
	pinMode(6, OUTPUT);
	digitalWrite(6, LOW);

	//setup data pins
	//pinMode(DHT22_DATA_PIN, INPUT); //not needed, setting up is done each time when powering up the sensor, refer to powerSensors()
	pinMode(PHOTORESISTOR_DATA_PIN, INPUT);
	pinMode(DS_DATA_PIN, INPUT);
	pinMode(HUMIDITY_DATA_PIN, INPUT);
}

/// <summary>
/// Setup radio NRF24
/// </summary>
void setupRadio() {
	radio.begin(); //for some reasong it returns false even tough radio initialized successfuly
	radio.setAutoAck(1); // Ensure autoACK is enabled
	//radio.setChannel(24);
	radio.setCRCLength(RF24_CRC_8);
	radio.setDataRate(RF24_1MBPS);
	radio.setPALevel(RF24_PA_MAX);
	radio.setRetries(3, 3); // Smallest time between retries, max no. of retries
	radio.setPayloadSize(sizeof(Payload));
	radio.openWritingPipe(pipes[0]);
	radio.openReadingPipe(1, pipes[1]);
}

/// <summary>
/// This sends payload on available channels until ACK is received on one of them. Do not use channels 126 and 127, for some reason they don't work correctly on my units.
/// </summary>
/// <returns>channel</returns>
int selectChannel() {
#if !RADIO
	return 0;
#endif // !RADIO

	unsigned long _lStartTime = millis();
	byte _byChannel = 0;
	radio.setRetries(5, 2);
	radio.powerUp();
	while (!radio.write(&payload, sizeof(Payload)))
	{
		ledLight('m', true);
		_byChannel++;
		if (_byChannel > 125) _byChannel = 0;
		radio.setChannel(_byChannel);
		if (millis() - _lStartTime > 4000) return InvalidValue;
	}
	radio.powerDown();
	radio.setRetries(3, 3);
	byFailedConsMsgs = 0;
	return _byChannel;
}
