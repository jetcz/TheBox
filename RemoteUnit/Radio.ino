//************************************
// Method:   	 sendMessage
// Description:  Send dataset through radio
// Access:   	 public 
// Returns:  	 void
// Qualifier:	
//************************************
void sendMessage() {
	digitalWrite(RADIO_PWR_PIN, HIGH);
	driver.send((uint8_t*)&DS, sizeof(DS));
	driver.waitPacketSent();
	digitalWrite(RADIO_PWR_PIN, LOW);	
}
