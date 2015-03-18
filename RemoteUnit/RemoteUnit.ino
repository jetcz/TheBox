#include <SPI.h>
#include <RH_ASK.h>
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
const int DHT22_DATA_PIN = 8;
const int DHT22_PWR_PIN = 5;
const int PHOTORESISTOR_DATA_PIN = 21;
const int PHOTORESISTOR_PWR_PIN = 3;
const int DS_DATA_PIN = 19;
const int DS_PWR_PIN = 18;
const int HUMIDITY_DATA_PIN = 20;
const int HUMIDITY_PWR_PIN = 4;
const int RADIO_DATA_PIN = 15;
const int RADIO_PWR_PIN = 6;
const int LED[3] = { 11, 12, 13 };


const float airTempOffset = -2.5;
const float soilTempOffset = -2;

char charVal[24];							//temp array for intToString, floatToString, getUptimeString, getDateTimeString
#define DHTTYPE DHT22
OneWire oneWire(DS_DATA_PIN);
DallasTemperature ds(&oneWire);
DHT dht(DHT22_DATA_PIN, DHTTYPE);
RH_ASK driver(2000, 14, RADIO_DATA_PIN);



void setup() {

	Serial.begin(9600);
	setupPins();
	powerSensors(true);
	dht.begin();
	ds.begin();

	if (!driver.init())
	Serial.println("init failed");
	ledLightDigital('g');

}

void loop() {
	powerSensors(true);
	digitalWrite(RADIO_PWR_PIN, HIGH);
	delay(500);
	ledLightDigital('y');
	printSensorData();
	ledLightDigital('b');
	sendMessage();
	powerSensors(false);
	ledLightDigital('k');
	delay(4500);
}