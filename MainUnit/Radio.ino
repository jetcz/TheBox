//************************************
// Method:   	 receiveData
// Description:  Receive data from radio module and fill our datasets
// Access:   	 public 
// Returns:  	 void
// Qualifier:	
//************************************

void receiveData() {
	const byte _byArrSize = 11; //how much float values are we sending
	byte _byBuff[_byArrSize * 4];
	byte _byBuffLen = sizeof(_byBuff);

	if (nrf24.recv(_byBuff, &_byBuffLen)) // Non-blocking
	{
		ledLight(2, 'b');
		float temp[_byArrSize];
		memcpy(&temp, _byBuff, _byBuffLen);

		RemoteDS.Data[0] = (temp[0] == -255) ? temp[0] : temp[0] + Settings.RemoteTempOffset;		//remoteTemperature
		RemoteDS.Data[1] = temp[1];																	//remoteHumidity
		RemoteDS.Data[2] = temp[2];																	//remoteHumidex
		RemoteDS.Data[3] = (temp[3] == -255) ? temp[3] : temp[3] + Settings.SoilTempOffset;			//remoteSoilTemperature
		RemoteDS.Data[4] = temp[4];																	//remoteSoilHumidity
		RemoteDS.Data[5] = temp[5];																	//remoteLight
		//these get filled in weather sketch
		//RemoteDS.Data[6]
		//RemoteDS.Data[7]
		fRainTicks = temp[6];										//total ticks 
		RemoteDS.Timestamp = now();

		SystemDS.Data[6] = temp[7];									//vcc
		SystemDS.Data[7] = temp[8];									//uptime

		nRemoteFreeRam = temp[9];									//remote free ram

		Settings.RadioMsgInterval = temp[10];						//radio messages interval

		sRemoteUptime = getUptimeString(TimeSpan(SystemDS.Data[7]));
		bReceivedRadioMsg = true;
	}
}


//************************************
// Method:   	 getFailedRadioMessages
// Description:  Calculate how many radio transmissions failed since main unit startup presuming that the remote unit. This needs to be called every radio msg interval
// Access:   	 public 
// Returns:  	 void
// Qualifier:	
//************************************
void getFailedRadioMessages(){
	static bool bSetRadioMsgInterval = false;
	if (now() - RemoteDS.Timestamp.unixtime() > Settings.RadioMsgInterval + 2) nFailedCntRadioTotal++;
	if (bReceivedRadioMsg && !bSetRadioMsgInterval) Alarm.write(failedRadioMessagesAlarm, Settings.RadioMsgInterval);
}
