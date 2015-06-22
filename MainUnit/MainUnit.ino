#include <Wire.h>
#include <SPI.h>
#include <RTClib.h>
#include <Adafruit_Sensor.h>
#include <Adafruit_BMP085_U.h>
#include <SD.h>
#include <utility/w5100.h>
#include <EthernetUdp.h>
#include <Timezone.h>
#include <Time.h>
#include <LiquidCrystal_I2C.h>
#include <IniFile.h>
#include <QueueArray.h>
#include <RunningAverage.h>
#include "avr/pgmspace.h"
//slightly modified libs, can use default
#include <DHT.h>
#include <Ethernet.h>
#include <WebServer.h>
#include <RH_ASK.h>
//modified, cannot use default (refer to readme)
#include <EmonLib.h>
#include <TimeAlarms.h>
#include "DataStructures.h"

#define PRINT_SUMMARY false
#define DEBUG false

//my arduino specific calibration constant for reading vcc
const float lVccCalibration = 1100000;

//setup pins
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
const int VOLTAGE_PIN = 54;
const int CURRENT_LEFT_PIN = 63;
const int CURRENT_RIGHT_PIN = 58;

//general buffers for various usages (datatypes conversion, reading ini settings etc)
const int nBuffLen1 = 30;
const int nBuffLen2 = 22;
char cBuff1[nBuffLen1];
char cBuff2[nBuffLen2];

//timezone settings
TimeChangeRule CEST = { "CEST", Last, Sun, Mar, 2, 120 };    //summer time = UTC + 2 hours
TimeChangeRule CET = { "CET", Last, Sun, Oct, 3, 60 };		 //winter time = UTC + 1 hours

//global reference variables
RTC_DS1307 rtc;
Adafruit_BMP085_Unified bmp = Adafruit_BMP085_Unified(10085);
DHT dht;
EthernetClient client;
EthernetUDP udp;
File file;
RH_ASK driver(2000, RADIO_RX_PIN, 0);
WebServer webserver("", 80);
LiquidCrystal_I2C lcd(0x27, 2, 1, 0, 4, 5, 6, 7, 3, POSITIVE);
Timezone myTZ(CEST, CET);
TimeChangeRule *tcr;
EnergyMonitor emon;
float Vcc;

//initialize custom structs
SystemSettings Settings;			//here are all user configurables
DataSet MainDS;						//main dataset
DataSet RemoteDS;					//remote dataset
DataSet SystemDS;					//system dataset (warning, system dataset contains values from both main and remote unit)
RelayScheduler Sched[4];			//scheduler settings

//global variables
unsigned int nFailedCounter;			//failed thingspeak uploads
unsigned int nFailedCntTSTotal;			//total failed thing speak messages
unsigned int nFailedCntRadioTotal;		//total failed radio messages
DateTime dtSysStart;					//time of system start for uptime 
DateTime dtLastNTPsync;					//time of last ntp sync
String sNow;							//current datetime string
String sMainUptime;						//uptime string
String sRemoteUptime;					//uptime string
bool bReceivedRadioMsg = false;			//received at least one remote ds
float fRainTicks;
unsigned int nRemoteFreeRam;
unsigned int nMainFreeRam;

//weather variables
const char* cWeather[] = { "  stable", "   sunny", "  cloudy", "    unstable", "   storm", " unknown" };
byte byForecast = 5;

//array of pointers to iterate through when updating thingspeak channels
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
float *TargetVarPtr[] = {
	&MainDS.Data[3]
	, &MainDS.Data[0]
	, &MainDS.Data[1]
	, &MainDS.Data[2]
	, &RemoteDS.Data[0]
	, &RemoteDS.Data[1]
	, &RemoteDS.Data[2]
	, &RemoteDS.Data[3]
	, &RemoteDS.Data[4]
	, &RemoteDS.Data[5]
	, &RemoteDS.Data[6] };

//Alarm variables
int printSerialAlarm;
int printLcdAlarm;
int updateTSAlarm;
int dhcpAlarm;
int writeSDAlarm;
int getInitialTipCntAlarm;
int failedRadioMessagesAlarm;

//reboot arduino
void(*resetFunc) (void) = 0;

