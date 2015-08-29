/// <summary>
/// Lights up specified led with specified color
/// </summary>
/// <param name="led">int led (1, 2, 3)</param>
/// <param name="color">char color (r, g, b, c, m, y, k)</param>
void ledLight(int led, char color) {
	if (led == 1)
	{
		switch (color)
		{
		case 'r':
			analogWrite(LED1[0], Settings.LightIntensity[0]);
			analogWrite(LED1[1], 0);
			analogWrite(LED1[2], 0);
			//Serial.println("LED " + String(led) + " color " + color);
			break;
		case 'g':
			analogWrite(LED1[0], 0);
			analogWrite(LED1[1], Settings.LightIntensity[1]);
			analogWrite(LED1[2], 0);
			//Serial.println("LED " + String(led) + " color " + color);
			break;
		case 'b':
			analogWrite(LED1[0], 0);
			analogWrite(LED1[1], 0);
			analogWrite(LED1[2], Settings.LightIntensity[2]);
			//Serial.println("LED " + String(led) + " color " + color);
			break;
		case 'c':
			analogWrite(LED1[0], 0);
			analogWrite(LED1[1], Settings.LightIntensity[1]);
			analogWrite(LED1[2], Settings.LightIntensity[2]);
			//Serial.println("LED " + String(led) + " color " + color);
			break;
		case 'm':
			analogWrite(LED1[0], Settings.LightIntensity[0]);
			analogWrite(LED1[1], 0);
			analogWrite(LED1[2], Settings.LightIntensity[2]);
			//Serial.println("LED " + String(led) + " color " + color);
			break;
		case 'y':
			analogWrite(LED1[0], Settings.LightIntensity[0]);
			analogWrite(LED1[1], Settings.LightIntensity[1]);
			analogWrite(LED1[2], 0);
			//Serial.println("LED " + String(led) + " color " + color);
			break;
		case 'k':
			analogWrite(LED1[0], 0);
			analogWrite(LED1[1], 0);
			analogWrite(LED1[2], 0);
			//Serial.println("LED " + String(led) + " color " + color);
			break;
		case 'w':
			analogWrite(LED1[0], Settings.LightIntensity[0]);
			analogWrite(LED1[1], Settings.LightIntensity[1]);
			analogWrite(LED1[2], Settings.LightIntensity[2]);
			//Serial.println("LED " + String(led) + " color " + color);
			break;
		default:
			return;
		}
	}

	if (led == 2) {
		switch (color)
		{
		case 'r':
			analogWrite(LED2[0], Settings.LightIntensity[0]);
			analogWrite(LED2[1], 0);
			analogWrite(LED2[2], 0);
			//Serial.println("LED " + String(led) + " color " + color);
			break;
		case 'g':

			analogWrite(LED2[0], 0);
			analogWrite(LED2[1], Settings.LightIntensity[1]);
			analogWrite(LED2[2], 0);
			//Serial.println("LED " + String(led) + " color " + color);
			break;
		case 'b':
			analogWrite(LED2[0], 0);
			analogWrite(LED2[1], 0);
			analogWrite(LED2[2], Settings.LightIntensity[2]);
			//Serial.println("LED " + String(led) + " color " + color);
			break;
		case 'c':
			analogWrite(LED2[0], 0);
			analogWrite(LED2[1], Settings.LightIntensity[1]);
			analogWrite(LED2[2], Settings.LightIntensity[2]);
			//Serial.println("LED " + String(led) + " color " + color);
			break;
		case 'm':
			analogWrite(LED2[0], Settings.LightIntensity[0]);
			analogWrite(LED2[1], 0);
			analogWrite(LED2[2], Settings.LightIntensity[2]);
			//Serial.println("LED " + String(led) + " color " + color);
			break;
		case 'y':
			analogWrite(LED2[0], Settings.LightIntensity[0]);
			analogWrite(LED2[1], Settings.LightIntensity[1]);
			analogWrite(LED2[2], 0);
			//Serial.println("LED " + String(led) + " color " + color);
			break;
		case 'k':
			analogWrite(LED2[0], 0);
			analogWrite(LED2[1], 0);
			analogWrite(LED2[2], 0);
			//Serial.println("LED " + String(led) + " color " + color);
			break;
		case 'w':
			analogWrite(LED2[0], Settings.LightIntensity[0]);
			analogWrite(LED2[1], Settings.LightIntensity[1]);
			analogWrite(LED2[2], Settings.LightIntensity[2]);
			//Serial.println("LED " + String(led) + " color " + color);
			break;
		default:
			return;

		}
	}

	if (led == 3){
		switch (color)
		{
		case 'r':
			analogWrite(LED3[0], Settings.LightIntensity[0]);
			analogWrite(LED3[1], 0);
			analogWrite(LED3[2], 0);
			//Serial.println("LED " + String(led) + " color " + color);
			break;
		case 'g':

			analogWrite(LED3[0], 0);
			analogWrite(LED3[1], Settings.LightIntensity[1]);
			analogWrite(LED3[2], 0);
			//Serial.println("LED " + String(led) + " color " + color);

			break;
		case 'b':
			analogWrite(LED3[0], 0);
			analogWrite(LED3[1], 0);
			analogWrite(LED3[2], Settings.LightIntensity[2]);
			//Serial.println("LED " + String(led) + " color " + color);
			break;
		case 'c':
			analogWrite(LED3[0], 0);
			analogWrite(LED3[1], Settings.LightIntensity[1]);
			analogWrite(LED3[2], Settings.LightIntensity[2]);
			//Serial.println("LED " + String(led) + " color " + color);
			break;
		case 'm':
			analogWrite(LED3[0], Settings.LightIntensity[0]);
			analogWrite(LED3[1], 0);
			analogWrite(LED3[2], Settings.LightIntensity[2]);
			//Serial.println("LED " + String(led) + " color " + color);
			break;
		case 'y':
			analogWrite(LED3[0], Settings.LightIntensity[0]);
			analogWrite(LED3[1], Settings.LightIntensity[1]);
			analogWrite(LED3[2], 0);
			//Serial.println("LED " + String(led) + " color " + color);
			break;
		case 'k':
			analogWrite(LED3[0], 0);
			analogWrite(LED3[1], 0);
			analogWrite(LED3[2], 0);
			//Serial.println("LED " + String(led) + " color " + color);
			break;
		case 'w':
			analogWrite(LED3[0], Settings.LightIntensity[0]);
			analogWrite(LED3[1], Settings.LightIntensity[1]);
			analogWrite(LED3[2], Settings.LightIntensity[2]);
			//Serial.println("LED " + String(led) + " color " + color);
			break;
		default:
			return;
		}
	}
}