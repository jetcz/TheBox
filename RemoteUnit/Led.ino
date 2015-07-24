//************************************
// Method:   	 ledLight
// Description:  Lights up led with specified color
// Access:   	 public 
// Returns:  	 void
// Qualifier:	
// Parameter:	 char color (r, g, b, c, m, y, k)
// Parameter:	 bool flash - if true, the led will only flash very briefly
//************************************
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

	default: return;

		if (flash) ledLight('k', false);
	}
}