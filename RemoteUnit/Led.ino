/// <summary>
/// Lights up led with specified color
/// </summary>
/// <param name="color">char color (r, g, b, c, m, y, k)</param>
/// <param name="flash">if true, the led will only flash very briefly</param>
void ledLight(char color, bool flash) {

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

	default: break;

	}

	if (flash) ledLight('k', false);
}

/// <summary>
/// Lights up specified led for specified duration in ms
/// </summary>
/// <param name="color"></param>
/// <param name="time"></param>
void ledLight(char color, int time) {
	ledLight(color, false);
	delay(time);
	ledLight('k', false);
}