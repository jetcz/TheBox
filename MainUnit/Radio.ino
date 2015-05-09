
void receiveData() {
	uint8_t buf[RH_ASK_MAX_MESSAGE_LEN];
	uint8_t buflen = sizeof(buf);

	if (driver.recv(buf, &buflen)) // Non-blocking
	{
		ledLight(2, 'b');
		float temp[9];
		memcpy(&temp, buf, buflen);

		RemoteDS.Data[0] = temp[0];							 		//remoteTemperature
		RemoteDS.Data[1] = temp[1];									//remoteHumidity
		RemoteDS.Data[2] = temp[2];									//remoteHumidex
		RemoteDS.Data[3] = temp[3];								//remoteSoilTemperature
		RemoteDS.Data[4] = temp[4];									//remoteSoilHumidity
		RemoteDS.Data[5] = temp[5];									//remoteLight
		//these get filled in weather sketch
		//RemoteDS.Data[6]
		//RemoteDS.Data[7]
		RemoteDS.Data[8] = temp[6];									//total ticks (hidden field)
		RemoteDS.Timestamp = now();

		SystemDS.Data[6] = temp[7];									//vcc
		SystemDS.Data[7] = temp[8];									//uptime

		sRemoteUptime = getUptimeString(TimeSpan(SystemDS.Data[7]));
		bReceivedRadioMsg = true;
	}
}

//this needs to be called once per second
void getFailedRadioMessages(){
	static unsigned long _lCnt = 0;
	static bool _bRepeat = false;
	_lCnt++;

	//this is to find out how many radio transmissions failed
	if (now() - RemoteDS.Timestamp.unixtime() > 63 && _lCnt > 63)
	{
		nFailedCntRadioTotal++;
		_lCnt = 0;
	}

	if (!_bRepeat)
	{
		_bRepeat = true;
		Alarm.timerRepeat(1, getFailedRadioMessages);
	}
}
