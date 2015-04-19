
void printLcdScreen1() {
	static byte byLastLenMainData0;
	static byte byLastLenRemoteData0;
	byte byCurrLenMainData0 = floatToString(MainDS.Data[0]).length();
	byte byCurrLenRemoteData0 = floatToString(RemoteDS.Data[0]).length();

	//line 1
	if (byLastLenMainData0 != byCurrLenMainData0)
	{
		lcd.setCursor(0, 0);
		lcd.print(F("          "));
	}
	lcd.setCursor(0, 0);
	lcd.print(MainDS.Data[0], 1);
	lcd.print(F("C"));
	if (byLastLenRemoteData0 != byCurrLenRemoteData0)
	{
		lcd.setCursor(10, 0);
		lcd.print(F("          "));
	}
	lcd.setCursor(19 - byCurrLenRemoteData0, 0);
	lcd.print(RemoteDS.Data[0], 1);
	lcd.print(F("C"));
	byLastLenMainData0 = byCurrLenMainData0;
	byLastLenRemoteData0 = byCurrLenRemoteData0;

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

	static byte byLastLenRemoteData2;
	static byte byLastLenRemoteData3;
	static byte byLastLenRemoteData4;
	static byte byLastLenRemoteData5;
	static byte byLastLenRemoteData6;
	static byte byLastLenMainData4;

	byte byCurrLenRemoteData2 = floatToString(RemoteDS.Data[2]).length();
	byte byCurrLenRemoteData3 = floatToString(RemoteDS.Data[3]).length();
	byte byCurrLenRemoteData4 = floatToString(RemoteDS.Data[4]).length();
	byte byCurrLenRemoteData5 = floatToString(RemoteDS.Data[5]).length();
	byte byCurrLenRemoteData6 = floatToString(RemoteDS.Data[6]).length();
	byte byCurrLenMainData4 = floatToString(MainDS.Data[4]).length();

	//line 1 humidex	soiltemp
	if (byCurrLenRemoteData2 != byLastLenRemoteData2)
	{
		lcd.setCursor(0, 0);
		lcd.print(F("          "));
	}
	lcd.setCursor(0, 0);
	lcd.print(RemoteDS.Data[2], 1);
	lcd.print(F("C/H")); //humidex
	if (byCurrLenRemoteData3 != byLastLenRemoteData3)
	{
		lcd.setCursor(10, 0);
		lcd.print(F("          "));
	}
	lcd.setCursor(19 - byCurrLenRemoteData3, 0);
	lcd.print(RemoteDS.Data[3], 1);
	lcd.print(F("C")); //soil temp

	//line 2
	if (byCurrLenRemoteData5 != byLastLenRemoteData5)
	{
		lcd.setCursor(0, 1);
		lcd.print(F("          "));
	}
	lcd.setCursor(0, 1);
	lcd.print(RemoteDS.Data[5], 1); //light
	lcd.print(F("%L"));
	if (byCurrLenRemoteData4 != byLastLenRemoteData4)
	{
		lcd.setCursor(10, 1);
		lcd.print(F("          "));
	}
	lcd.setCursor(17 - byCurrLenRemoteData4, 1);
	lcd.print(RemoteDS.Data[4], 1);
	lcd.print(F("%RH")); //soil hum

	//line 3 
	if (byCurrLenMainData4 != byLastLenMainData4)
	{
		lcd.setCursor(0, 2);
		lcd.print(F("          "));
	}
	lcd.setCursor(0, 2);
	lcd.print(MainDS.Data[4], 1); //pressure
	lcd.print(F("hPa"));
	if (byCurrLenRemoteData6 != byLastLenRemoteData6)
	{
		lcd.setCursor(10, 2);
		lcd.print(F("          "));
	}
	lcd.setCursor(16 - byCurrLenRemoteData6, 2);
	lcd.print(RemoteDS.Data[6], 1); //rain
	lcd.print(F("mm/h"));

	byLastLenMainData4 = byCurrLenMainData4;
	byLastLenRemoteData2 = byCurrLenRemoteData2;
	byLastLenRemoteData3 = byCurrLenRemoteData3;
	byLastLenRemoteData4 = byCurrLenRemoteData4;
	byLastLenRemoteData5 = byCurrLenRemoteData5;
	byLastLenRemoteData6 = byCurrLenRemoteData6;

}

void printLcdScreen3() {
	//line 1
	lcd.setCursor(0, 0);
	lcd.print(SystemDS.Data[0], 1);
	lcd.print(F("C"));
	lcd.setCursor(20 - sMainUptime.length(), 0);
	lcd.print(sMainUptime);

	//line 2
	lcd.setCursor(0, 1);
	lcd.print(SystemDS.Data[6], 0);
	lcd.print(F("mV"));
	lcd.setCursor(20 - sRemoteUptime.length(), 1);
	lcd.print(sRemoteUptime);

	//3
	String s;
	if (bReceivedRadioMsg) s = intToString(now() - RemoteDS.Timestamp.unixtime());
	else s = "NaN";
	lcd.setCursor(0, 2);
	lcd.print(F("RemoteDS age"));
	lcd.setCursor(13, 2);
	lcd.print(F("       "));
	lcd.setCursor(20 - s.length(), 2);
	lcd.print(s);

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
