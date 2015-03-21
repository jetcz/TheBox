
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
	Serial.print(fRemoteUnitDataSet[4],0);
	Serial.println(F("%"));

	Serial.print(F("Light: "));
	Serial.print(fRemoteUnitDataSet[5],0);
	Serial.println(F("%"));

	Serial.print(F("RainTicks: "));
	Serial.println(fRemoteUnitDataSet[6],0);

	Serial.print(F("Vcc: "));
	Serial.print(fRemoteUnitDataSet[7],0);
	Serial.println(F("mV"));

	Serial.print(F("Uptime: "));
	Serial.print(fRemoteUnitDataSet[8], 0);
	Serial.println("s");

	Serial.print(F("Free ram: "));
	Serial.print(freeRam());
	Serial.println("b");

	Serial.println();
}

void prepareDataSetArrays() {

	fRemoteUnitDataSet[7] = readVcc();							//Vcc

	fRemoteUnitDataSet[0] = getAirTemperature();				//remoteTemperature
	fRemoteUnitDataSet[1] = getAirHumidity();					//remoteHumidity
	fRemoteUnitDataSet[2] = getAirHumidex();					//remoteHumidex
	fRemoteUnitDataSet[3] = getSoilTemperature();				//remoteSoilTemperature
	fRemoteUnitDataSet[4] = getSoilHumidity();					//remoteSoilHumidity
	fRemoteUnitDataSet[5] = getLight();							//remoteLight
	fRemoteUnitDataSet[6] = nRainTicks;							//rain

	fRemoteUnitDataSet[8] = round(millis() / 1000);				//uptime
}