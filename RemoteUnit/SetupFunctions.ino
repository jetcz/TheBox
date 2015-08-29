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
	pinMode(DHT22_DATA_PIN, INPUT);
	pinMode(PHOTORESISTOR_DATA_PIN, INPUT);
	pinMode(DS_DATA_PIN, INPUT);
	pinMode(HUMIDITY_DATA_PIN, INPUT);

}

/// <summary>
/// Setup radio NRF24
/// </summary>
void setupRadio(){
	bool _bSuccess = false;
	_bSuccess = radio.begin();
	radio.setAutoAck(1);                    // Ensure autoACK is enabled
	//radio.setChannel(24);
	radio.setCRCLength(RF24_CRC_8);
	_bSuccess = _bSuccess | radio.setDataRate(RF24_1MBPS);
	radio.setPALevel(RF24_PA_MAX);
	radio.setRetries(1, byRadioAutoRetransmits);              // Smallest time between retries, max no. of retries
	radio.setPayloadSize(sizeof(Payload));
	radio.openWritingPipe(pipes[1]);
	radio.openReadingPipe(1, pipes[0]);
	radio.startListening();                 // Start listening	


	if (_bSuccess)
	{
#if DEBUG
		Serial.println(F("Radio initialized"));
#endif
		ledLight('g', true);
	}
	else {
#if DEBUG
		Serial.println(F("Radio failed to initialize or not present!"));
#endif	
		ledLight('r', false);
		while(1);	
	}
}
