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
