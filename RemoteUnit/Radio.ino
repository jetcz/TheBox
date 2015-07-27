//************************************
// Method:   	 sendPayload
// Description:  Send dataset through radio
// Access:   	 public 
// Returns:  	 bool
// Qualifier:	
// Parameter:	 Payload p
//************************************
bool sendPayload() {
	bool _bSucces;
	digitalWrite(RADIO_PWR_PIN, HIGH);
	Sleepy::loseSomeTime(250);

	radio.stopListening();

	if (!radio.write(&p, sizeof(p))){
		_bSucces = false;
	}
	else _bSucces = true;

	digitalWrite(RADIO_PWR_PIN, LOW);

	return _bSucces;
}
