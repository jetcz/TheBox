/* libraries */
#include <Wire.h>
#include <RTClib.h>
#include <Adafruit_Sensor.h>
#include <Adafruit_BMP085_U.h>
#include <DHT.h>
#include <SPI.h>
#include <Ethernet.h>
#include <EthernetUdp.h>
#include <utility/w5100.h>
#include <SD.h>
#include <Time.h>
#include <TimeAlarms.h>
#include <Timezone.h>
#include <RunningAverage.h>
#include <WebServer.h>
#include <RH_ASK.h>
#include <LiquidCrystal_I2C.h>
#include <IniFile.h>
#include "avr/pgmspace.h"
#include "DataStructures.h"

//////////////////////USER CONFIGURABLE///////////////////////////////////

/* pins */
const int RESET_ETH_SHIELD_PIN = 14;
const int DHT22_PIN = 9;
const int PIR_PIN = 19;
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
const byte iUpdateThingSpeakInterval = 20;
const byte iRemoteDataSetTimeout = 180;		//for how long is dataset valid and send to thingspeak (sec)
const byte iRestartEthernetThreshold = 10;	//if thingspeak update fails x times -> ethernet shield reset
const byte iRestartArduinoThreshold = 42;	//if thingspeak update fails x times -> arduino reset

/* ntp server */
const char timeServer[] = "tik.cesnet.cz";

/* timezone settings */
TimeChangeRule CEST = { "CEST", Last, Sun, Mar, 2, 120 };    //summer time = UTC + 2 hours
TimeChangeRule CET = { "CET", Last, Sun, Oct, 3, 60 };     //winter time = UTC + 1 hours
Timezone myTZ(CEST, CET);

/* lcd settings */
const byte byLcdMsgTimeout = 4;

/* led settings R, G, B*/
const byte lightIntensity[] = { 6, 1, 2 }; //0-255, these led are very bright so we need low values

/* sensor polling settings */
const byte iUpdateSensorsInterval = 10;

/* ini files settings
relay config must be int following format:
modes = 0, 1, 1, 2 where 0 is off, 1 is on and 2 is auto */

const char *ethernet = "/settings/ethernet.ini";
const char *relays = "/settings/relays.ini";

//////////////////////////////////////////////////////////////////////////

#define DHTTYPE DHT22
#define PREFIX ""
/* reference variables */
RTC_DS1307 rtc;
Adafruit_BMP085_Unified bmp = Adafruit_BMP085_Unified(10085);
DHT dht(DHT22_PIN, DHTTYPE);
EthernetClient client;
EthernetUDP udp;
File myFile;
RH_ASK driver(1000, RADIO_RX_PIN, 0);
WebServer webserver(PREFIX, 80);
LiquidCrystal_I2C lcd(0x27, 2, 1, 0, 4, 5, 6, 7, 3, POSITIVE);

DataSet MainDS;
DataSet RemoteDS;
DataSet SystemDS;

/* general buffer for various usages (datatypes conversion, reading ini settings)*/
const size_t bufferLen = 30;
char buffer[bufferLen];

/* variables */
byte iFailedCounter = 0;					//failed thingspeak uploads
DateTime sysStart;							//time of system start for uptime 
byte iCurrentDataSet = 0;					//for cycling betweeen thingspeak datasets
String sNow = "";							//current datetime string
String sMainUptime = "";					//uptime string
String sRemoteUptime = "";					//uptime string
byte byLcdMsgTimeoutCnt = 0;
boolean bConnectivityCheck = true;

/* weather */
const char* weather[] = { "  stable", "   sunny", "  cloudy", "    unstable", "   storm", " unknown" };
byte forecast = 5;

/* array of pointers to iterate through when updating thingspeak channels */
DataSet *DataSetPointer[] = { (DataSet*)&MainDS, (DataSet*)&RemoteDS, (DataSet*)&SystemDS };

/* relay states/modes*/
byte byRelay[] = { 0, 0, 0, 0 };

/* sensor variables */
/* bmp180 */
RunningAverage rmSysTemp(6);
const float fSysTempOffset = -0.2;
RunningAverage rmPressure(6);
const byte iPressureOffset = 24;
/* main dht22 */
RunningAverage rmMainTemp(6);
RunningAverage rmMainHumidity(6);
const float fMainTempOffset = -0.9;

/* network settings */
byte mac[] = { 0xB0, 0x0B, 0x5B, 0x00, 0xB5, 0x00 };
byte ip[4];
byte gw[4];
byte subnet[4];
byte dns1[4];
boolean bDhcp;

/* alarms */
int systemAlarm;
int prepareDatasetAlarm;
int printSerialAlarm;
int updateTSAlarm;
int weatherAlarm;
int printLcdAlarm;
int dhcpAlarm;
int syncRTCAlarm;

/* reset arduino function (must be here)*/
void(*resetFunc) (void) = 0;

