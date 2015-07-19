//************************************
// Method:   	 sendMessage
// Description:  Send dataset through radio
// Access:   	 public 
// Returns:  	 void
// Qualifier:	
//************************************
void sendPayload() {
	bool bSucces = false;
	digitalWrite(RADIO_PWR_PIN, HIGH);
	Sleepy::loseSomeTime(250);

	radio.stopListening();

	if (!radio.write(&p, sizeof(p))){
		bSucces = false;
	}
	else bSucces = true;

	digitalWrite(RADIO_PWR_PIN, LOW);

	if (!bSucces) {
		ledLightDigital('r');
		ledLightDigital('k');
	}
}
