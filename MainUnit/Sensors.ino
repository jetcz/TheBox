/// <summary>
/// Get running average of system temperature
/// </summary>
/// <param name="event"></param>
/// <returns></returns>
float getSysTemperature(sensors_event_t event)
{
	static RunningAverage raSysTemp(6);
	float fSysTemp;
	bmp.getEvent(&event);
	bmp.getTemperature(&fSysTemp);
	if (fSysTemp == 85) return Settings.InvalidValue;
	raSysTemp.addValue(fSysTemp);
	return raSysTemp.getAverage();
}

/// <summary>
/// Get running average of atmospheric pressure
/// </summary>
/// <param name="event"></param>
/// <returns></returns>
float getPressure(sensors_event_t event)
{
	if (*SystemDS.Temperature == Settings.InvalidValue) return Settings.InvalidValue;
	static RunningAverage raPressure(6);
	bmp.getEvent(&event);
	raPressure.addValue(event.pressure);
	return raPressure.getAverage();
}

/// <summary>
/// Get running average of main temperature
/// </summary>
/// <returns></returns>
float getMainTemperature()
{
	static RunningAverage raMainTemp(6);
	raMainTemp.addValue(DHT.temperature);
	return raMainTemp.getAverage();
}

/// <summary>
/// Get running average of main humidity
/// </summary>
/// <returns></returns>
float getMainHumidity()
{
	static RunningAverage raMainHumidity(6);
	raMainHumidity.addValue(DHT.humidity);
	return raMainHumidity.getAverage();
}

/// <summary>
/// Get running average of main humidex
/// </summary>
/// <returns></returns>
float getMainHumidex()
{
	float e;
	e = (6.112 * pow(10, (7.5 * *MainDS.Temperature / (237.7 + *MainDS.Temperature))) *  *MainDS.Humidity / 100.0); //vapor pressure
	float humidex = *MainDS.Temperature + 0.55555555 * (e - 10.0); //humidex
	return humidex;
}

/// <summary>
/// Get main PIR
/// </summary>
/// <returns></returns>
bool getMainPir()
{
	return digitalRead(PIR_PIN);
}

/// <summary>
/// Get running average of power consumptions in the socekts
/// </summary>
/// <param name="relay">relay</param>
/// <returns></returns>
float getPower(int relay)
{
	static RunningAverage raCurr0(5);
	static RunningAverage raCurr3(5);

	if (relay == 0)
	{
		if (!getRelayState(0) ||
			emon.realPower1 < 0 ||
			emon.powerFactor1 < 0
			)
		{
			raCurr0.clear();
		}
		else raCurr0.addValue(emon.realPower1);

		return raCurr0.getAverage();
	}

	if (relay == 3)
	{
		if (!getRelayState(3) ||
			emon.realPower2 < 0 ||
			emon.powerFactor2 < 0
			)
		{
			raCurr3.clear();
		}
		else raCurr3.addValue(emon.realPower2);

		return raCurr3.getAverage();
	}
	return 0;
}

/// <summary>
/// Get running average of mains voltage
/// </summary>
/// <returns></returns>
float getVoltage()
{
	static RunningAverage raVoltage(10);
	raVoltage.addValue((emon.Vrms));
	return raVoltage.getAverage();
}