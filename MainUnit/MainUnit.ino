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

#define DEBUG true //enable disable all serial.print messages

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
const char cTimeServer[] = "tik.cesnet.cz";

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

/* sensor offsets */
const float fSysTempOffset = -0.2;
const byte iPressureOffset = 24;
const float fMainTempOffset = -0.9;

/* ini files settings
relay config must be in following format:
modes = 0, 1, 1, 2 where 0 is off, 1 is on and 2 is auto */

char *ethernet = "/settings/ethernet.ini";
char *relays = "/settings/relays.ini";

//////////////////////////////////////////////////////////////////////////

/* reference variables */
RTC_DS1307 rtc;
Adafruit_BMP085_Unified bmp = Adafruit_BMP085_Unified(10085);
DHT dht(DHT22_PIN, DHT22);
EthernetClient client;
EthernetUDP udp;
File myFile;
RH_ASK driver(1000, RADIO_RX_PIN, 0);
WebServer webserver("", 80);
LiquidCrystal_I2C lcd(0x27, 2, 1, 0, 4, 5, 6, 7, 3, POSITIVE);

DataSet MainDS;
DataSet RemoteDS;
DataSet SystemDS;

/* general buffer for various usages (datatypes conversion, reading ini settings)*/
const size_t bufferLen = 30;
char buffer[bufferLen];

/* variables */
int iFailedCounter = 0;					//failed thingspeak uploads
unsigned int iFailedCntTSTotal = 0;		//total failed thing speak messages
unsigned int iFailedCntRadioTotal = 0;
DateTime sysStart;							//time of system start for uptime 
DateTime lastNTPsync;
byte iCurrentDataSet = 0;					//for cycling betweeen thingspeak datasets
String sNow = "";							//current datetime string
String sMainUptime = "";					//uptime string
String sRemoteUptime = "";					//uptime string
boolean bConnectivityCheck = true;

/* weather */
const char* weather[] = { "  stable", "   sunny", "  cloudy", "    unstable", "   storm", " unknown" };
byte forecast = 5;

/* array of pointers to iterate through when updating thingspeak channels */
DataSet *DataSetPtr[] = { (DataSet*)&MainDS, (DataSet*)&RemoteDS, (DataSet*)&SystemDS };

/*Target variables are :
0 MainPir
1 MainTemperature
2 MainHumidity
3 MainHumidex
4 RemoteTemperature
5 RemoteHumidity
6 RemoteHumidex
7 RemoteSoilTemperature
8 RemoteSoilHumidity
9 Light
10 Rain
*/
float *TargetVarPtr[] = { &MainDS.Data[3], &MainDS.Data[0], &MainDS.Data[1], &MainDS.Data[2], &RemoteDS.Data[0], &RemoteDS.Data[1], &RemoteDS.Data[2], &RemoteDS.Data[3], &RemoteDS.Data[4], &RemoteDS.Data[5], &RemoteDS.Data[6] };

/* relay states/modes*/
byte byRelay[4] = { 0 };

/* network settings */
byte mac[] = { 0xB0, 0x0B, 0x5B, 0x00, 0xB5, 0x00 };
byte ip[4] = { 0 };
byte gw[4] = { 0 };
byte subnet[4] = { 0 };
byte dns1[4] = { 0 };
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
int writeSDAlarm;

/* reset arduino function (must be here)*/
void(*resetFunc) (void) = 0;

