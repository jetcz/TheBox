/*
				MainDS			RemoteDS				SystemDS
				TS size			8				8						8
				mainTemperature	remoteTemperature		sysTemperature
				mainHumidity	remoteHumidity			sysUptime
				mainHumidex		remoteHumidex			relay1
				mainPir			remoteSoilTemperature	relay2
				pressure		remoteSoilHumidity		relay3
				leftSocektPWR	remoteLight				relay4
				rightSocektPWR	rainHour				remoteVoltage
				mainsVoltage	rainDay					remoteUptime
not sent to TS					rainTicks				remoteFreeRam
not sent to TS											mainFreeRam

*/

/// <summary>
/// Structure holding data from sensors sent to thingspeak
/// </summary>
class DataSet
{
public:
	/// <summary>
	/// constructor
	/// </summary>
	DataSet() {
		TimeStamp = 0;
		Size = 8;
		isValid = true;
		ThingSpeakStr.reserve(100);
	}

	//properties
	float Data[8];
	byte Size; //size is used to determine length to the array. It is changed manulay in the code. Refer to alarms.ino
	bool isValid;
	String APIkey;
	DateTime TimeStamp;
	String ThingSpeakStr;

	//commonly used pointers
	float *Temperature = &Data[0];
	float *Humidity = &Data[1]; //not for system DS
	float *Humidex = &Data[2]; //not for system DS

	/// <summary>
	/// Create ThingSpeak string
	/// </summary>
	void GetTSString() {
		ThingSpeakStr = "";
		for (int i = 0; i < this->Size; i++)
		{
			if (this->Data[i] > -100) //in case we get some broken values which are -255
			{
				ThingSpeakStr += String(i + 1) + "=" + String(this->Data[i], 2);
				if (i < this->Size - 1) ThingSpeakStr += "&";
			}
		}
	}
}; typedef class DataSet DataSet;