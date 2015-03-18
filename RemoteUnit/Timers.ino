void printSensorData() {
	Serial.print(F("DHT22 temp: "));
	Serial.print(getAirTemperature());
	Serial.println(F("C"));
	Serial.print(F("DHT22 hum: "));
	Serial.print(getAirHumidity());
	Serial.println(F("%RH"));
	Serial.print(F("Photoresistor: "));
	Serial.print(getLight());
	Serial.println(F("(mV)"));
	Serial.print(F("DS temp: "));
	Serial.print(getSoilTemperature());
	Serial.println(F("C"));
	Serial.print(F("Soil hum: "));
	Serial.print(getSoilHumidity());
	Serial.println(F("(mV)"));
	Serial.print(F("Vcc: "));
	Serial.print(readVcc());
	Serial.println(F("mV"));
	Serial.print(F("Free ram: "));
	Serial.print(freeRam());
	Serial.println("b");
	Serial.println();
}

void getSensorData() {



}
