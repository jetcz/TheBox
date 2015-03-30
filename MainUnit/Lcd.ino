
void printLcdScreen1() {

	//line 1
	lcd.setCursor(0, 0);
	lcd.print(MainDS.Data[0], 1);
	lcd.print(F("C"));
	lcd.setCursor(19 - floatToString(RemoteDS.Data[0]).length(), 0);
	lcd.print(RemoteDS.Data[0], 1);
	lcd.print(F("C"));

	//line 2
	lcd.setCursor(0, 1);
	lcd.print(MainDS.Data[1], 1);
	lcd.print(F("%RH"));
	lcd.setCursor(17 - floatToString(RemoteDS.Data[1]).length(), 1);
	lcd.print(RemoteDS.Data[1], 1);
	lcd.print(F("%RH"));

	//line 3
	lcd.setCursor(0, 2);
	lcd.print(F("Weather:"));
	lcd.setCursor(20 - (strlen(weather[forecast])), 2);
	lcd.print(weather[forecast]);
	//line 4
	lcd.setCursor(0, 3);
	lcd.print(sNow);

}

void printLcdScreen2() {
	//line 1 humidex	soiltemp
	lcd.setCursor(0, 0);
	lcd.print(RemoteDS.Data[2], 1);
	lcd.print(F("C/H")); //humidex
	lcd.setCursor(19 - floatToString(RemoteDS.Data[3]).length(), 0);
	lcd.print(RemoteDS.Data[3],1);
	lcd.print(F("C")); //soil temp

	//line 2
	lcd.setCursor(0, 1);
	lcd.print(RemoteDS.Data[5], 1); //light
	lcd.print(F("%L"));
	lcd.setCursor(17 - floatToString(RemoteDS.Data[4]).length(), 1);
	lcd.print(RemoteDS.Data[4], 1);
	lcd.print(F("%RH")); //soil hum

	//line 3 
	lcd.setCursor(0, 2);
	lcd.print(MainDS.Data[4], 1); //pressure
	lcd.print(F("hPa"));
	lcd.setCursor(16 - floatToString(RemoteDS.Data[6]).length(), 2);
	lcd.print(RemoteDS.Data[6], 1); //rain
	lcd.print(F("mm/h"));

}

void printLcdScreen3() {
	//line 3
	lcd.setCursor(0, 2);
	lcd.print(SystemDS.Data[0], 1);
	lcd.print(F("C"));
	lcd.setCursor(20 - sMainUptime.length(), 2);
	lcd.print(sMainUptime);

	//line 4
	lcd.setCursor(0, 3);
	lcd.print(SystemDS.Data[6], 0);
	lcd.print(F("mV"));
	lcd.setCursor(20 - sRemoteUptime.length(), 3);
	lcd.print(sRemoteUptime);

}

void lcdBacklight() {

	if (getMainPir())
	{
		lcd.backlight();
	}
	else
	{
		lcd.noBacklight();
	}
}


/*	//line 1
	lcd.setCursor(0, 0);
	lcd.print(MainDS.Data[0], 1);
	lcd.print(F("C"));
	lcd.setCursor(13, 0);
	lcd.print(MainDS.Data[1], 1);
	lcd.print(F("%RH"));

	//line 2
	lcd.setCursor(0, 1);
	lcd.print(weather[forecast]);
	lcd.setCursor(strlen(weather[forecast]), 1);
	lcd.print(F("      "));
	lcd.setCursor(13, 1);
	lcd.print(MainDS.Data[4], 0);
	lcd.setCursor(17, 1);
	lcd.print(F("hPa"));

	//line 3
	lcd.setCursor(0, 2);
	lcd.print(SystemDS.Data[0], 1);
	lcd.print(F("C"));
	lcd.setCursor(20 - sMainUptime.length(), 2);
	lcd.print(sMainUptime);

	//line 4
	lcd.setCursor(0, 3);
	lcd.print(sNow);*/