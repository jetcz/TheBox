/// <summary>
/// Send dataset through radio
/// </summary>
void sendPayload() {
	radio.powerUp();
	radio.stopListening();

	if (radio.write(&payload, sizeof(Payload))) {
		ledLight('g', true);
#if DEBUG
		Serial.println(F("Radio message sent, ACK OK"));
#endif
	}
	else
	{
		payload.FailedMsgs++;
		ledLight('r', true);
#if DEBUG
		Serial.println(F("Radio message sent, NO ACK!"));
#endif
	}

	radio.powerDown();
}
