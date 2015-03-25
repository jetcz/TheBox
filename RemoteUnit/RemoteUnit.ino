#include <RunningAverage.h>
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
const int RADIO_TX_PIN = 15;
const int RADIO_PWR_PIN = 6;
const int LED[3] = { 13, 12, 11 };


const float airTempOffset = -2.5;
const float soilTempOffset = -2;

char buffer[24];
#define DHTTYPE DHT22
OneWire oneWire(DS_DATA_PIN);
DallasTemperature ds(&oneWire);
DHT dht(DHT22_DATA_PIN, DHTTYPE);
RH_ASK driver(2000, 14, RADIO_TX_PIN);
RunningAverage Light(3);
RunningAverage AirTemp(3);
RunningAverage AirHum(3);
RunningAverage Humidex(3);
RunningAverage SoilTemp(3);
RunningAverage SoilHum(3);

float fRemoteUnitDataSet[] = { 0, 0, 0, 0, 0, 0, 0, 0, 0 };
volatile int nRainTicks = 0;
float *Vcc = &fRemoteUnitDataSet[7];

float fAirTemperatureOffset = -0.5;
float fSoilTemperatureOffset = 0.1;


void setup() {
	attachInterrupt(0, ISRTipCnt, FALLING);
	Serial.begin(9600);
	setupPins();
	digitalWrite(DS_PWR_PIN, HIGH);
	ds.requestTemperatures();
	dht.begin();
	ds.begin();
	while (!driver.init()) {
		Serial.println("radio init failed");
		ledLightDigital('r');
	}
	ledLightDigital('g');

}

void loop() {
	powerSensors(true);
	delay(500);
	ledLightDigital('y');
	prepareDataSetArrays();
	powerSensors(false);
	printSensorData();
	ledLightDigital('b');
	sendMessage();
	ledLightDigital('k');
	delay(4500);
}
