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

const int nSleepTime = 18532; //18532;
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
const float fSoilTemperatureOffset = 0.4;

unsigned int previousSec = 0; // last time update
unsigned int interval = 3600; // interval at which to do something (rain mm/h)

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

	ledLightDigital('g');
	delay(200);
	ledLightDigital('m');
	delay(200);
	ledLightDigital('g');
	delay(200);
	nRainTips = 0;
	interrupts();
}

void loop() {
	//Serial.println(millis());
	//delay(1000);
	//get sensor data after 20s
	ledLightDigital('g');
	ledLightDigital('k');
	prepareDataSetArrays();
#if debug
	printSensorData();
#endif
	Sleepy::loseSomeTime(nSleepTime);

	//get sensor data after 40s
	ledLightDigital('g');
	ledLightDigital('k');
	prepareDataSetArrays();
#if debug
	printSensorData();
#endif
	Sleepy::loseSomeTime(nSleepTime);

	//get sensor data after 60s
	ledLightDigital('g');
	ledLightDigital('k');
	prepareDataSetArrays();
#if debug
	printSensorData();
#endif
	//send sensor data
	ledLightDigital('b');
	delayMicroseconds(200);
	ledLightDigital('k');
	sendMessage();
	Sleepy::loseSomeTime(nSleepTime);

}
