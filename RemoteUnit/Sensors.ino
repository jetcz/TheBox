/// <summary>
/// Fills dataset with sensor readings.
/// </summary>
void getPayload() {

	//get DHT data
	powerSensors(true, true);
	nDHTStatus = DHT.read22(DHT22_DATA_PIN);
	powerSensors(false, true);
	if (nDHTStatus == 0) {
		payload.AirTemp = getAirTemperature() * 10;				//remoteTemperature
		payload.AirHum = getAirHumidity() * 10;					//remoteHumidity
		payload.AirHumidex = getAirHumidex() * 10;				//remoteHumidex
	}
	else payload.AirTemp = payload.AirHum = payload.AirHumidex = InvalidValue;	//remoteTemperature

	//get DS data
	float _fVal = getSoilTemperature();
	payload.SoilTemp = (_fVal == (85 || -127)) ? InvalidValue : _fVal * 10;	//remoteSoilTemperature

	//get simple analog sensors data
	powerSensors(true, false);
	payload.Vcc = getVcc();
	payload.SoilHum = getSoilHumidity() * 10;						//remoteSoilHumidity
	payload.Light = getLight() * 10;								//remoteLight
	powerSensors(false, false);

	//get rain
	noInterrupts();
	payload.RainTips = nRainTips;									//rain tips
	interrupts();

	//get sys info
	payload.Uptime = getUptime();									//uptime
	payload.FreeRam = getFreeRam();									//freeRam

#if DEBUG
	payload.print();
#endif	
}

/// <summary>
/// Power up/down the sensors.
/// DHT22 notes:
/// For some reason we have to put the data pin low aswell otherwise the idle amperage is about 1.8mA.
/// If we put the data pin low, total amperage is about 12.7uA.
/// This method also handles the necessary delay after powering up the DHT22, which is empirically about 1500ms, otherwise readings are unsuccessful.
/// </summary>
/// <param name="pwr">on/off</param>
/// <param name="dht">power DHT22 separetely</param>
void powerSensors(bool pwr, bool dht) {
	if (dht)
	{
		if (pwr)
		{
			pinMode(DHT22_DATA_PIN, INPUT);
			digitalWrite(DHT22_PWR_PIN, HIGH);
			Sleepy::loseSomeTime(1500);
		}
		else {
			pinMode(DHT22_DATA_PIN, OUTPUT);	//for some reason we have to put the data pin low or the idle power consumption is about 1.8mA
			digitalWrite(DHT22_DATA_PIN, LOW);
			digitalWrite(DHT22_PWR_PIN, LOW);
		}
	}
	else {
		if (pwr)
		{
			digitalWrite(PHOTORESISTOR_PWR_PIN, HIGH);
			digitalWrite(HUMIDITY_PWR_PIN, HIGH);
		}
		else {
			digitalWrite(PHOTORESISTOR_PWR_PIN, LOW);
			digitalWrite(HUMIDITY_PWR_PIN, LOW);
		}
	}
}

/// <summary>
/// Get running average of remote air temperature
/// </summary>
/// <returns></returns>
float getAirTemperature() {
	static RunningAverage AirTemp(3);
	AirTemp.addValue(DHT.temperature);
	return AirTemp.getAverage();
}

/// <summary>
/// Get running average of remote air humidity
/// </summary>
/// <returns></returns>
float getAirHumidity() {
	static RunningAverage AirHum(3);
	AirHum.addValue(DHT.humidity);
	return AirHum.getAverage();
}

/// <summary>
/// Get running average of remote humidex
/// </summary>
/// <returns></returns>
float getAirHumidex() {
	float e;
	e = (6.112 * pow(10, (7.5 * (payload.AirTemp / 10.0) / (237.7 + (payload.AirTemp / 10.0)))) *  (payload.AirHum / 10.0) / 100.0); //vapor pressure
	float humidex = (payload.AirTemp / 10.0) + 0.55555555 * (e - 10.0); //humidex
	return humidex;
}

/// <summary>
/// Get running average of light intensity
/// </summary>
/// <returns></returns>
float getLight() {
	static RunningAverage Light(3);
	float _fLight = (analogRead(PHOTORESISTOR_DATA_PIN) + 0.5)*(*Vcc) / 1024.0;
	_fLight = ((_fLight / *Vcc) * 100);
	Light.addValue(_fLight);
	return Light.getAverage();
}

/// <summary>
/// Get running average of remote soil temperature
/// </summary>
/// <returns></returns>
float getSoilTemperature() {
	static RunningAverage SoilTemp(3);
	ds.requestTemperatures();
	float _fTemp = ds.getTempCByIndex(0);
	if (_fTemp == 85) return _fTemp;
	else SoilTemp.addValue(_fTemp);
	return SoilTemp.getAverage();
}

/// <summary>
/// Get running average of remote soil humidity
/// perc: air = 0%, salt water = 100%
/// tick cnt: air = 15, salt water = 746
/// </summary>
/// <returns></returns>
float getSoilHumidity() {
	static RunningAverage SoilHum(3);
	RunningAverage AnalogReadings(3);
	for (int i = 0; i < 3; i++)
	{
		AnalogReadings.addValue(analogRead(HUMIDITY_DATA_PIN));
	}
	float _fHum = (AnalogReadings.getAverage() + 0.5) / 1024.0;
	_fHum = ((_fHum - 1)*-100);
	_fHum -= 27.3;	//calibration
	_fHum *= 1.405;	//calibration
	_fHum = _fHum > 100 ? 100 : _fHum;
	_fHum = _fHum < 0 ? 0 : _fHum;
	SoilHum.addValue(_fHum);
	return SoilHum.getAverage();
}

/// <summary>
/// Get running average of reference voltage (5V)
/// </summary>
/// <returns></returns>
float getVcc() {
	static RunningAverage _raVcc(3);
	_raVcc.addValue(float(readVcc()));
	return _raVcc.getAverage();
}

/// <summary>
/// Interrupt method to gather tipping bucket tip count.
/// </summary>
void ISRTipCnt() {
	nRainTips++;
}
