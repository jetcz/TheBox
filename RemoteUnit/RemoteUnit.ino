#include <DHT.h>
#include <OneWire.h>
#include <DallasTemperature.h>

/* pin mappings
a0	14
a1	15
a2	16
a3	17
a4	18
a5	19
*/
const int DHT22_DATA_PIN = 14;
const int DHT22_PWR_PIN = 5;
const int PHOTORESISTOR_DATA_PIN = 17;
const int PHOTORESISTOR_PWR_PIN = 3;
const int DS_DATA_PIN = 16;
const int DS_PWR_PIN = 2;
const int HUMIDITY_DATA_PIN = 15;
const int HUMIDITY_PWR_PIN = 4;
const int RADIO_DATA_PIN = 6;
const int RADIO_PWR_PIN = 7;
const int LED[3] = { 11, 12, 13 };

#define DHTTYPE DHT22
OneWire oneWire(DS_DATA_PIN);
DallasTemperature ds(&oneWire);
DHT dht(DHT22_DATA_PIN, DHTTYPE);

void setup() {
	Serial.begin(9600);
	setupPins();
	powerSensors();
	dht.begin();
	ds.begin();

}

void loop() {
	Serial.print(getAirTemperature());
	Serial.println(" C");
	Serial.print(getLight());
	Serial.println(" Light V");
	Serial.print(getSoilTemperature());
	Serial.println(" C");
	delay(3000);
}