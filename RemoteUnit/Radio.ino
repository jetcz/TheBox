//************************************
// Method:   	 sendPayload
// Description:  Send dataset through radio
// Access:   	 public 
// Returns:  	 bool
// Qualifier:	
//************************************
void sendPayload() {
	bool _bSucces = false;
	digitalWrite(RADIO_PWR_PIN, HIGH);
	Sleepy::loseSomeTime(250);

	for (int i = 0; i < byRadioManualRetransmits; i++)
	{
		radio.stopListening();
		if (radio.write(&payload, sizeof(payload))){
			_bSucces = true;
			break;
		}
		else
		{
			ledLight('y', true);
		}
	}
	if (_bSucces) ledLight('g', true);
	else ledLight('r', true);

	digitalWrite(RADIO_PWR_PIN, LOW);
}
