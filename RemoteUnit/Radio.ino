//************************************
// Method:   	 sendMessage
// Description:  Send dataset through radio
// Access:   	 public 
// Returns:  	 void
// Qualifier:	
//************************************
void sendMessage() {
	digitalWrite(RADIO_PWR_PIN, HIGH);
	Sleepy::loseSomeTime(10);

	nrf24.send((uint8_t*)&DS, sizeof(DS));
	nrf24.waitPacketSent();

	digitalWrite(RADIO_PWR_PIN, LOW);	
}
