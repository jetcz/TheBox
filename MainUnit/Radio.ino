//************************************
// Method:   	 receiveData
// Description:  Receive data from radio module and fill our datasets
// Access:   	 public 
// Returns:  	 void
// Qualifier:	
//************************************
void receiveData() {
	const byte _byArrSize = 10; //how much float values are we sending

	uint8_t buf[_byArrSize * 4];
	uint8_t buflen = sizeof(buf);
	if (driver.recv(buf, &buflen)) // Non-blocking
	{
		ledLight(2, 'b');
		float temp[_byArrSize];
		memcpy(&temp, buf, buflen);

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

		sRemoteUptime = getUptimeString(TimeSpan(SystemDS.Data[7]));
		bReceivedRadioMsg = true;
	}
}

//************************************
// Method:   	 getFailedRadioMessages
// Description:  Calculate how many radio transmissions failed since main unit startup presuming that the remote unit is up and running and sending interval is 60s. This needs to be called every second.
// Access:   	 public 
// Returns:  	 void
// Qualifier:	
//************************************
void getFailedRadioMessages(){
	static int getFailedRadioMsgs;
	static unsigned int _lCnt = 0;
	_lCnt++;

	//this is to find out how many radio transmissions failed
	if (now() - RemoteDS.Timestamp.unixtime() > 63 && _lCnt > 63)
	{
		nFailedCntRadioTotal++;
		_lCnt = 0;
	}

	if (getFailedRadioMsgs == 0) getFailedRadioMsgs = Alarm.timerRepeat(1, getFailedRadioMessages);
}
