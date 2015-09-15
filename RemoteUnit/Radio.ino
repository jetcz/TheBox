/// <summary>
/// Send dataset through radio
/// </summary>
/// <returns>Success</returns>
bool sendPayload() {
	bool _bSuccess;
	radio.powerUp();
	radio.stopListening();

	if (radio.write(&payload, sizeof(Payload))) {
		ledLight('g', true);
		_bSuccess = true;
#if DEBUG
		Serial.println(F("Radio message sent, ACK OK"));
#endif
	}
	else
	{
		payload.FailedMsgs++;
		ledLight('r', true);
		_bSuccess = false;
#if DEBUG
		Serial.println(F("Radio message sent, NO ACK!"));
#endif
	}

	radio.powerDown();
	return _bSuccess;
}

/// <summary>
/// Adjust radio parameter according to consecutive failed message count
/// </summary>
void adjustRadio() {

	if (byFailedConsMsgs == 0)
	{
		radio.setRetries(3, 3);
	}
	if (byFailedConsMsgs == 2)
	{
		radio.setRetries(5, 5);
	}
	if (byFailedConsMsgs == 4)
	{
		radio.setRetries(10, 10);
	}
	if (byFailedConsMsgs == 6)
	{
		radio.setRetries(15, 15);
	}
	if (byFailedConsMsgs > 8)
	{
		if (selectChannel() == InvalidValue) radio.setChannel(nChannel);
	}
}
