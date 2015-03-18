/* libraries */
#include <Wire.h>
#include <RTClib.h>
#include <Adafruit_Sensor.h>
#include <Adafruit_BMP085_U.h>
#include <DHT.h>
#include <SPI.h>
#include <Ethernet.h>
#include <utility/w5100.h>
#include <SD.h>
#include <Time.h>
#include <TimeAlarms.h>
#include <RunningMedian.h>
#include <WebServer.h>
#include <RH_ASK.h>
#include <LiquidCrystal_I2C.h>

//////////////////////USER CONFIGURABLE///////////////////////////////////

/* pins */
const int RESET_ETH_SHIELD_PIN = 14;
const int DHT22_PIN = 9;
const int PIR_PIN = 8;
const int ETH_SELECT_PIN = 10;
const int SD_SELECT_PIN = 4;
const int RELAY_PIN[4] = { 22, 24, 26, 28 };
const int LED1[3] = { 44, 45, 46 };
const int LED2[3] = { 5, 6, 7 };
const int LED3[3] = { 11, 12, 13 };
const int LCD_SWITCH[3] = { 32, 34, 36 };
const int LCD_SWITCH_PWR_PIN = 30;
const int RADIO_RX_PIN = 17;
const int RADIO_CTRL_PIN = 18;


/* ThingSpeak settings */
const char cThingSpeakAddress[] = "api.thingspeak.com";
//const char cThingSpeakAddress[] = "184.106.153.149";
const char* sAPIkeys[] = { "FNHSHUE6A3XKP71C", "OL1GVYUB2HFK7E2M", "GNQST00GBW05EYGC" };
const byte iUpdateThingSpeakInterval = 10;
const byte iRemoteDataSetTimeout = 180;		//for how long is dataset valid and send to thingspeak (sec)
const byte iRestartEthernetThreshold = 10;	//if thingspeak update fails x times -> ethernet shield reset
const byte iRestartArduinoThreshold = 42;	//if thingspeak update fails x times -> arduino reset

/* lcd settings */
const byte byLcdMsgTimeout = 4;

/* led settings R, G, B*/
const byte lightIntensity[] = { 6, 1, 2 }; //0-255, these led are very bright so we need low values

/* sensor polling settings */
const byte iUpdateSensorsInterval = 20;

//////////////////////////////////////////////////////////////////////////

#define DHTTYPE DHT22
#define PREFIX ""
/* reference variables */
RTC_DS1307 rtc;
Adafruit_BMP085_Unified bmp = Adafruit_BMP085_Unified(10085);
DHT dht(DHT22_PIN, DHTTYPE);
EthernetClient client;
File myFile;
RH_ASK driver(2000, RADIO_RX_PIN, 0);
//WebServer webserver(PREFIX, 80);
LiquidCrystal_I2C lcd(0x27, 2, 1, 0, 4, 5, 6, 7, 3, POSITIVE);

/* variables */
byte iFailedCounter = 0;					 //failed thingspeak uploads
DateTime sysStart;							//time of system start for uptime 
byte iCurrentDataSet = 0;					//for cycling betweeen thingspeak datasets
char charVal[24];							//temp array for intToString, floatToString, getUptimeString, getDateTimeString
String sNow = "";							//current datetime string
String sUptime = "";						//uptime string
byte byLcdMsgTimeoutCnt = 0;
boolean bConnectivityCheck = true;

/* weather */
const char* weather[] = { "stable", "sunny", "cloudy", "unstable", "thunderstorm", "unknown" };
byte forecast = 5;
int minuteCount = 0;
boolean firstRound = true;
float pressureAvg[7];
float dP_dt;

/* arrays for sensor variables */
float fMainUnitDataSet[] = { 0, 0, 0, 0, 0 };
float fRemoteUnitDataSet[] = { 0, 0, 0, 0, 0, 0, 0 };
float fSysDataSet[] = { 0, 0, 0, 0, 0, 0 };
/* array of pointers to iterate through when updating thingspeak channels */
float* fDataSetPointer[] = { fMainUnitDataSet, fRemoteUnitDataSet, fSysDataSet };
/* sizes of our arrays */
byte arrSizes[] = { sizeof(fMainUnitDataSet) / sizeof(float*) / 2, sizeof(fRemoteUnitDataSet) / sizeof(float*) / 2, sizeof(fSysDataSet) / sizeof(float*) / 2 };

/* relay states/modes*/
byte byRelay[] = { 0, 0, 0, 0 };

/* sensor variables */
unsigned long lRemoteDataSetTimeStamp = 0;	//timestamp when was received last dataset from remote unit
/* bmp180 */
RunningMedian rmSysTemp = RunningMedian(6);
const float fSysTempOffset = -0.2;
RunningMedian rmPressure = RunningMedian(6);
const byte iPressureOffset = 24;
/* main dht22 */
RunningMedian rmMainTemp = RunningMedian(3);
RunningMedian rmMainHumidity = RunningMedian(6);
const float fMainTempOffset = -1.2;

/* network settings */
byte mac[] = { 0xB0, 0x0B, 0x5B, 0x00, 0xB5, 0x00 };
byte ip[4];
byte gateway[4];
byte subnet[4];
byte dns1[4];
boolean bDhcp;

/* timers */
byte byAlarm[7];
boolean bAlarmEnabled[] = { true, true, true, true, true, true, true };

/* reset arduino function (must be here)*/
void(*resetFunc) (void) = 0;

void setup()
{
	setupPins();
	setupSD();											//order of these
	readSDSettings("/settings/relays.ini");				//function calls must be
	switchRelays();										//like this!!!!!
	setupSerial();
	setupLCD();
	readSDSettings("/settings/ethernet.ini");
	setupWire();
	setupDHT();
	setupBMP();
	setupRTC();
	setupRadio();
	setupEthernet();
	setupTimers();
	Serial.println(F("Setup Done"));
}

/* control everything by timer alarms */
void loop()
{	
	Alarm.delay(0); //run alarms without any delay so the loop isn't slowed down
}

//TO DO
//look for different runnig average library
//use library to read ini settings
//implement ntp
//ethernet.maintain is blocking - if we dont get ip at startup, it blocks the whole unit for x (look into ethernet library) sec every  loop
//make web interface
