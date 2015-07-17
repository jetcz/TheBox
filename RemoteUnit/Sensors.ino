//************************************
// Method:   	 prepareDataSetArrays
// Description:  Fills dataset with sensor readings.
// Access:   	 public 
// Returns:  	 void
// Qualifier:	
//************************************
void getDataSet() {
	digitalWrite(DHT22_PWR_PIN, HIGH);
	Sleepy::loseSomeTime(1500);
	dhtStatus = dht1.read22(DHT22_DATA_PIN);
	digitalWrite(DHT22_PWR_PIN, LOW);

	DS[0] = getAirTemperature();				//remoteTemperature
	DS[1] = getAirHumidity();					//remoteHumidity
	DS[2] = getAirHumidex();					//remoteHumidex
	DS[3] = getSoilTemperature();				//remoteSoilTemperature

	powerSensors(true);
	DS[7] = getVcc();
	DS[4] = getSoilHumidity();					//remoteSoilHumidity
	DS[5] = getLight();							//remoteLight
	powerSensors(false);

	noInterrupts();
	DS[6] = fRainTips;							//rain tips
	interrupts();

	DS[8] = getUptime();						//uptime
	DS[9] = freeRam();							//freeRam
}

//************************************
// Method:   	 printSensorData
// Description:  Print sensor data to serial port.
// Access:   	 public 
// Returns:  	 void
// Qualifier:	
//************************************
void printSensorData() {
	Serial.print(F("Air temp: "));
	Serial.print(DS[0], 1);
	Serial.println(F("C"));

	Serial.print(F("Air hum: "));
	Serial.print(DS[1], 0);
	Serial.println(F("%RH"));

	Serial.print(F("Humidex: "));
	Serial.print(DS[2], 1);
	Serial.println(F("C"));

	Serial.print(F("Soil temp: "));
	Serial.print(DS[3], 1);
	Serial.println(F("C"));

	Serial.print(F("Soil hum: "));
	Serial.print(DS[4], 0);
	Serial.println(F("%"));

	Serial.print(F("Light: "));
	Serial.print(DS[5], 0);
	Serial.println(F("%"));

	Serial.print(F("RainTicks: "));
	Serial.println(DS[6], 0);

	Serial.print(F("Vcc: "));
	Serial.print(DS[7], 0);
	Serial.println(F("mV"));

	Serial.print(F("Uptime: "));
	Serial.print(DS[8], 0);
	Serial.println("s");

	Serial.print(F("Free ram: "));
	Serial.print(freeRam());
	Serial.println("b");

	Serial.println();
}


//************************************
// Method:   	 powerSensors
// Description:  Powers sensors if true. Disables sensors if false. (excluding DHT22 and DS18B20, these need to be handled separately)
// Access:   	 public 
// Returns:  	 void
// Qualifier:	
// Parameter:	 bool state
//************************************
void powerSensors(bool state) {
	if (state)
	{
		digitalWrite(PHOTORESISTOR_PWR_PIN, HIGH);
		digitalWrite(HUMIDITY_PWR_PIN, HIGH);
	}
	else
	{
		digitalWrite(PHOTORESISTOR_PWR_PIN, LOW);
		digitalWrite(HUMIDITY_PWR_PIN, LOW);
	}
}

float getAirTemperature() {
	static RunningAverage AirTemp(3);
	if (dhtStatus == 0)
	{
		AirTemp.addValue(dht1.temperature);
		return AirTemp.getAverage();
	}
	else return -255;
}


float getAirHumidity(){
	static RunningAverage AirHum(3);
	if (dhtStatus == 0)
	{
		AirHum.addValue(dht1.humidity);
		return AirHum.getAverage();
	}
	else return -255;
}

float getAirHumidex() {
	if ((DS[0] == -255) || (DS[1] == -255)) return -255;
	else {
		float e;
		e = (6.112 * pow(10, (7.5 * DS[0] / (237.7 + DS[0]))) *  DS[1] / 100); //vapor pressure
		float humidex = DS[0] + 0.55555555 * (e - 10.0); //humidex
		return humidex;
	}
}

byte getLight() {
	static RunningAverage Light(3);
	float _fLight = (analogRead(PHOTORESISTOR_DATA_PIN) + 0.5)*(*Vcc) / 1024.0;
	byte _byLight = ((_fLight / *Vcc) * 100);
	Light.addValue(_byLight);
	return Light.getAverage();
}

float getSoilTemperature() {
	static RunningAverage SoilTemp(3);
	ds.requestTemperatures();
	float _fTemp = ds.getTempCByIndex(0);
	if (_fTemp == 85) return -255;
	SoilTemp.addValue(_fTemp);
	return SoilTemp.getAverage();
}

//returns soil humidity percentage 0 = air, 100 = salt water
byte getSoilHumidity() {
	static RunningAverage SoilHum(3);
	RunningAverage AnalogReadings(3);
	for (int i = 0; i < 3; i++)
	{
		AnalogReadings.addValue(analogRead(HUMIDITY_DATA_PIN));
	}
	float _fHum = (AnalogReadings.getAverage() + 0.5)*(*Vcc) / 1024.0;
	byte hum = ((_fHum / *Vcc - 1)*-103);
	SoilHum.addValue(hum);
	return SoilHum.getAverage();
}

float getVcc(){
	static RunningAverage _raVcc(3);
	_raVcc.addValue(float(readVcc()));
	return _raVcc.getAverage();
}

//************************************
// Method:   	 ISRTipCnt
// Description:  Interrupt method to gather tipping bucket tip count.
// Access:   	 public 
// Returns:  	 void
// Qualifier:	
//************************************
void ISRTipCnt() {
	fRainTips++;
}
