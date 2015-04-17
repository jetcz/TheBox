void switchRelays() {
#if DEBUG
	Serial.print(F("Setting relays:"));
#endif
	for (int i = 0; i < 4; i++)
	{

		switch (byRelay[i])
		{
		case 0:
#if DEBUG
			Serial.print(F(" OFF"));
#endif
			digitalWrite(RELAY_PIN[i], HIGH); //HIGH is inactive
			break;
		case 1:
#if DEBUG
			Serial.print(F(" ON"));
#endif
			digitalWrite(RELAY_PIN[i], LOW); //LOW is active
			break;
		default:
#if DEBUG
			Serial.print(F(" AUTO"));
#endif
			//there will be some call for some scheduler function
			break;
		}
	}
#if DEBUG
	Serial.println();
#endif
}

float getRelayState(int relay){
	boolean state = digitalRead(RELAY_PIN[relay]);
	if (state == HIGH) return 0;
	if (state == LOW) return 1;
}