/* commands for webserver */
void homePageCmd(WebServer &server, WebServer::ConnectionType type, char *, bool)
{
	server.httpSuccess("text/html", "Connection: keep-alive"CRLF);
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
void sensorsXMLCmd(WebServer &server, WebServer::ConnectionType type, char *, bool)
{
	if (type == WebServer::POST)
	{
		server.httpFail();
		return;
	}
	server.httpSuccess("text/xml", "Connection: keep-alive"CRLF);

	if (type == WebServer::GET)
	{

		P(tag_start_sensor) = "<V>";
		P(tag_end_sensor) = "</V>";

		server.print(F("<?xml version = \"1.0\" ?>"));
		server.print(F("<Inputs>"));
		server.print(F("<OK>"));
		server.print(RemoteDS.Valid); //remote data set valid?
		server.print(F("</OK>"));
		server.print(F("<Sen>"));

		server.printP(tag_start_sensor);
		server.print(MainDS.Data[0], 1); //mainairtemp
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

		server.print(F("</Sen>"));

		server.print(F("<Relays>"));
		server.print(F("<Mod>"));
		for (int i = 0; i < 4; i++)
		{
			server.print(F("<M>"));
			server.print(byRelay[i]);
			server.print(F("</M>"));
		}
		server.print(F("</Mod>"));
		server.print(F("<States>"));

		for (int i = 0; i < 4; i++)
		{
			server.print(F("<S>"));
			server.print(int(getRelayState(i)));
			server.print(F("</S>"));
		}
		server.print(F("</States>"));
		server.print(F("</Relays>"));
		server.print(F("</Inputs>"));
	}
}
void relayDataCmd(WebServer &server, WebServer::ConnectionType type, char *url_param, bool param_complete)
{
	server.httpSuccess();
	char name[3];
	char value[3];
	if (type == WebServer::POST)
	{
		while (server.readPOSTparam(name, 3, value, 3))
		{
			byRelay[atoi(name) - 1] = atoi(value);
		}
		switchRelays();
		Alarm.disable(writeSDAlarm);
		writeSDAlarm = Alarm.timerOnce(5, writeSD);
	}
}
void graphs1PageCmd(WebServer &server, WebServer::ConnectionType type, char *, bool)
{
	server.httpSuccess("text/html"CRLF);
	if (type == WebServer::GET)
	{
		myFile = SD.open("/www/graphs1.htm");        // open web page file
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
void graphs2PageCmd(WebServer &server, WebServer::ConnectionType type, char *, bool)
{
	server.httpSuccess("text/html"CRLF);
	if (type == WebServer::GET)
	{
		myFile = SD.open("/www/graphs2.htm");        // open web page file
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
void schedPageCmd(WebServer &server, WebServer::ConnectionType type, char *, bool)
{
	server.httpSuccess("text/html"CRLF);
	if (type == WebServer::GET)
	{
		myFile = SD.open("/www/sched.htm");        // open web page file
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
void schedXMLCmd(WebServer &server, WebServer::ConnectionType type, char *, bool)
{

}
void schedDataCmd(WebServer &server, WebServer::ConnectionType type, char *, bool)
{

}
void systemPageCmd(WebServer &server, WebServer::ConnectionType type, char *, bool)
{
	server.httpSuccess("text/html"CRLF);
	if (type == WebServer::GET)
	{
		myFile = SD.open("/www/system.htm");        // open web page file
		if (myFile)   {
			int16_t c;
			while ((c = myFile.read()) >= 0) {
				server.print((char)c);
			}
			myFile.close();
		}
		else server.print(F("SD failed"));
	}
};
void statsXMLCmd(WebServer &server, WebServer::ConnectionType type, char *, bool){


	if (type == WebServer::POST)
	{
		server.httpFail();
		return;
	}
	server.httpSuccess("text/xml", "Connection: keep-alive"CRLF);

	if (type == WebServer::GET)
	{
		P(tag_start_sensor) = "<V>";
		P(tag_end_sensor) = "</V>";

		server.print(F("<?xml version = \"1.0\" ?>"));
		server.print(F("<Sys>"));
		server.print(F("<Time>"));
		server.print(F("<Loc>"));
		server.print(sNow);
		server.print(F("</Loc>"));
		server.print(F("<Sync>"));
		if (!isnan(lastNTPsync.unixtime())) server.print("never");
		else server.print(getUptimeString(DateTime(now()) - lastNTPsync) + " ago");
		server.print(F("</Sync>"));
		server.print(F("</Time>"));
		server.print(F("<Stats>"));

		server.printP(tag_start_sensor);
		server.print(sMainUptime);
		server.printP(tag_end_sensor);

		server.printP(tag_start_sensor);
		server.print(sRemoteUptime);
		server.printP(tag_end_sensor);

		server.printP(tag_start_sensor);
		server.print(readVcc());
		server.printP(tag_end_sensor);

		server.printP(tag_start_sensor);
		server.print(int(SystemDS.Data[6]));
		server.printP(tag_end_sensor);

		server.printP(tag_start_sensor);
		server.print(now() - MainDS.Timestamp.unixtime());
		server.printP(tag_end_sensor);

		server.printP(tag_start_sensor);
		if (!isnan(RemoteDS.Timestamp.unixtime())) server.print(9999);
		else server.print(now() - RemoteDS.Timestamp.unixtime());
		server.printP(tag_end_sensor);

		server.printP(tag_start_sensor);
		server.print(iFailedCntTSTotal);
		server.printP(tag_end_sensor);

		server.printP(tag_start_sensor);
		server.print(iFailedCntRadioTotal);
		server.printP(tag_end_sensor);

		server.print(F("</Stats>"));
		server.print(F("</Sys>"));
	};

};
void networkXMLCmd(WebServer &server, WebServer::ConnectionType type, char *, bool){

	if (type == WebServer::POST)
	{
		server.httpFail();
		return;
	}
	server.httpSuccess("text/xml", "Connection: keep-alive"CRLF);

	if (type == WebServer::GET)
	{

		P(tag_start_sensor) = "<V>";
		P(tag_end_sensor) = "</V>";

		server.print(F("<?xml version = \"1.0\" ?>"));
		server.print(F("<Net>"));

		for (int i = 0; i < 4; i++)
		{
			server.printP(tag_start_sensor);
			server.print(ip[i]);
			server.printP(tag_end_sensor);
		}
		for (int i = 0; i < 4; i++)
		{
			server.printP(tag_start_sensor);
			server.print(subnet[i]);
			server.printP(tag_end_sensor);
		}
		for (int i = 0; i < 4; i++)
		{
			server.printP(tag_start_sensor);
			server.print(gw[i]);
			server.printP(tag_end_sensor);
		}
		for (int i = 0; i < 4; i++)
		{
			server.printP(tag_start_sensor);
			server.print(dns1[i]);
			server.printP(tag_end_sensor);
		}
		server.print(F("</Net>"));
	};
}
void rebootCmd(WebServer &server, WebServer::ConnectionType type, char *, bool)
{
	P(message) =
		"<!DOCTYPE html><html><head>"
		"<meta http-equiv=\"refresh\" content=\"3; url=system.htm\">"
		"<script language=\"javascript\">"
		"setTimeout(function(){ location.href = \"system.htm\" }, 3000);"
		"</script>"
		"<link rel=\"stylesheet\" type=\"text / css\" href=\"http://jet.php5.cz/thebox/css/general.css\">"
		"</head>"
		"<body>"
		"<div class=\"content\" style=\"font-weight:bold\">"
		"Rebooting, please wait..."
		"</div>"
		"</body>"
		"</html>";
	server.printP(message);
	server.flushBuf();
	resetFunc();
}
void networkDataCmd(WebServer &server, WebServer::ConnectionType type, char *, bool)
{
	server.httpSuccess();
	char value[4];
	byte counter[4] = { 0 };
	if (type == WebServer::POST)
	{
		while (server.readPOSTparam(buffer, 10, value, 4))
		{
			if (strcmp(buffer, "IP") == 0)
			{
				ip[counter[0]] = atoi(value);
				counter[0]++;
			}
			if (strcmp(buffer, "Mask") == 0)
			{
				subnet[counter[1]] = atoi(value);
				counter[1]++;
			}
			if (strcmp(buffer, "GW") == 0)
			{
				gw[counter[2]] = atoi(value);
				counter[2]++;
			}
			if (strcmp(buffer, "DNS") == 0)
			{
				dns1[counter[3]] = atoi(value);
				counter[3]++;
			}
		}

		//build new ip to string for future redirect
		String sNewIP = "";
		for (int i = 0; i < 4; i++)
		{
			sNewIP += String(ip[i]);
			if (i < 3)
			{
				sNewIP += ".";
			}
			else sNewIP += "/";
		}


		P(saveNetSucces1) =
			"<!DOCTYPE html><html><head>"
			"<script language=\"javascript\">"
			"setTimeout(function(){ location.href = \"http://";
		P(saveNetSucces2) = "system.htm\" }, 3000);"
			"</script>"
			"<link rel=\"stylesheet\" type=\"text / css\" href=\"http://jet.php5.cz/thebox/css/general.css\">"
			"</head>"
			"<body>"
			"<div class=\"content\" style=\"color:green;font-weight:bold\">"
			"Network settings saved successfully"
			"</div>"
			"</body>"
			"</html>";
		P(saveNetFail) =
			"<!DOCTYPE html><html><head>"
			"<meta http-equiv=\"refresh\" content=\"4; url=system.htm\">"
			"<script language=\"javascript\">"
			"setTimeout(function(){ location.href = \"system.htm\" }, 4000);"
			"</script>"
			"<link rel=\"stylesheet\" type=\"text / css\" href=\"http://jet.php5.cz/thebox/css/general.css\">"
			"</head>"
			"<body>"
			"<div class=\"content\" style=\"color:red;font-weight:bold\">"
			"Saving network settings failed!"
			"</div>"
			"</body>"
			"</html>";
		if (writeSDEthernetSettings()) {
			server.printP(saveNetSucces1);
			server.print(sNewIP);
			server.printP(saveNetSucces2);
			server.flushBuf();
			setupEthernet();
		}
		else {
			server.printP(saveNetFail);
			server.flushBuf();
		}
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

	webserver.setDefaultCommand(&homePageCmd); //get page
	webserver.addCommand("index.htm", homePageCmd); //get page
	webserver.addCommand("sensors.xml", sensorsXMLCmd); //get xml
	webserver.addCommand("relays.data", relayDataCmd); //post data
	webserver.addCommand("graphs1.htm", graphs1PageCmd); //get page
	webserver.addCommand("graphs2.htm", graphs2PageCmd); //get page
	webserver.addCommand("sched.htm", schedPageCmd); //get page
	webserver.addCommand("sched.xml", schedXMLCmd); //get xml
	webserver.addCommand("sched.data", schedDataCmd); //post data
	webserver.addCommand("system.htm", systemPageCmd); //get page
	webserver.addCommand("stats.xml", statsXMLCmd); //get xml
	webserver.addCommand("network.xml", networkXMLCmd); //get xml
	webserver.addCommand("network.data", networkDataCmd); //post data
	webserver.addCommand("reboot", rebootCmd);
	webserver.begin();

	MainDS.APIkey = "FNHSHUE6A3XKP71C";
	MainDS.Size = 5;
	MainDS.Valid = true;
	MainDS.Timestamp = sysStart.unixtime();

	RemoteDS.APIkey = "OL1GVYUB2HFK7E2M";
	RemoteDS.Size = 7;
	RemoteDS.Valid = false;

	SystemDS.APIkey = "GNQST00GBW05EYGC";
	SystemDS.Size = 8;
	SystemDS.Valid = true;

#if DEBUG
	Serial.println(F("Setup Done"));
#endif
}

/* control everything by timer alarms */
void loop()
{
	Alarm.delay(0); //run alarms without any delay so the loop isn't slowed down
	webserver.processConnection();
}

//TO DO
//clean and organize project - commands for webduino shouldnt need to be in main sketch
//handle connectivity check better without dhcp
//ethernet.maintain is blocking - if we dont get ip at startup, it blocks the whole unit for x (look into ethernet library) sec every  loop