#pragma region webduino
P(messageSDFail) =
"<!DOCTYPE html><html><head>"
"<meta http-equiv=\"refresh\" content=\"2\">"
"<script language=\"javascript\">"
"setTimeout(function(){ location.reload(); }, 2000);"
"</script>"
"<link rel=\"stylesheet\" type=\"text / css\" href=\"http://jet.php5.cz/thebox/css/general.css\">"
"</head>"
"<body>"
"<div class=\"content\" style=\"color:red;font-weight:bold\">"
"SD Card failed! Reloading..."
"</div>"
"</body>"
"</html>";

/* commands for webserver */
void homePageCmd(WebServer &server, WebServer::ConnectionType type, char *, bool)
{
	ledLight(1, 'b');
	server.httpSuccess();
	if (type == WebServer::GET)
	{
		file = SD.open("/www/index.htm");        // open web page file
		if (file)   {
			int16_t c;
			while ((c = file.read()) >= 0) {
				server.print((char)c);
			}
			file.close();
		}
		else server.printP(messageSDFail);
	}
	ledLight(1, 'g');
}
void sensorsXMLCmd(WebServer &server, WebServer::ConnectionType type, char *, bool)
{
	ledLight(1, 'c');
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
		server.print(cWeather[byForecast]); //weather
		server.printP(tag_end_sensor);

		server.printP(tag_start_sensor);
		server.print(RemoteDS.Data[5], 0); //light
		server.printP(tag_end_sensor);

		server.printP(tag_start_sensor);
		server.print(RemoteDS.Data[6], 1); //rain /hour
		server.printP(tag_end_sensor);

		server.printP(tag_start_sensor);
		server.print(RemoteDS.Data[7], 1); //rain /day
		server.printP(tag_end_sensor);

		server.printP(tag_start_sensor);
		if (getRelayState(0))
		{
			server.print(MainDS.Data[5], (MainDS.Data[5] >= 100) ? 0 : 1);	//left socket pwr
			server.print("W");
		}
		else server.print("");

		server.printP(tag_end_sensor);

		server.printP(tag_start_sensor);
		if (getRelayState(3))
		{
			server.print(MainDS.Data[6], (MainDS.Data[6] >= 100) ? 0 : 1);	//right soceket pwr
			server.print("W");
		}
		else server.print("");
		server.printP(tag_end_sensor);

		server.print(F("</Sen>"));

		server.print(F("<Relays>"));
		server.print(F("<Mod>"));
		for (int i = 0; i < 4; i++)
		{
			server.print(F("<M>"));
			server.print(Settings.RelayMode[i]);
			server.print(F("</M>"));
		}
		server.print(F("</Mod>"));
		server.print(F("<States>"));

		for (int i = 0; i < 4; i++)
		{
			server.print(F("<S>"));
			server.print(getRelayState(i));
			server.print(F("</S>"));
		}
		server.print(F("</States>"));
		server.print(F("</Relays>"));
		server.print(F("</Inputs>"));
	}
	ledLight(1, 'g');
}
void relayDataCmd(WebServer &server, WebServer::ConnectionType type, char *, bool)
{
	ledLight(1, 'y');
	server.httpSuccess();
	char name[3];
	char value[3];
	if (type == WebServer::POST)
	{
		while (server.readPOSTparam(name, 3, value, 3))
		{
			Settings.RelayMode[atoi(name) - 1] = atoi(value);
		}
		switchRelays();
		Alarm.disable(writeSDAlarm);
		writeSDAlarm = Alarm.timerOnce(5, writeSD);
	}
	ledLight(1, 'g');
}
void graphs1PageCmd(WebServer &server, WebServer::ConnectionType type, char *, bool)
{
	ledLight(1, 'b');
	server.httpSuccess();
	if (type == WebServer::GET)
	{
		file = SD.open("/www/graphs1.htm");        // open web page file
		if (file)   {
			int16_t c;
			while ((c = file.read()) >= 0) {
				server.print((char)c);
			}
			file.close();
		}
		else server.printP(messageSDFail);
	}
	ledLight(1, 'g');
}
void graphs2PageCmd(WebServer &server, WebServer::ConnectionType type, char *, bool)
{
	ledLight(1, 'b');
	server.httpSuccess();
	if (type == WebServer::GET)
	{
		file = SD.open("/www/graphs2.htm");        // open web page file
		if (file)   {
			int16_t c;
			while ((c = file.read()) >= 0) {
				server.print((char)c);
			}
			file.close();
		}
		else server.printP(messageSDFail);
	}
	ledLight(1, 'g');
}
void schedPageCmd(WebServer &server, WebServer::ConnectionType type, char *, bool)
{
	ledLight(1, 'b');
	server.httpSuccess();
	if (type == WebServer::GET)
	{
		file = SD.open("/www/sched.htm");        // open web page file
		if (file)   {
			int16_t c;
			while ((c = file.read()) >= 0) {
				server.print((char)c);
			}
			file.close();
		}
		else server.printP(messageSDFail);
	}
	ledLight(1, 'g');
}
void schedXMLCmd(WebServer &server, WebServer::ConnectionType type, char *, bool)
{
	ledLight(1, 'c');
	if (type == WebServer::POST)
	{
		server.httpFail();
		return;
	}
	server.httpSuccess("text/xml");

	if (type == WebServer::GET)
	{
		server.print(F("<?xml version = \"1.0\" ?>"));
		server.print(F("<Schedulers>"));
		for (int i = 0; i < 4; i++)
		{
			server.print(F("<Relay>"));
			server.print(F("<Variable>"));
			server.print(Sched[i].Variable);
			server.print(F("</Variable>"));

			for (int j = 0; j < 5; j++)
			{
				server.print(F("<Interval>"));
				server.print(F("<Enabled>"));
				server.print(Sched[i].Enabled[j]);
				server.print(F("</Enabled>"));
				server.print(F("<Time>"));
				server.print(F("<H>"));
				server.print(Sched[i].Time[j][0]);
				server.print(F("</H>"));
				server.print(F("<M>"));
				server.print(Sched[i].Time[j][1]);
				server.print(F("</M>"));
				server.print(F("</Time>"));
				server.print(F("<Value>"));
				server.print(F("<From>"));
				server.print(Sched[i].Value[j][0]);
				server.print(F("</From>"));
				server.print(F("<To>"));
				server.print(Sched[i].Value[j][1]);
				server.print(F("</To>"));
				server.print(F("</Value>"));
				server.print(F("</Interval>"));
			}
			server.print(F("</Relay>"));
		}
		server.print(F("</Schedulers>"));
	}
	ledLight(1, 'g');
}
void schedDataCmd(WebServer &server, WebServer::ConnectionType type, char *, bool)
{
	ledLight(1, 'y');

	server.httpSuccess();

	if (type == WebServer::POST)
	{
		while (server.readPOSTparam(cBuff1, nBuffLen1, cBuff2, nBuffLen2)) //buffer = name, buff = value
		{
			if (cBuff1[0] == 'R')
			{
				int _relay = cBuff1[1] - '1';
				int _interval = cBuff1[3] - '1';

				if (cBuff1[2] == 'V') {
					Sched[_relay].Variable = atoi(cBuff2);
				}

				if (cBuff1[2] == 'I') {
					Sched[_relay].Enabled[_interval] = cBuff2[0] != '0';
				}

				if (cBuff1[2] == 'H')
				{
					Sched[_relay].Time[_interval][0] = atoi(cBuff2);
				}
				if (cBuff1[2] == 'M')
				{
					Sched[_relay].Time[_interval][1] = atoi(cBuff2);
				}
				if (cBuff1[2] == 'F')
				{
					Sched[_relay].Value[_interval][0] = atof(cBuff2);
				}
				if (cBuff1[2] == 'T')
				{
					Sched[_relay].Value[_interval][1] = atof(cBuff2);
				}

			}
		}
	}

#if DEBUG
	for (int i = 0; i < 4; i++)
	{
		Serial.print(F("Variable "));
		Serial.println(Sched[i].Variable);
		Serial.println(F("Enabled\tTime\t\tValue"));
		for (int j = 0; j < 5; j++)
		{
			Serial.print(Sched[i].Enabled[j]);
			Serial.print("\t");
			Serial.print(Sched[i].Time[j][0]);
			Serial.print(":");
			Serial.print(Sched[i].Time[j][1]);
			Serial.print("\t\t");
			Serial.print(Sched[i].Value[j][0]);
			Serial.print("-");
			Serial.println(Sched[i].Value[j][1]);
		}
	}
#endif // debug

	P(messageSuccess) =
		"<!DOCTYPE html><html><head>"
		"<meta http-equiv=\"refresh\" content=\"2; url=sched.htm\">"
		"<script language=\"javascript\">"
		"setTimeout(function(){ location.href = \"sched.htm\" }, 2000);"
		"</script>"
		"<link rel=\"stylesheet\" type=\"text / css\" href=\"http://jet.php5.cz/thebox/css/general.css\">"
		"</head>"
		"<body>"
		"<div class=\"content\" style=\"color:green;font-weight:bold\">"
		"Scheduler settings successfully saved"
		"</div>"
		"</body>"
		"</html>";

	P(messageFail) =
		"<!DOCTYPE html><html><head>"
		"<meta http-equiv=\"refresh\" content=\"2; url=sched.htm\">"
		"<script language=\"javascript\">"
		"setTimeout(function(){ location.href = \"sched.htm\" }, 2000);"
		"</script>"
		"<link rel=\"stylesheet\" type=\"text / css\" href=\"http://jet.php5.cz/thebox/css/general.css\">"
		"</head>"
		"<body>"
		"<div class=\"content\" style=\"color:red;font-weight:bold\">"
		"Failed to save scheduler settings!"
		"</div>"
		"</body>"
		"</html>";

	if (writeSDSched()) server.printP(messageSuccess);
	else server.printP(messageFail);
	server.flushBuf();

	ledLight(1, 'g');
}
void schedDeleteCmd(WebServer &server, WebServer::ConnectionType type, char *, bool)
{
	ledLight(1, 'b');
	server.httpSuccess();
	P(message) =
		"<!DOCTYPE html><html><head>"
		"<meta http-equiv=\"refresh\" content=\"1; url=sched.htm\">"
		"<script language=\"javascript\">"
		"setTimeout(function(){ location.href = \"sched.htm\" }, 1000);"
		"</script>"
		"<link rel=\"stylesheet\" type=\"text / css\" href=\"http://jet.php5.cz/thebox/css/general.css\">"
		"</head>"
		"<body>"
		"<div class=\"content\" style=\"font-weight:bold\">"
		"Deleting scheduler settings, please wait..."
		"</div>"
		"</body>"
		"</html>";
	server.printP(message);
	server.flushBuf();
	deleteSDSched();
	for (int i = 0; i < 4; i++)
	{
		Sched[i].setDefault();
	}
	ledLight(1, 'g');
}
void systemPageCmd(WebServer &server, WebServer::ConnectionType type, char *, bool)
{
	ledLight(1, 'b');
	server.httpSuccess();
	if (type == WebServer::GET)
	{
		file = SD.open("/www/system.htm");        // open web page file
		if (file)   {
			int16_t c;
			while ((c = file.read()) >= 0) {
				server.print((char)c);
			}
			file.close();
		}
		else server.printP(messageSDFail);
	}
	ledLight(1, 'g');
};
void statsXMLCmd(WebServer &server, WebServer::ConnectionType type, char *, bool){
	ledLight(1, 'c');

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
		server.print(sNow + " " + tcr->abbrev);
		server.print(F("</Loc>"));
		server.print(F("<Sync>"));
		if (now() - dtLastNTPsync.unixtime() > 1000000000) server.print(F("never"));
		else
		{
			server.print(getUptimeString(DateTime(now()) - dtLastNTPsync));
			server.print(F(" ago"));
		}
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
		server.print(nMainFreeRam);		 //free ram main unit
		server.printP(tag_end_sensor);

		server.printP(tag_start_sensor);
		server.print(nRemoteFreeRam);	//free ram remote unit
		server.printP(tag_end_sensor);

		server.printP(tag_start_sensor);
		server.print(SystemDS.Data[0], 1); //systemp
		server.printP(tag_end_sensor);

		server.printP(tag_start_sensor);
		server.print(Vcc, 0);
		server.print(F("mV / "));
		server.print(MainDS.Data[7], 1);
		server.print(F("V"));
		server.printP(tag_end_sensor);

		server.printP(tag_start_sensor);
		server.print(int(SystemDS.Data[6]));
		server.print(F("mV"));
		server.printP(tag_end_sensor);

		server.printP(tag_start_sensor);
		server.print(now() - MainDS.Timestamp.unixtime());
		server.printP(tag_end_sensor);

		server.printP(tag_start_sensor);
		if (bReceivedRadioMsg) server.print(now() - RemoteDS.Timestamp.unixtime());
		else server.print(F("9999"));
		server.printP(tag_end_sensor);

		server.printP(tag_start_sensor);
		server.print(nFailedCntTSTotal);
		server.printP(tag_end_sensor);

		server.printP(tag_start_sensor);
		server.print(nFailedCntRadioTotal);
		server.printP(tag_end_sensor);

		server.print(F("</Stats>"));
		server.print(F("</Sys>"));
	};
	ledLight(1, 'g');
};
void settingsXMLCmd(WebServer &server, WebServer::ConnectionType type, char *, bool)
{
	ledLight(1, 'c');
	if (type == WebServer::POST)
	{
		server.httpFail();
		return;
	}
	server.httpSuccess("text/xml");

	if (type == WebServer::GET)
	{

		P(tag_start_sensor) = "<V>";
		P(tag_end_sensor) = "</V>";

		server.print(F("<?xml version = \"1.0\" ?>"));
		server.print(F("<Settings>"));
		server.print(F("<General>"));
		server.print(F("<RadioMsgInterval>"));
		server.print(Settings.RadioMsgInterval);
		server.print(F("</RadioMsgInterval>"));
		server.print(F("<RemoteDSTimeout>"));
		server.print(Settings.RemoteDataSetTimeout);
		server.print(F("</RemoteDSTimeout>"));
		server.print(F("<InvalidDSAction>"));
		server.print(Settings.InvalidDSAction);
		server.print(F("</InvalidDSAction>"));
		server.print(F("<TSEnabled>"));
		server.print(Settings.TSenabled);
		server.print(F("</TSEnabled>"));
		server.print(F("<TSAddr>"));
		server.print(Settings.ThingSpeakAddress);
		server.print(F("</TSAddr>"));
		server.print(F("<NTPAddr>"));
		server.print(Settings.NTPServer);
		server.print(F("</NTPAddr>"));
		server.print(F("</General>"));

		server.print(F("<Offsets>"));
		server.print(F("<SysTempOffset>"));
		server.print(Settings.SysTempOffset);
		server.print(F("</SysTempOffset>"));
		server.print(F("<PressureOffset>"));
		server.print(Settings.PressureOffset);
		server.print(F("</PressureOffset>"));
		server.print(F("<MainTempOffset>"));
		server.print(Settings.MainTempOffset);
		server.print(F("</MainTempOffset>"));
		server.print(F("<RemoteTempOffset>"));
		server.print(Settings.RemoteTempOffset);
		server.print(F("</RemoteTempOffset>"));
		server.print(F("<SoilTempOffset>"));
		server.print(Settings.SoilTempOffset);
		server.print(F("</SoilTempOffset>"));
		server.print(F("</Offsets>"));

		server.print(F("<Net>"));
		server.print(F("<DHCP>"));
		server.print(Settings.DHCP);
		server.print(F("</DHCP>"));
		for (int i = 0; i < 4; i++)
		{
			server.printP(tag_start_sensor);
			server.print(Settings.IP[i]);
			server.printP(tag_end_sensor);
		}
		for (int i = 0; i < 4; i++)
		{
			server.printP(tag_start_sensor);
			server.print(Settings.Mask[i]);
			server.printP(tag_end_sensor);
		}
		for (int i = 0; i < 4; i++)
		{
			server.printP(tag_start_sensor);
			server.print(Settings.GW[i]);
			server.printP(tag_end_sensor);
		}
		for (int i = 0; i < 4; i++)
		{
			server.printP(tag_start_sensor);
			server.print(Settings.DNS[i]);
			server.printP(tag_end_sensor);
		}
		server.print(F("</Net>"));
		server.print(F("</Settings>"));
	};
	ledLight(1, 'g');
}
void settingsDataCmd(WebServer &server, WebServer::ConnectionType type, char *, bool)
{
	const static char remoteDStimeout[] PROGMEM = "remoteDStimeout";
	const static char action[] PROGMEM = "action";
	const static char thingspeak[] PROGMEM = "thingspeak";
	const static char tsaddr[] PROGMEM = "tsaddr";
	const static char ntpaddr[] PROGMEM = "ntpaddr";

	ledLight(1, 'y');
	server.httpSuccess();
	if (type == WebServer::POST)
	{
		while (server.readPOSTparam(cBuff1, nBuffLen1, cBuff2, nBuffLen2)) //buffer = name, buff = value
		{
			if (strcmp_P(cBuff1, remoteDStimeout) == 0)
			{
				Settings.RemoteDataSetTimeout = atoi(cBuff2);
			}
			if (strcmp_P(cBuff1, action) == 0)
			{
				Settings.InvalidDSAction = cBuff2[0] != '0';
			}
			if (strcmp_P(cBuff1, thingspeak) == 0)
			{
				Settings.TSenabled = cBuff2[0] != '0';
			}
			if (strcmp_P(cBuff1, tsaddr) == 0)
			{
				memcpy(&Settings.ThingSpeakAddress, cBuff2, nBuffLen2);
			}
			if (strcmp_P(cBuff1, ntpaddr) == 0)
			{
				memcpy(&Settings.NTPServer, cBuff2, nBuffLen2);
			}
		}

		P(messageSuccess) =
			"<!DOCTYPE html><html><head>"
			"<meta http-equiv=\"refresh\" content=\"2; url=system.htm\">"
			"<script language=\"javascript\">"
			"setTimeout(function(){ location.href = \"system.htm\" }, 2000);"
			"</script>"
			"<link rel=\"stylesheet\" type=\"text / css\" href=\"http://jet.php5.cz/thebox/css/general.css\">"
			"</head>"
			"<body>"
			"<div class=\"content\" style=\"color:green;font-weight:bold\">"
			"General settings successfully saved"
			"</div>"
			"</body>"
			"</html>";

		P(messageFail) =
			"<!DOCTYPE html><html><head>"
			"<meta http-equiv=\"refresh\" content=\"2; url=system.htm\">"
			"<script language=\"javascript\">"
			"setTimeout(function(){ location.href = \"system.htm\" }, 2000);"
			"</script>"
			"<link rel=\"stylesheet\" type=\"text / css\" href=\"http://jet.php5.cz/thebox/css/general.css\">"
			"</head>"
			"<body>"
			"<div class=\"content\" style=\"color:red;font-weight:bold\">"
			"Failed to save general settings!"
			"</div>"
			"</body>"
			"</html>";

		if (writeSDSettings()) server.printP(messageSuccess);
		else server.printP(messageFail);
		server.flushBuf();

		//enable/disable thingspeak
		if (Settings.TSenabled && !Alarm.active(updateTSAlarm))
		{
			Alarm.enable(updateTSAlarm);
		}
		if (!Settings.TSenabled && Alarm.active(updateTSAlarm))
		{
			Alarm.disable(updateTSAlarm);
		}

		ledLight(1, 'g');
	}
}
void offsetsDataCmd(WebServer &server, WebServer::ConnectionType type, char *, bool)
{
	const static char SysTempOffset[] PROGMEM = "SysTempOffset";
	const static char PressureOffset[] PROGMEM = "PressureOffset";
	const static char MainTempOffset[] PROGMEM = "MainTempOffset";
	const static char RemoteTempOffset[] PROGMEM = "RemoteTempOffset";
	const static char SoilTempOffset[] PROGMEM = "SoilTempOffset";

	ledLight(1, 'y');
	server.httpSuccess();
	if (type == WebServer::POST)
	{
		while (server.readPOSTparam(cBuff1, nBuffLen1, cBuff2, nBuffLen2)) //buffer = name, buff = value
		{
			if (strcmp_P(cBuff1, SysTempOffset) == 0)
			{
				Settings.SysTempOffset = atof(cBuff2);
			}
			if (strcmp_P(cBuff1, PressureOffset) == 0)
			{
				Settings.PressureOffset = atof(cBuff2);
			}
			if (strcmp_P(cBuff1, MainTempOffset) == 0)
			{
				Settings.MainTempOffset = atof(cBuff2);
			}
			if (strcmp_P(cBuff1, RemoteTempOffset) == 0)
			{
				Settings.RemoteTempOffset = atof(cBuff2);
			}
			if (strcmp_P(cBuff1, SoilTempOffset) == 0)
			{
				Settings.SoilTempOffset = atof(cBuff2);
			}
		}

		P(messageSuccess) =
			"<!DOCTYPE html><html><head>"
			"<meta http-equiv=\"refresh\" content=\"2; url=system.htm\">"
			"<script language=\"javascript\">"
			"setTimeout(function(){ location.href = \"system.htm\" }, 2000);"
			"</script>"
			"<link rel=\"stylesheet\" type=\"text / css\" href=\"http://jet.php5.cz/thebox/css/general.css\">"
			"</head>"
			"<body>"
			"<div class=\"content\" style=\"color:green;font-weight:bold\">"
			"Sensor offsets settings successfully saved"
			"</div>"
			"</body>"
			"</html>";

		P(messageFail) =
			"<!DOCTYPE html><html><head>"
			"<meta http-equiv=\"refresh\" content=\"2; url=system.htm\">"
			"<script language=\"javascript\">"
			"setTimeout(function(){ location.href = \"system.htm\" }, 2000);"
			"</script>"
			"<link rel=\"stylesheet\" type=\"text / css\" href=\"http://jet.php5.cz/thebox/css/general.css\">"
			"</head>"
			"<body>"
			"<div class=\"content\" style=\"color:red;font-weight:bold\">"
			"Failed to save sensor offsets settings!"
			"</div>"
			"</body>"
			"</html>";

		if (writeSDOffsets()) server.printP(messageSuccess);
		else server.printP(messageFail);
		server.flushBuf();
		ledLight(1, 'g');
	}
}
void offsetsDefaultCmd(WebServer &server, WebServer::ConnectionType type, char *, bool)
{
	ledLight(1, 'b');
	server.httpSuccess();
	P(message) =
		"<!DOCTYPE html><html><head>"
		"<meta http-equiv=\"refresh\" content=\"2; url=system.htm\">"
		"<script language=\"javascript\">"
		"setTimeout(function(){ location.href = \"system.htm\" }, 2000);"
		"</script>"
		"<link rel=\"stylesheet\" type=\"text / css\" href=\"http://jet.php5.cz/thebox/css/general.css\">"
		"</head>"
		"<body>"
		"<div class=\"content\" style=\"font-weight:bold\">"
		"Applying default settings, please wait..."
		"</div>"
		"</body>"
		"</html>";
	server.printP(message);
	server.flushBuf();
	SD.remove(Settings.OffsetsPath);
	Settings.setDefaultOffsetsSettings();
}
void settingsDefaultCmd(WebServer &server, WebServer::ConnectionType type, char *, bool)
{
	ledLight(1, 'b');
	server.httpSuccess();
	P(message) =
		"<!DOCTYPE html><html><head>"
		"<meta http-equiv=\"refresh\" content=\"2; url=system.htm\">"
		"<script language=\"javascript\">"
		"setTimeout(function(){ location.href = \"system.htm\" }, 2000);"
		"</script>"
		"<link rel=\"stylesheet\" type=\"text / css\" href=\"http://jet.php5.cz/thebox/css/general.css\">"
		"</head>"
		"<body>"
		"<div class=\"content\" style=\"font-weight:bold\">"
		"Applying default settings, please wait..."
		"</div>"
		"</body>"
		"</html>";
	server.printP(message);
	server.flushBuf();
	SD.remove(Settings.SettingsPath);
	Settings.setDefaultSystemSettings();
}
void rebootCmd(WebServer &server, WebServer::ConnectionType type, char *, bool)
{
	ledLight(1, 'c');
	server.httpSuccess();
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
	const static char DHCP[] PROGMEM = "DHCP";
	const static char IP[] PROGMEM = "IP";
	const static char Mask[] PROGMEM = "Mask";
	const static char GW[] PROGMEM = "GW";
	const static char DNS[] PROGMEM = "DNS";

	ledLight(1, 'y');
	server.httpSuccess();
	char value[4];
	byte counter[4] = { 0 };
	if (type == WebServer::POST)
	{
		while (server.readPOSTparam(cBuff1, 10, value, 4))
		{
			if (strcmp_P(cBuff1, DHCP) == 0)
			{
				Settings.DHCP = value[0] != '0';
			}
			if (strcmp_P(cBuff1, IP) == 0)
			{
				Settings.IP[counter[0]] = atoi(value);
				counter[0]++;
			}
			if (strcmp_P(cBuff1, Mask) == 0)
			{
				Settings.Mask[counter[1]] = atoi(value);
				counter[1]++;
			}
			if (strcmp_P(cBuff1, GW) == 0)
			{
				Settings.GW[counter[2]] = atoi(value);
				counter[2]++;
			}
			if (strcmp_P(cBuff1, DNS) == 0)
			{
				Settings.DNS[counter[3]] = atoi(value);
				counter[3]++;
			}
		}

		if (!Settings.DHCP && Alarm.active(dhcpAlarm))
		{
			Alarm.disable(dhcpAlarm);
		}
		if (Settings.DHCP && !Alarm.active(dhcpAlarm))
		{
			Alarm.enable(dhcpAlarm);
		}

		//build new ip to string for future redirect
		String sNewIP = "";
		for (int i = 0; i < 4; i++)
		{
			sNewIP += String(Settings.IP[i]);
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
	ledLight(1, 'g');
}
#pragma endregion webduino commands implementation

void setup()
{
	setupSerial();
	setupPins();
	setupSD();
	readSDSettings(Settings.RelaysPath);
	readSDSched();
	switchRelays();
	readSDSettings(Settings.SettingsPath);
	readSDSettings(Settings.OffsetsPath);
	if (!readSDSettings(Settings.EthernetPath)) Settings.DHCP = true; //reading ethernet settings must for some reason take place much earlier than ethernet.begin
	setupLCD();
	setupWire();
	setupDHT();
	setupBMP();
	setupRTC();
	setupRadio();
	setupEthernet();
	setupAlarms();

	//add webduino commands
	webserver.setDefaultCommand(&homePageCmd);						//get page
	webserver.addCommand("index.htm", homePageCmd);					//get page
	webserver.addCommand("sensors.xml", sensorsXMLCmd);				//get xml
	webserver.addCommand("relays.data", relayDataCmd);				//post data
	webserver.addCommand("graphs1.htm", graphs1PageCmd);			//get page
	webserver.addCommand("graphs2.htm", graphs2PageCmd);			//get page
	webserver.addCommand("sched.htm", schedPageCmd);				//get page
	webserver.addCommand("sched.xml", schedXMLCmd);					//get xml
	webserver.addCommand("sched.data", schedDataCmd);				//post data
	webserver.addCommand("sched.delete", schedDeleteCmd);			//delete sched data from sd
	webserver.addCommand("system.htm", systemPageCmd);				//get page
	webserver.addCommand("stats.xml", statsXMLCmd);					//get xml
	webserver.addCommand("settings.xml", settingsXMLCmd);			//get xml
	webserver.addCommand("settings.data", settingsDataCmd);			//post data
	webserver.addCommand("settings.default", settingsDefaultCmd);	//delete settings data from sd
	webserver.addCommand("offsets.data", offsetsDataCmd);			//post data
	webserver.addCommand("offsets.default", offsetsDefaultCmd);		//delete settings data from sd
	webserver.addCommand("network.data", networkDataCmd);			//post data
	webserver.addCommand("reboot", rebootCmd);						//reboot arduino
	webserver.begin();

	//datasets setup
	MainDS.APIkey = "FNHSHUE6A3XKP71C";
	MainDS.Size = 8;
	MainDS.Valid = true;
	MainDS.Timestamp = dtSysStart.unixtime();

	RemoteDS.APIkey = "OL1GVYUB2HFK7E2M";
	RemoteDS.Size = 8;
	RemoteDS.Valid = false;

	SystemDS.APIkey = "GNQST00GBW05EYGC";
	SystemDS.Size = 8;
	SystemDS.Valid = true;
	SystemDS.Timestamp = dtSysStart.unixtime();

	//Calibration process: attach a classic light bulb or heater and set the phase_shift constant so that the reported power factor is 1,
	//then connect a multimeter and set the calibration constant so that the reported voltage is same as on multimeter
	emon.voltage(VOLTAGE_PIN, 939, -0.24);  // Voltage: input pin, calibration, phase_shift
	//Calibration process: connect a known load and adjust the calibration constants so the reported wattage is the same as the load
	emon.current(CURRENT_LEFT_PIN, CURRENT_RIGHT_PIN, 18, 17.25); //Current: input pin, input pin, calibration, calibration

#if DEBUG
	Serial.println(F("Setup Done"));
#endif	
}

void loop()
{
	Alarm.delay(0);					//run alarms without any delay so the loop isn't slowed down
	webserver.processConnection();	//process webserver request as soon as possible		
	emon.calcVI(80, Vcc);			//measure power consumption in outlets (non-blocking)
}

