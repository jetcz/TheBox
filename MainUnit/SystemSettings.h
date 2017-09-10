enum RequestMethod
{
	GET,
	POST,
	PUT
};

/// <summary>
/// Structure for holding all the settings
/// </summary>
struct SystemSettings
{
	/// <summary>
	/// Constructor
	/// </summary>
	SystemSettings()
	{
		setDefaultSystemSettings();
		setDefaultOffsetsSettings();
	}

	/// <summary>
	/// Set default values for the settings
	/// </summary>
	void setDefaultSystemSettings()
	{
		strncpy(ThingSpeakAddress, "api.thingspeak.com", 30);
		UpdateThingSpeakInterval = 20;
		RemoteDataSetTimeout = 180;
		RestartEthernetThreshold = 9;
		RestartWifiThreshold = 61;
		RestartArduinoThreshold = 301;
		strncpy(NTPServer, "tik.cesnet.cz", 30);
		LcdMsgTimeout = 4;
		LightIntensity[0] = 6; //R
		LightIntensity[1] = 1; //G
		LightIntensity[2] = 2; //B
		UpdateSensorsInterval = 10;
		UpdatePWRSensorsInterval = 1;
		TSenabled = true;
		InvalidDSAction = false;
		UpdateRainInterval[0] = 60; //1 min interval - for hour cumulative rainfall
		UpdateRainInterval[1] = 600; //10 min interval - for day cumulative rainfall
		RadioMsgInterval = 20;
		InvalidValue = -255;
		NRFChannel = 0;
		TSCnnTimeout = 400;
		Method = RequestMethod(POST);
	}

	/// <summary>
	/// Set default offsets values
	/// </summary>
	void setDefaultOffsetsSettings()
	{
		SysTempOffset = -0.2;
		PressureOffset = 29;
		MainTempOffset = -1.2;
		RemoteTempOffset = -0.5;
		SoilTempOffset = -0.2;
	}
	/* ThingSpeak settings */
	char ThingSpeakAddress[30];
	//const char cThingSpeakAddress[] = "184.106.153.149";
	byte UpdateThingSpeakInterval;
	unsigned int RemoteDataSetTimeout;		//for how long is dataset valid and send to thingspeak (sec)
	byte RestartEthernetThreshold;			//if thingspeak update fails x times -> ethernet shield reset
	byte RestartWifiThreshold;				//if thingspeak update fails x times -> wifi reset
	byte RestartArduinoThreshold;			//if thingspeak update fails x times -> arduino reset

	/* ntp server */
	char NTPServer[30];
	/* lcd settings */
	byte LcdMsgTimeout;

	/* led settings R, G, B*/
	byte LightIntensity[3]; //0-255, these led are very bright so we need low values

	/* sensor polling settings */
	byte UpdateSensorsInterval;
	byte UpdatePWRSensorsInterval;

	/* Other settings*/
	int UpdateRainInterval[2];
	byte RadioMsgInterval;
	int InvalidValue;
	byte NRFChannel;
	int TSCnnTimeout;
	RequestMethod Method;


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
	char* Rain[2] = { "/rph.dat","/rpd.dat" };
}; typedef struct SystemSettings SystemSettings;
