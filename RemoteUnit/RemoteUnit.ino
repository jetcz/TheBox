#include <RunningAverage.h>
#include <SPI.h>
#include <dht.h>
#include <OneWire.h>
#include <DallasTemperature.h>
#include <JeeLib.h>
#include <Ports.h>
#include <nRF24L01.h>
#include <RF24.h>

#define DEBUG false

//structure holding data which came from remote unit
struct Payload
{
	int AirTemp; //must divide by 10
	int AirHum; //must divide by 10
	int AirHumidex; //must divide by 10
	int SoilTemp; //must divide by 10
	int SoilHum; //must divide by 10
	int Light; //must divide by 10
	unsigned int RainTips;
	unsigned long Uptime;
	int FreeRam;
	int Vcc;

	void print(){
		Serial.print(F("Air Temperature: "));
		Serial.print(AirTemp / 10.0, 1);
		Serial.println(F("C"));
		Serial.print(F("Air Humidity: "));
		Serial.print(AirHum / 10.0, 1);
		Serial.println(F("%RH"));
		Serial.print(F("Air Humidex: "));
		Serial.print(AirHumidex / 10.0, 1);
		Serial.println(F("C"));
		Serial.print(F("Soil Temperature: "));
		Serial.print(SoilTemp / 10.0, 1);
		Serial.println(F("C"));
		Serial.print(F("Soil Humidity: "));
		Serial.print(SoilHum / 10.0, 1);
		Serial.println(F("%RH"));
		Serial.print(F("Light: "));
		Serial.print(Light / 10.0, 1);
		Serial.println(F("%"));
		Serial.print(F("Rain Tips: "));
		Serial.print(RainTips);
		Serial.println();
		Serial.print(F("Uptime: "));
		Serial.print(Uptime);
		Serial.println(F("s"));
		Serial.print(F("Free RAM: "));
		Serial.print(FreeRam);
		Serial.println(F("B"));
		Serial.print(F("Vcc: "));
		Serial.print(Vcc);
		Serial.println(F("mV"));
		Serial.println();
	}

}; typedef struct Payload Payload;

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
dht DHT;
int nDHTStatus;
RF24 radio(RADIO_ENABLE_PIN, RADIO_SELECT_PIN);
const uint64_t pipes[2] = { 0x24CDABCD71LL, 0x244d52687CLL };
Payload p;

volatile unsigned int nRainTips;
int *Vcc = &p.Vcc;
unsigned long lLastTime;


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
	lLastTime = millis();

	getPayload();

#if DEBUG
	p.print();
#endif

	sendPayload();

	lLastTime = millis() - lLastTime;
	Sleepy::loseSomeTime(nMsgTimeout - lLastTime);
}