/* commands for webserver */
void sensorsCmd(WebServer &server, WebServer::ConnectionType type, char *, bool)
{
	if (type == WebServer::POST)
	{
		bool repeat;
		char name[16], value[16];
		do
		{
			/* readPOSTparam returns false when there are no more parameters
			* to read from the input.  We pass in buffers for it to store
			* the name and value strings along with the length of those
			* buffers. */
			repeat = server.readPOSTparam(name, 16, value, 16);

			//look for parameters
			if (strcmp(name, "r1") == 0)
			{
				byRelay[0] = strtoul(value, NULL, 10);
			}
			if (strcmp(name, "r2") == 0)
			{
				byRelay[1] = strtoul(value, NULL, 10);
			}
			if (strcmp(name, "r3") == 0)
			{
				byRelay[2] = strtoul(value, NULL, 10);
			}
			if (strcmp(name, "r4") == 0)
			{
				byRelay[3] = strtoul(value, NULL, 10);
			}
		} while (repeat);

		// after procesing the POST data, tell the web browser to reload
		// the page using a GET method. 
		server.httpSeeOther(PREFIX);
		return;
	}

	/* for a GET or HEAD, send the standard "it's all OK headers" */
	server.httpSuccess("text/html", "Connection: keep-alive"CRLF);

	/* we don't output the body for a HEAD request */
	if (type == WebServer::GET)
	{
		myFile = SD.open("/www/index.htm");        // open web page file
		if (myFile)   {
			int16_t c;
			while ((c = myFile.read()) >= 0) {
				server.print((char)c);
			}
			myFile.close();
		}
		else server.print(F("SD failed"));
	}

}
void XMLresponseCmd(WebServer &server, WebServer::ConnectionType type, char *, bool)
{
	if (type == WebServer::POST)
	{
		server.httpFail();
		return;
	}
	server.httpSuccess("text/xml", "Connection: keep-alive"CRLF);

	if (type == WebServer::GET)
	{

		P(tag_start_sensor) = "<sensor>";
		P(tag_end_sensor) = "</sensor>";

		server.print(F("<?xml version = \"1.0\" ?>"));
		server.print(F("<inputs>"));
		server.print(F("<sensors>"));

		server.printP(tag_start_sensor);
		server.print(MainDS.Data[0], 1); //maintemp
		server.printP(tag_end_sensor);

		server.printP(tag_start_sensor);
		server.print(RemoteDS.Data[0], 1); //remoteairtemp
		server.printP(tag_end_sensor);

		server.printP(tag_start_sensor);
		server.print(RemoteDS.Data[3], 1); //remotesoiltemp
		server.printP(tag_end_sensor);

		server.printP(tag_start_sensor);
		server.print(MainDS.Data[1], 0); //mainhum
		server.printP(tag_end_sensor);

		server.printP(tag_start_sensor);
		server.print(RemoteDS.Data[1], 0); //remoteairhum
		server.printP(tag_end_sensor);

		server.printP(tag_start_sensor);
		server.print(RemoteDS.Data[4], 0); //remotesoilhum
		server.printP(tag_end_sensor);

		server.printP(tag_start_sensor);
		server.print(MainDS.Data[4], 1); //pressure
		server.printP(tag_end_sensor);

		server.printP(tag_start_sensor);
		server.print(weather[forecast]); //weather
		server.printP(tag_end_sensor);

		server.printP(tag_start_sensor);
		server.print(RemoteDS.Data[5], 0); //light
		server.printP(tag_end_sensor);

		server.printP(tag_start_sensor);
		server.print(RemoteDS.Data[6], 1); //rain
		server.printP(tag_end_sensor);

		server.print(F("</sensors>"));


		server.print(F("<relays>"));
		for (int i = 0; i < 4; i++)
		{
			server.print(F("<relay>"));
			server.print(String(byRelay[i]));
			server.print(F("</relay>"));
		}
		server.print(F("</relays>"));
		server.print(F("</inputs>"));
	}
}

void setup()
{
	setupSerial();
	setupPins();
	setupSD();
	readSettings(relays);
	switchRelays();
	if (!readSettings(ethernet)) //reading ethernet settings must for some reason take place much earlier that ethernet.begin
	{
		bDhcp = true;
	}
	else bDhcp = false;
	setupLCD();
	setupWire();
	setupDHT();
	setupBMP();
	setupRTC();
	setupRadio();
	setupEthernet();
	setupAlarms();

	webserver.setDefaultCommand(&sensorsCmd);
	webserver.addCommand("sensors_relays", XMLresponseCmd);
	webserver.begin();

	MainDS.APIkey = "FNHSHUE6A3XKP71C";
	MainDS.Size = 5;
	MainDS.Valid = true;

	RemoteDS.APIkey = "OL1GVYUB2HFK7E2M";
	RemoteDS.Size = 7;
	RemoteDS.Valid = false;
	RemoteDS.Timestamp = sysStart.unixtime() - iRemoteDataSetTimeout;

	SystemDS.APIkey = "GNQST00GBW05EYGC";
	SystemDS.Size = 8;
	SystemDS.Valid = true;

	Serial.println(F("Setup Done"));
}

/* control everything by timer alarms */
void loop()
{
	Alarm.delay(0); //run alarms without any delay so the loop isn't slowed down
	webserver.processConnection();
}

//TO DO
//clean and organize project - commands for webduino shouldnt need to be in main sketch
//refactor command functions and get rid of unecessary buffers
//www - set status colors - not currently working
//handle connectivity check better without dhcp
//ethernet.maintain is blocking - if we dont get ip at startup, it blocks the whole unit for x (look into ethernet library) sec every  loop

