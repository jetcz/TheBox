/// <summary>
/// Prints sensor values to lcd
/// </summary>
void printLcdScreen1() {
	//this is to know lengths of printed strings so we dont have to refresh whole screen (causes flicker)
	static byte _byLastLenMainData0;
	static byte _byLastLenMainData1;
	static byte _byLastLenRemoteData0;
	static byte _byLastLenRemoteData1;
	byte _byCurrLenMainData0 = floatToString(*MainDS.Temperature).length();
	byte _byCurrLenMainData1 = floatToString(*MainDS.Humidity).length();
	byte _byCurrLenRemoteData0 = floatToString(*RemoteDS.Temperature).length();
	byte _byCurrLenRemoteData1 = floatToString(*RemoteDS.Humidity).length();

	//line 1
	if (_byLastLenMainData0 != _byCurrLenMainData0)
	{
		lcd.setCursor(0, 0);
		lcd.print(F("          "));
	}
	lcd.setCursor(0, 0);
	lcd.print(*MainDS.Temperature, 1);
	lcd.print(F("C"));

	if (_byLastLenRemoteData0 != _byCurrLenRemoteData0)
	{
		lcd.setCursor(10, 0);
		lcd.print(F("          "));
	}
	lcd.setCursor(19 - _byCurrLenRemoteData0, 0);
	lcd.print(*RemoteDS.Temperature, 1);
	lcd.print(F("C"));
	_byLastLenMainData0 = _byCurrLenMainData0;
	_byLastLenRemoteData0 = _byCurrLenRemoteData0;

	//line 2
	if (_byLastLenMainData1 != _byCurrLenMainData1)
	{
		lcd.setCursor(0, 1);
		lcd.print(F("          "));
	}
	lcd.setCursor(0, 1);
	lcd.print(*MainDS.Humidity, 1);
	lcd.print(F("%RH"));

	if (_byLastLenRemoteData1 != _byCurrLenRemoteData1)
	{
		lcd.setCursor(10, 1);
		lcd.print(F("          "));
	}
	lcd.setCursor(17 - _byCurrLenRemoteData1, 1);
	lcd.print(*RemoteDS.Humidity, 1);
	lcd.print(F("%RH"));
	_byLastLenMainData1 = _byCurrLenMainData1;
	_byLastLenRemoteData1 = _byCurrLenRemoteData1;

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
void printLcdScreen2() {

	static byte _byLastLenRemoteData2;
	static byte _byLastLenRemoteData3;
	static byte _byLastLenRemoteData4;
	static byte _byLastLenRemoteData5;
	static byte _byLastLenRemoteData6;
	static byte _byLastLenRemoteData7;
	static byte _byLastLenMainData4;
	static byte _byLastLenMainData5;
	static byte _byLastLenMainData6;

	byte _byCurrLenRemoteData2 = floatToString(RemoteDS.Data[2]).length();
	byte _byCurrLenRemoteData3 = floatToString(RemoteDS.Data[3]).length();
	byte _byCurrLenRemoteData4 = floatToString(RemoteDS.Data[4]).length();
	byte _byCurrLenRemoteData5 = floatToString(RemoteDS.Data[5]).length();
	byte _byCurrLenRemoteData6 = floatToString(RemoteDS.Data[6]).length();
	byte _byCurrLenRemoteData7 = floatToString(RemoteDS.Data[7]).length();
	byte _byCurrLenMainData4 = floatToString(MainDS.Data[4]).length();
	byte _byCurrLenMainData5 = floatToString(MainDS.Data[5]).length();
	byte _byCurrLenMainData6 = floatToString(MainDS.Data[6]).length();

	//line 1 humidex	soiltemp
	if (_byCurrLenRemoteData2 != _byLastLenRemoteData2)
	{
		lcd.setCursor(0, 0);
		lcd.print(F("          "));
	}
	lcd.setCursor(0, 0);
	lcd.print(RemoteDS.Data[2], 1);
	lcd.print(F("C/H")); //humidex
	if (_byCurrLenRemoteData3 != _byLastLenRemoteData3)
	{
		lcd.setCursor(10, 0);
		lcd.print(F("          "));
	}
	lcd.setCursor(19 - _byCurrLenRemoteData3, 0);
	lcd.print(RemoteDS.Data[3], 1);
	lcd.print(F("C")); //soil temp

	//line 2
	if (_byCurrLenRemoteData5 != _byLastLenRemoteData5)
	{
		lcd.setCursor(0, 1);
		lcd.print(F("          "));
	}
	lcd.setCursor(0, 1);
	lcd.print(RemoteDS.Data[5], 1); //light
	lcd.print(F("%L"));
	if (_byCurrLenRemoteData4 != _byLastLenRemoteData4)
	{
		lcd.setCursor(10, 1);
		lcd.print(F("          "));
	}
	lcd.setCursor(17 - _byCurrLenRemoteData4, 1);
	lcd.print(RemoteDS.Data[4], 1);
	lcd.print(F("%RH")); //soil hum

	//line 3 
	if (_byCurrLenMainData4 != _byLastLenMainData4)
	{
		lcd.setCursor(0, 2);
		lcd.print(F("          "));
	}
	lcd.setCursor(0, 2);
	lcd.print(MainDS.Data[4], 1); //pressure
	lcd.print(F("hPa"));
	if (_byCurrLenRemoteData6 != _byLastLenRemoteData6)
	{
		lcd.setCursor(10, 2);
		lcd.print(F("          "));
	}
	lcd.setCursor(16 - _byCurrLenRemoteData6, 2);
	lcd.print(RemoteDS.Data[6], 1); //rain/hour
	lcd.print(F("mm/h"));

	//line 4
	if (_byCurrLenMainData5 != _byLastLenMainData5)
	{
		lcd.setCursor(0, 3);
		lcd.print(F("     "));
	}
	lcd.setCursor(0, 3);
	lcd.print(MainDS.Data[5], (MainDS.Data[5] >= 100) ? 0 : 1); //power left socket
	lcd.print(F("W"));

	if (_byCurrLenMainData6 != _byLastLenMainData6)
	{
		lcd.setCursor(6, 3);
		lcd.print(F("     "));
	}
	lcd.setCursor(6, 3);
	lcd.print(MainDS.Data[6], (MainDS.Data[6] >= 100) ? 0 : 1); //power right socket
	lcd.print(F("W"));

	if (_byCurrLenRemoteData6 != _byLastLenRemoteData6)
	{
		lcd.setCursor(10, 3);
		lcd.print(F("          "));
	}
	lcd.setCursor(16 - _byCurrLenRemoteData7, 3);
	lcd.print(RemoteDS.Data[7], 1); //rain/day
	lcd.print(F("mm/d"));

	_byLastLenMainData4 = _byCurrLenMainData4;
	_byLastLenMainData5 = _byCurrLenMainData5;
	_byLastLenMainData6 = _byCurrLenMainData6;
	_byLastLenRemoteData2 = _byCurrLenRemoteData2;
	_byLastLenRemoteData3 = _byCurrLenRemoteData3;
	_byLastLenRemoteData4 = _byCurrLenRemoteData4;
	_byLastLenRemoteData5 = _byCurrLenRemoteData5;
	_byLastLenRemoteData6 = _byCurrLenRemoteData6;
	_byLastLenRemoteData7 = _byCurrLenRemoteData7;

}

/// <summary>
/// Prints system values to lcd
/// </summary>
void printLcdScreen3() {
	static byte _byLastFailedCntRadioTotal;
	byte _byCurrFailedCntRadioTotal = intToString(nFailedCntRadioTotal).length();

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
	if (_byLastFailedCntRadioTotal != _byCurrFailedCntRadioTotal)
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

	_byLastFailedCntRadioTotal = _byCurrFailedCntRadioTotal;
}
