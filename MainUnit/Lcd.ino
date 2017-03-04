/// <summary>
/// Prints sensor values to lcd
/// </summary>
void printLcdScreen1()
{
	//this is to know lengths of printed strings so we dont have to refresh whole screen (causes flicker)
	static byte byLastLenMainData0;
	static byte byLastLenMainData1;
	static byte byLastLenRemoteData0;
	static byte byLastLenRemoteData1;
	byte byCurrLenMainData0 = floatToString(*MainDS.Temperature).length();
	byte byCurrLenMainData1 = floatToString(*MainDS.Humidity).length();
	byte byCurrLenRemoteData0 = floatToString(*RemoteDS.Temperature).length();
	byte byCurrLenRemoteData1 = floatToString(*RemoteDS.Humidity).length();

	//line 1
	if (byLastLenMainData0 != byCurrLenMainData0)
	{
		lcd.setCursor(0, 0);
		lcd.print(F("          "));
	}
	lcd.setCursor(0, 0);
	lcd.print(*MainDS.Temperature, 1);
	lcd.print(F("C"));

	if (byLastLenRemoteData0 != byCurrLenRemoteData0)
	{
		lcd.setCursor(10, 0);
		lcd.print(F("          "));
	}
	lcd.setCursor(19 - byCurrLenRemoteData0, 0);
	lcd.print(*RemoteDS.Temperature, 1);
	lcd.print(F("C"));
	byLastLenMainData0 = byCurrLenMainData0;
	byLastLenRemoteData0 = byCurrLenRemoteData0;

	//line 2
	if (byLastLenMainData1 != byCurrLenMainData1)
	{
		lcd.setCursor(0, 1);
		lcd.print(F("          "));
	}
	lcd.setCursor(0, 1);
	lcd.print(*MainDS.Humidity, 1);
	lcd.print(F("%RH"));

	if (byLastLenRemoteData1 != byCurrLenRemoteData1)
	{
		lcd.setCursor(10, 1);
		lcd.print(F("          "));
	}
	lcd.setCursor(17 - byCurrLenRemoteData1, 1);
	lcd.print(*RemoteDS.Humidity, 1);
	lcd.print(F("%RH"));
	byLastLenMainData1 = byCurrLenMainData1;
	byLastLenRemoteData1 = byCurrLenRemoteData1;

	//line 3
	lcd.setCursor(0, 2);
	lcd.print(F("Weather:"));
	lcd.setCursor(20 - (strlen(cWeather[byForecast])), 2);
	lcd.print(cWeather[byForecast]);
	//line 4
	lcd.setCursor(0, 3);
	lcd.print(sNow);
}

/// <summary>
/// Prints sensor values to lcd
/// </summary>
void printLcdScreen2()
{
	static byte byLastLenRemoteData2;
	static byte byLastLenRemoteData3;
	static byte byLastLenRemoteData4;
	static byte byLastLenRemoteData5;
	static byte byLastLenRemoteData6;
	static byte byLastLenRemoteData7;
	static byte byLastLenMainData4;
	static byte byLastLenMainData5;
	static byte byLastLenMainData6;

	byte byCurrLenRemoteData2 = floatToString(RemoteDS.Data[2]).length();
	byte byCurrLenRemoteData3 = floatToString(RemoteDS.Data[3]).length();
	byte byCurrLenRemoteData4 = floatToString(RemoteDS.Data[4]).length();
	byte byCurrLenRemoteData5 = floatToString(RemoteDS.Data[5]).length();
	byte byCurrLenRemoteData6 = floatToString(RemoteDS.Data[6]).length();
	byte byCurrLenRemoteData7 = floatToString(RemoteDS.Data[7]).length();
	byte byCurrLenMainData4 = floatToString(MainDS.Data[4]).length();
	byte byCurrLenMainData5 = floatToString(MainDS.Data[5]).length();
	byte byCurrLenMainData6 = floatToString(MainDS.Data[6]).length();

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
	lcd.print(RemoteDS.Data[6], 1); //rain/hour
	lcd.print(F("mm/h"));

	//line 4
	if (byCurrLenMainData5 != byLastLenMainData5)
	{
		lcd.setCursor(0, 3);
		lcd.print(F("     "));
	}
	lcd.setCursor(0, 3);
	lcd.print(MainDS.Data[5], (MainDS.Data[5] >= 100) ? 0 : 1); //power left socket
	lcd.print(F("W"));

	if (byCurrLenMainData6 != byLastLenMainData6)
	{
		lcd.setCursor(6, 3);
		lcd.print(F("     "));
	}
	lcd.setCursor(6, 3);
	lcd.print(MainDS.Data[6], (MainDS.Data[6] >= 100) ? 0 : 1); //power right socket
	lcd.print(F("W"));

	if (byCurrLenRemoteData6 != byLastLenRemoteData6)
	{
		lcd.setCursor(10, 3);
		lcd.print(F("          "));
	}
	lcd.setCursor(16 - byCurrLenRemoteData7, 3);
	lcd.print(RemoteDS.Data[7], 1); //rain/day
	lcd.print(F("mm/d"));

	byLastLenMainData4 = byCurrLenMainData4;
	byLastLenMainData5 = byCurrLenMainData5;
	byLastLenMainData6 = byCurrLenMainData6;
	byLastLenRemoteData2 = byCurrLenRemoteData2;
	byLastLenRemoteData3 = byCurrLenRemoteData3;
	byLastLenRemoteData4 = byCurrLenRemoteData4;
	byLastLenRemoteData5 = byCurrLenRemoteData5;
	byLastLenRemoteData6 = byCurrLenRemoteData6;
	byLastLenRemoteData7 = byCurrLenRemoteData7;

}

/// <summary>
/// Prints system values to lcd
/// </summary>
void printLcdScreen3()
{
	static byte byLastFailedCntRadioTotal;
	byte byCurrFailedCntRadioTotal = intToString(nFailedCntRadioTotal).length();

	//line 1
	lcd.setCursor(0, 0);
	lcd.print(*SystemDS.Temperature, 1);
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
	if (bReceivedRadioMsg) s = intToString(now() - RemoteDS.TimeStamp.unixtime());
	else s = "NaN";
	lcd.setCursor(0, 2);
	lcd.print(F("RDS age     "));
	lcd.setCursor(8, 2);
	lcd.print(s);
	if (byLastFailedCntRadioTotal != byCurrFailedCntRadioTotal)
	{
		lcd.setCursor(12, 2);
		lcd.print(F("        "));
	}
	lcd.setCursor(17 - intToString(nFailedCntRadioTotal).length(), 2);
	lcd.print(F("fR "));
	lcd.print(nFailedCntRadioTotal);

	//4
	lcd.setCursor(0, 3);
	lcd.print(F("FreeRAM "));
	lcd.print(floatToString(float(nMainFreeRam) / 8192 * 100));
	lcd.print(F("% "));
	lcd.setCursor(17 - intToString(nFailedNetowkOpsTotal).length(), 3);
	lcd.print(F("fT "));
	lcd.print(nFailedNetowkOpsTotal);

	byLastFailedCntRadioTotal = byCurrFailedCntRadioTotal;
}
