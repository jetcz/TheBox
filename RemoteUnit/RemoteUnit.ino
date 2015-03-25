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
volatile int nRainTips = 0;
float *Vcc = &fRemoteUnitDataSet[7];

const float fAirTemperatureOffset = -0.5;
const float fSoilTemperatureOffset = 0.1;

unsigned int previousSec = 0; // last time update
unsigned int interval = 3600; // interval at which to do something (rain mm/h)


void setup() {

	noInterrupts();
	Serial.begin(9600);
	setupPins();
	ds.requestTemperatures();
	dht.begin();
	ds.begin();
	while (!driver.init()) {
		Serial.println("radio init failed");
		ledLightDigital('r');
	}
	ledLightDigital('g');
	attachInterrupt(0, ISRTipCnt, FALLING);
	nRainTips = 0;
	interrupts();
}

void loop() {
	powerSensors(true);
	delay(500);
	ledLightDigital('g');
	delayMicroseconds(1);
	ledLightDigital('k');
	prepareDataSetArrays();
	powerSensors(false);
	printSensorData();
	ledLightDigital('b');
	delayMicroseconds(100);
	ledLightDigital('k');
	sendMessage();
	delay(4500);
}
