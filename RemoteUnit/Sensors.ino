/// <summary>
/// Fills dataset with sensor readings.
/// </summary>
void getPayload() {

	//get DHT data
	digitalWrite(DHT22_PWR_PIN, HIGH);
	Sleepy::loseSomeTime(1500);
	nDHTStatus = DHT.read22(DHT22_DATA_PIN);
	digitalWrite(DHT22_PWR_PIN, LOW);
	if (nDHTStatus == 0){
		payload.AirTemp = getAirTemperature() * 10;				//remoteTemperature
		payload.AirHum = getAirHumidity() * 10;					//remoteHumidity
		payload.AirHumidex = getAirHumidex() * 10;				//remoteHumidex
	}
	else payload.AirTemp = payload.AirHum = payload.AirHumidex = InvalidValue * 10;	//remoteTemperature

	//get DS data
	float _fVal = getSoilTemperature();
	payload.SoilTemp = (_fVal == (85 || -127)) ? InvalidValue * 10 : _fVal * 10;	//remoteSoilTemperature

	//get simple analog sensors data
	digitalWrite(PHOTORESISTOR_PWR_PIN, HIGH);
	digitalWrite(HUMIDITY_PWR_PIN, HIGH);
	payload.Vcc = getVcc();
	payload.SoilHum = getSoilHumidity() * 10;						//remoteSoilHumidity
	payload.Light = getLight() * 10;								//remoteLight
	digitalWrite(PHOTORESISTOR_PWR_PIN, LOW);
	digitalWrite(HUMIDITY_PWR_PIN, LOW);

	//get rain
	noInterrupts();
	payload.RainTips = nRainTips;									//rain tips
	interrupts();

	//get sys info
	payload.Uptime = getUptime();									//uptime
	payload.FreeRam = freeRam();									//freeRam

#if DEBUG
	payload.print();
#endif

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
float getAirHumidity(){
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
/// 0 = air, 100 = salt water
/// </summary>
/// <returns></returns>
float getSoilHumidity() {
	static RunningAverage SoilHum(3);
	RunningAverage AnalogReadings(3);
	for (int i = 0; i < 3; i++)
	{
		AnalogReadings.addValue(analogRead(HUMIDITY_DATA_PIN));
	}
	float _fHum = (AnalogReadings.getAverage() + 0.5)*(*Vcc) / 1024.0;
	_fHum = ((_fHum / *Vcc - 1)*-103);
	SoilHum.addValue(_fHum);
	return SoilHum.getAverage();
}

/// <summary>
/// Get running average of reference voltage (5V)
/// </summary>
/// <returns></returns>
float getVcc(){
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
