void printSensorData() {
	Serial.print("DHT22 temp: ");
	Serial.print(getAirTemperature());
	Serial.println("C");
	Serial.print("DHT22 hum: ");
	Serial.print(getAirHumidity());
	Serial.println("%RH");
	Serial.print("Photoresistor: ");
	Serial.print(getLight());
	Serial.println("(mV)");
	Serial.print("DS temp: ");
	Serial.print(getSoilTemperature());
	Serial.println("C");
	Serial.print("Soil hum: ");
	Serial.print(getSoilHumidity());
	Serial.println("(mV)");
	Serial.print("Vcc: ");
	Serial.print(readVcc());
	Serial.println("mV");
	Serial.println();
}
