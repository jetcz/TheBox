
void printLcdScreen1() {

	//line 1
	lcd.setCursor(0, 0);
	lcd.print(fMainUnitDataSet[0], 1);
	lcd.print(F("C"));
	lcd.setCursor(13, 0);
	lcd.print(fMainUnitDataSet[1], 1);
	lcd.print(F("%RH"));

	//line 2
	lcd.setCursor(0, 1);
	lcd.print(weather[forecast]);
	lcd.setCursor(strlen(weather[forecast]), 1);
	lcd.print(F("      "));
	lcd.setCursor(13, 1);
	lcd.print(fMainUnitDataSet[4], 0);
	lcd.setCursor(17, 1);
	lcd.print(F("hPa"));

	//line 3
	lcd.setCursor(0, 2);
	lcd.print(fSysDataSet[0], 1);
	lcd.print(F("C"));
	lcd.setCursor(20 - sUptime.length(), 2);
	lcd.print(sUptime);

	//line 4
	lcd.setCursor(0, 3);
	lcd.print(sNow);
}

void printLcdScreen2() {
	lcd.clear();
	lcd.setCursor(0, 0);
	lcd.print(F("screen 2"));
}

void printLcdScreen3() {
	lcd.clear();
	lcd.setCursor(0, 0);
	lcd.print(F("screen 3"));
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
