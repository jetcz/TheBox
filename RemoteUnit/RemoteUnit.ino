#include <RunningAverage.h>
#include <SPI.h>
#include <RH_NRF24.h>
#include <dht.h>
#include <OneWire.h>
#include <DallasTemperature.h>
#include <JeeLib.h>
#include <Ports.h>

#define DEBUG false

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

const int RADIO_ENABLE_PIN = 15;
const int RADIO_SELECT_PIN = 15;

const int nMsgTimeout = 20000;		//timeout between messsages in ms
const long lVccCalibration = 1093800;

char buffer[24];
OneWire oneWire(DS_DATA_PIN);
DallasTemperature ds(&oneWire);
dht dht1;
int dhtStatus;
RH_NRF24 nrf24(RADIO_ENABLE_PIN, RADIO_SELECT_PIN);
//RH_ASK driver(2000, 14, RADIO_TX_PIN);

float DS[11] = { 0 };
volatile float fRainTips = 0;
float *Vcc = &DS[7];
unsigned long lLastTime;

ISR(WDT_vect) { Sleepy::watchdogEvent(); }

void setup() {
	DS[10] = nMsgTimeout / 1000;
	noInterrupts();
#if DEBUG
	Serial.begin(9600);
#endif
	attachInterrupt(0, ISRTipCnt, FALLING);
	setupPins();
	ds.begin();
	ds.requestTemperatures();
	setupRadio();
	fRainTips = 0;
	interrupts();
}

void loop() {	
	lLastTime = millis();

	ledLightDigital('g');
	ledLightDigital('k');

	getDataSet();

#if DEBUG
	printSensorData();
#endif

	sendMessage();

	lLastTime = millis() - lLastTime;
	Sleepy::loseSomeTime(nMsgTimeout - lLastTime);
}
