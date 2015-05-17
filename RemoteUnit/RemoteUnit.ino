#include <RunningAverage.h>
#include <SPI.h>
#include <RH_ASK.h>
#include <DHT.h>
#include <OneWire.h>
#include <DallasTemperature.h>
#include <JeeLib.h>
#include <Ports.h>

#define debug false
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

const int nSleepTime2 = 1500;
#if debug
const int nSleepTime = 18548 - nSleepTime2 - 1000; 
#else
const int nSleepTime = 18550 - nSleepTime2;
#endif
char buffer[24];
#define DHTTYPE DHT22
OneWire oneWire(DS_DATA_PIN);
DallasTemperature ds(&oneWire);
DHT dht(DHT22_DATA_PIN, DHTTYPE);
RH_ASK driver(2000, 14, RADIO_TX_PIN);

float DS[10] = { 0 };
volatile float fRainTips = 0;
float *Vcc = &DS[7];

//handle offsets in main unit
//const float fAirTemperatureOffset = -0.7;
//const float fSoilTemperatureOffset = -0.2;

ISR(WDT_vect) { Sleepy::watchdogEvent(); }

void setup() {

	noInterrupts();
#if debug
	Serial.begin(9600);
#endif
	attachInterrupt(0, ISRTipCnt, FALLING);
	setupPins();
	dht.begin();
	ds.begin();
	ds.requestTemperatures();
	while (!driver.init()) {
		Serial.println("radio init failed");
		ledLightDigital('r');
	}
	fRainTips = 0;
	interrupts();

	//////////////////////////////////////////////////////////////////////////send initial sensor readings after startup
	digitalWrite(DHT22_PWR_PIN, HIGH);
	Sleepy::loseSomeTime(nSleepTime2);
	ledLightDigital('g');
	ledLightDigital('k');
	prepareDataSetArrays();
	digitalWrite(DHT22_PWR_PIN, LOW);
	ledLightDigital('b');
	delayMicroseconds(100);
	ledLightDigital('k');
	sendMessage();
}

void loop() {
	//////////////////////////////////////////////////////////////////////////get sensor data after 20s
	digitalWrite(DHT22_PWR_PIN, HIGH);
	Sleepy::loseSomeTime(nSleepTime2);
	ledLightDigital('g');
	ledLightDigital('k');
	prepareDataSetArrays();
	digitalWrite(DHT22_PWR_PIN, LOW);
#if debug
	printSensorData();
	delay(1000);
#endif
	Sleepy::loseSomeTime(nSleepTime);

	//////////////////////////////////////////////////////////////////////////get sensor data after 40s
	digitalWrite(DHT22_PWR_PIN, HIGH);
	Sleepy::loseSomeTime(nSleepTime2);
	ledLightDigital('g');
	ledLightDigital('k');
	prepareDataSetArrays();
	digitalWrite(DHT22_PWR_PIN, LOW);
#if debug
	printSensorData();
	delay(1000);
#endif
	Sleepy::loseSomeTime(nSleepTime);

	//////////////////////////////////////////////////////////////////////////get sensor data after 60s
	digitalWrite(DHT22_PWR_PIN, HIGH);
	Sleepy::loseSomeTime(nSleepTime2);
	ledLightDigital('g');
	ledLightDigital('k');
	prepareDataSetArrays();
	digitalWrite(DHT22_PWR_PIN, LOW);
#if debug
	printSensorData();
	delay(1000);
#endif
	//////////////////////////////////////////////////////////////////////////send sensor data
	ledLightDigital('b');
	delayMicroseconds(100);
	ledLightDigital('k');
	sendMessage();
	Sleepy::loseSomeTime(nSleepTime);
}
