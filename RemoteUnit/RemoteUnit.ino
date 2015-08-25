#include <RunningAverage.h>
#include <SPI.h>
#include <dht.h>
#include <OneWire.h>
#include <DallasTemperature.h>
#include <JeeLib.h>
#include <Ports.h>
#include <nRF24L01.h>
#include <RF24.h>

#define DEBUG true

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
	unsigned int FailedMsgs = 0;

	void print(){
		Serial.println();
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
		Serial.print(F("Failed messages: "));
		Serial.print(FailedMsgs);
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

const int nMsgTimeout = 10000;		//timeout between messsages in ms 20000
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
int *Vcc = &payload.Vcc;
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

	getPayload(); //read sensor data
	sendPayload(); //try to send data over the air; we use hardware auto ACK and retransmit AND manually retransmit few times if there is no success

	//go to sleep again for 20 seconds minus the time it took to read and send data
	lDelay = millis() - lDelay;
#if !DEBUG
	Sleepy::loseSomeTime(nMsgTimeout - lDelay);
#endif // !DEBUG
#if DEBUG
	delay(nMsgTimeout - lDelay);
#endif // !DEBUG

}
