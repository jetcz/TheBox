void ledLightDigital(char color) {

	switch (color)
	{
	case 'r':
		digitalWrite(LED[0], HIGH);
		digitalWrite(LED[1], LOW);
		digitalWrite(LED[2], LOW);
		//Serial.println("color " + color);
		break;

	case 'g':
		digitalWrite(LED[0], LOW);
		digitalWrite(LED[1], HIGH);
		digitalWrite(LED[2], LOW);
		//Serial.println("color " + color);
		break;

	case 'b':
		digitalWrite(LED[0], LOW);
		digitalWrite(LED[1], LOW);
		digitalWrite(LED[2], HIGH);
		//Serial.println("color " + color);
		break;

	case 'c':
		digitalWrite(LED[0], LOW);
		digitalWrite(LED[1], HIGH);
		digitalWrite(LED[2], HIGH);
		//Serial.println("color " + color);
		break;

	case 'm':
		digitalWrite(LED[0], HIGH);
		digitalWrite(LED[1], LOW);
		digitalWrite(LED[2], HIGH);
		//Serial.println("color " + color);
		break;

	case 'y':
		digitalWrite(LED[0], HIGH);
		digitalWrite(LED[1], HIGH);
		digitalWrite(LED[2], LOW);
		//Serial.println("color " + color);
		break;

	case 'k':
		digitalWrite(LED[0], LOW);
		digitalWrite(LED[1], LOW);
		digitalWrite(LED[2], LOW);
		//Serial.println("color " + color);
		break;

	case 'w':
		digitalWrite(LED[0], HIGH);
		digitalWrite(LED[1], HIGH);
		digitalWrite(LED[2], HIGH);
		//Serial.println("color " + color);
		break;

	default: return;
	}
}

void disco() {
	for (int i = 0; i < 5; i++)
	{
		ledLightDigital('c');
		delay(50);
		ledLightDigital('m');
		delay(50);
		ledLightDigital('y');
		delay(50);
		ledLightDigital('r');
		delay(50);
		ledLightDigital('g');
		delay(50);
		ledLightDigital('b');
		delay(50);
	}
}