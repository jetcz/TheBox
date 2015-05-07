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

void disco() {
	for (int i = 0; i < 5; i++)
	{
		ledLight(1, 'c');
		ledLight(2, 'm');
		ledLight(3, 'y');
		delay(50);
		ledLight(1, 'y');
		ledLight(2, 'c');
		ledLight(3, 'm');
		delay(50);
		ledLight(1, 'm');
		ledLight(2, 'y');
		ledLight(3, 'c');
		delay(50);
	}
}
