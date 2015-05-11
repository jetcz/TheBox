//************************************
// Method:   	 sendMessage
// Description:  Send dataset through radio
// Access:   	 public 
// Returns:  	 void
// Qualifier:	
//************************************
void sendMessage() {
	digitalWrite(RADIO_PWR_PIN, HIGH);
	driver.send((uint8_t*)&fRemoteUnitDataSet, sizeof(fRemoteUnitDataSet));
	driver.waitPacketSent();
	digitalWrite(RADIO_PWR_PIN, LOW);
	
}
