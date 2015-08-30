#define DEBUG false

#include <RunningAverage.h>
#include <SPI.h>
#include <dht.h>
#include <OneWire.h>
#include <DallasTemperature.h>
#include <JeeLib.h>
#include <Ports.h>
#include <nRF24L01.h>
#include <RF24.h> 
#include "Payload.h" //this must the same struct as in the main unit

/* pin mappings
a0	14
a1	15
a2	16
a3	17
a4	18
a5	19
*/
const int DHT22_DATA_PIN = 3;
const int DHT22_PWR_PIN = 4;
const int PHOTORESISTOR_DATA_PIN = 16;
const int PHOTORESISTOR_PWR_PIN = 17;
const int DS_DATA_PIN = 5;
const int HUMIDITY_DATA_PIN = 14;
const int HUMIDITY_PWR_PIN = 15;
const int LED[3] = { 18, 8, 7 };
const int RADIO_ENABLE_PIN = 9;
const int RADIO_SELECT_PIN = 10;


//some settings
const int InvalidValue = -255;
const int nMsgTimeout = 20000;		//timeout between messsages in ms 20000
const byte byRadioAutoRetransmits = 15;
const long lVccCalibration = 1093800;

char buffer[24];
OneWire oneWire(DS_DATA_PIN);
DallasTemperature ds(&oneWire);
dht DHT;
int nDHTStatus;
RF24 radio(RADIO_ENABLE_PIN, RADIO_SELECT_PIN);
const uint64_t pipes[2] = { 0x24CDABCD71LL, 0x244d52687CLL };
Payload payload;

volatile unsigned int nRainTips;
unsigned int *Vcc = &payload.Vcc;
unsigned long lDelay;

ISR(WDT_vect) { Sleepy::watchdogEvent(); }

void setup() {
	
	noInterrupts();
#if DEBUG
	Serial.begin(9600);
#endif
	attachInterrupt(0, ISRTipCnt, FALLING);
	setupPins();
	setupRadio();
	ds.begin();
	ds.requestTemperatures();
	nRainTips = 0;
	interrupts();	

}

void loop() {
	lDelay = millis();

	getPayload();	//read sensor data
	sendPayload();	//try to send data over the air; we use hardware auto ACK and retransmit AND manually retransmit few times if there is no success

	//go to sleep again for 20 seconds minus the time it took to read and send data
	lDelay = millis() - lDelay;
#if !DEBUG
	Sleepy::loseSomeTime(nMsgTimeout - lDelay);
#endif // !DEBUG
#if DEBUG
	delay(nMsgTimeout - lDelay);
#endif // DEBUG

}

