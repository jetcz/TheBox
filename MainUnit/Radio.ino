
void receiveData() {
	uint8_t buf[RH_ASK_MAX_MESSAGE_LEN];
	uint8_t buflen = sizeof(buf);

	if (driver.recv(buf, &buflen)) // Non-blocking
	{
		ledLight(2, 'b');
		float temp[9];
		memcpy(&temp, buf, buflen);

		for (int i = 0; i < 8; i++)
		{
			RemoteDS.Data[i] = temp[i];
		}

		SystemDS.Data[6] = temp[7];
		SystemDS.Data[7] = temp[8];
		RemoteDS.Timestamp = now();
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
		failedMsgsAlarm = Alarm.timerRepeat(1, getFailedRadioMessages);
	}
}
