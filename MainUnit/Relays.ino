void switchRelays() {
	Serial.print(F("Setting relays:"));
	for (int i = 0; i < 4; i++)
	{

		switch (byRelay[i])
		{
		case 0:
			Serial.print(F(" OFF"));
			digitalWrite(RELAY_PIN[i], HIGH); //HIGH is inactive
			break;
		case 1:
			Serial.print(F(" ON"));
			digitalWrite(RELAY_PIN[i], LOW); //LOW is active
			break;
		default:
			Serial.print(F(" AUTO"));
			//there will be some call for some scheduler function
			break;
		}
	}
	Serial.println();
}

float getRelayState(int relay){
	boolean state = digitalRead(RELAY_PIN[relay]);
	if (state == HIGH) return 0;
	if (state == LOW) return 1;
}


