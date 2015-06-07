struct SystemSettings
{
	SystemSettings() {
		setDefaultSystemSettings();
		setDefaultOffsetsSettings();
	}

	void setDefaultSystemSettings(){
		strncpy(ThingSpeakAddress, "api.thingspeak.com", 30);
		UpdateThingSpeakInterval = 20;
		RemoteDataSetTimeout = 180;
		RestartEthernetThreshold = 10;
		RestartArduinoThreshold = 42;
		strncpy(NTPServer, "tik.cesnet.cz", 30);
		LcdMsgTimeout = 4;
		LightIntensity[0] = 6; //R
		LightIntensity[1] = 1; //G
		LightIntensity[2] = 2; //B
		UpdateSensorsInterval = 10;
		TSenabled = true;
		InvalidDSAction = false;
		UpdateRainPerDayInterval = 600; //10 min interval
		RadioMsgInterval = 20;
	}

	void setDefaultOffsetsSettings(){
		SysTempOffset = -0.2;
		PressureOffset = 24;
		MainTempOffset = -1.2;
		RemoteTempOffset = -0.5;
		SoilTempOffset = -0.2;
	}
	/* ThingSpeak settings */
	char ThingSpeakAddress[30];
	//const char cThingSpeakAddress[] = "184.106.153.149";
	byte UpdateThingSpeakInterval;
	int RemoteDataSetTimeout;		//for how long is dataset valid and send to thingspeak (sec)
	byte RestartEthernetThreshold;	//if thingspeak update fails x times -> ethernet shield reset
	byte RestartArduinoThreshold;	//if thingspeak update fails x times -> arduino reset

	/* ntp server */
	char NTPServer[30];
	/* lcd settings */
	byte LcdMsgTimeout;

	/* led settings R, G, B*/
	byte LightIntensity[3]; //0-255, these led are very bright so we need low values

	/* sensor polling settings */
	byte UpdateSensorsInterval;

	int UpdateRainPerDayInterval;

	byte RadioMsgInterval;

	/* sensor offsets */
	float SysTempOffset;
	float PressureOffset;
	float MainTempOffset;
	float RemoteTempOffset;
	float SoilTempOffset;

	//ethernet settings
	byte MAC[6] = { 0xB0, 0x0B, 0x5B, 0x00, 0xB5, 0x00 };
	byte IP[4] = { 0 };
	byte GW[4] = { 0 };
	byte Mask[4] = { 0 };
	byte DNS[4] = { 0 };
	bool DHCP = false;

	bool TSenabled;						//enable disable thingspeak
	bool InvalidDSAction;				//what to do with relay if dataset is invalid true=turn off relay; false=do nothing

	//relay modes
	byte RelayMode[4] = { 0 };
	/* ini files settings
	relay config must be in following format:
	modes = 0, 1, 1, 2 where 0 is off, 1 is on and 2 is auto */

	char* EthernetPath = "/settings/ethernet.ini";
	char* RelaysPath = "/settings/relays.ini";
	char* SettingsPath = "/settings/settings.ini";
	char* OffsetsPath = "/settings/offsets.ini";

}; typedef struct SystemSettings SystemSettings;

struct DataSet
{
	float Data[8] = { 0 };
	byte Size;
	bool Valid;
	String APIkey;
	DateTime Timestamp;
}; typedef struct DataSet DataSet;

struct RelayScheduler
{
	byte CurrentInterval = 0;  //this is pointer to arrays enabled, time, value
	byte Variable = 0;	//target variable
	bool Enabled[5] = { false }; //interval enabled<
	byte Time[5][2] = { { 0 }, { 0 } }; //5x time of day - consists of two values h:m
	float Value[5][2] = { { 0 }, { 0 } }; //5x pair of valuemin and valuemax

	void setDefault() {
		CurrentInterval = 0;
		Variable = 0;
		for (int i = 0; i < 5; i++)
		{
			Enabled[i] = false;
			Time[i][0] = 0;
			Time[i][1] = 0;
			Value[i][0] = 0;
			Value[i][1] = 0;
		}
	}
}; typedef struct RelayScheduler RelayScheduler;

/*
				MainDS			RemoteDS				SystemDS
TS size			5				8						8
				mainTemperature	remoteTemperature		sysTemperature
				mainHumidity	remoteHumidity			sysUptime
				mainHumidex		remoteHumidex			relay1
				mainPir			remoteSoilTemperature	relay2
				pressure		remoteSoilHumidity		relay3
								remoteLight				relay4
								rainHour				remoteVoltage
								rainDay					remoteUptime
not sent to TS					rainTicks				remoteFreeRam
not sent to TS											mainFreeRam

*/


