//************************************
// Method:   	 prepareDataSetArrays
// Description:  Fills dataset with sensor readings.
// Access:   	 public 
// Returns:  	 void
// Qualifier:	
//************************************
void prepareDataSetArrays() {


	fRemoteUnitDataSet[0] = getAirTemperature();				//remoteTemperature
	fRemoteUnitDataSet[1] = getAirHumidity();					//remoteHumidity
	fRemoteUnitDataSet[2] = getAirHumidex();					//remoteHumidex

	powerSensors(true);
	fRemoteUnitDataSet[7] = readVcc();
	fRemoteUnitDataSet[3] = getSoilTemperature();				//remoteSoilTemperature

	fRemoteUnitDataSet[4] = getSoilHumidity();					//remoteSoilHumidity
	fRemoteUnitDataSet[5] = getLight();							//remoteLight
	powerSensors(false);
	noInterrupts();
	fRemoteUnitDataSet[6] = fRainTips;							//rain tips
	interrupts();
	fRemoteUnitDataSet[8] = getUptime();						//uptime
	fRemoteUnitDataSet[9] = freeRam();							//freeRam


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
	Serial.print(fRemoteUnitDataSet[0], 1);
	Serial.println(F("C"));

	Serial.print(F("Air hum: "));
	Serial.print(fRemoteUnitDataSet[1], 0);
	Serial.println(F("%RH"));

	Serial.print(F("Humidex: "));
	Serial.print(fRemoteUnitDataSet[2], 1);
	Serial.println(F("C"));

	Serial.print(F("Soil temp: "));
	Serial.print(fRemoteUnitDataSet[3], 1);
	Serial.println(F("C"));

	Serial.print(F("Soil hum: "));
	Serial.print(fRemoteUnitDataSet[4], 0);
	Serial.println(F("%"));

	Serial.print(F("Light: "));
	Serial.print(fRemoteUnitDataSet[5], 0);
	Serial.println(F("%"));

	Serial.print(F("RainTicks: "));
	Serial.println(fRemoteUnitDataSet[6], 0);

	Serial.print(F("Vcc: "));
	Serial.print(fRemoteUnitDataSet[7], 0);
	Serial.println(F("mV"));

	Serial.print(F("Uptime: "));
	Serial.print(fRemoteUnitDataSet[8], 0);
	Serial.println("s");

	Serial.print(F("Free ram: "));
	Serial.print(freeRam());
	Serial.println("b");

	Serial.println();
}


//************************************
// Method:   	 powerSensors
// Description:  Powers sensors if true. Disabùles sensors if false. (excluding DHT22 and DS18B20, these need to be handled separately)
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
	float _fTemp = dht.readTemperature();
	if (isnan(_fTemp)) {
#if debug
		Serial.print(F("Failed to read from DHT sensor! "));
#endif
		return -255;
	}
	else {
		AirTemp.addValue(_fTemp);
		return AirTemp.getAverage();
	}

}

float getAirHumidity(){
	float _fHum = dht.readHumidity();
	if (isnan(_fHum) || (fRemoteUnitDataSet[0] == -255)) {
#if debug
		Serial.print(F("Failed to read from DHT sensor! "));
#endif
		return -255;
	}
	else {
		AirHum.addValue(_fHum);
		return AirHum.getAverage();
	};
}

float getAirHumidex() {
	if ((fRemoteUnitDataSet[0] == -255) || (fRemoteUnitDataSet[1] == -255))
	{
		return -255;
	}
	else
		return (dht.computeHeatIndex(AirTemp.getAverage()*1.8 + 32, AirHum.getAverage()) - 32)*0.556;
}

byte getLight() {
	float _fLight = analogRead(PHOTORESISTOR_DATA_PIN)*(*Vcc) / 1023;
	byte _byLight = ((_fLight / *Vcc) * 100);
	Light.addValue(_byLight);
	return Light.getAverage();
}

float getSoilTemperature() {
	ds.requestTemperatures();
	float _fTemp = ds.getTempCByIndex(0);
	if (_fTemp == 85) return -255;
	SoilTemp.addValue(_fTemp);
	return SoilTemp.getAverage();
}

//returns soil humidity percentage 0 = air, 100 = salt water
byte getSoilHumidity() {
	float _fHum = analogRead(HUMIDITY_DATA_PIN)*(*Vcc) / 1023;
	byte hum = ((_fHum / *Vcc - 1)*-115);
	SoilHum.addValue(hum);
	return SoilHum.getAverage();
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
