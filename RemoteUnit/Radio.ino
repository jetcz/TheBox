/// <summary>
/// Send dataset through radio
/// </summary>
void sendPayload() {
	radio.powerUp();
	radio.stopListening();

	if (radio.write(&payload, sizeof(payload))){
#if DEBUG
		Serial.println(F("Radio message sent, ACK OK"));
#endif
		ledLight('g', true);

	}
	else
	{
		payload.FailedMsgs++;
#if DEBUG
		Serial.println(F("Radio message sent, NO ACK!"));
#endif
		ledLight('r', true);
	}

	radio.powerDown();
}
