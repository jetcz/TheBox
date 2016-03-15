/// <summary>
/// Calculates weather forecast from relative changes in atmospheric pressure. This needs to be
/// called every 60s. Algorithm found here
/// http://www.freescale.com/files/sensors/doc/app_note/AN3914.pdf Code ripped from here
/// http://forum.micasaverde.com/index.php?topic=23394.0 and modified to use runnig average library
/// to save ram
/// </summary>
void weatherForecast() {
	static byte _nMinuteCnt = 0;
	static bool _bFirstRound = true;
	static float _fPressureAvg[7];
	static float _fDP_dt;

	if (_nMinuteCnt > 180)
		_nMinuteCnt = 6;
	_nMinuteCnt++;

	if (_nMinuteCnt == 5) {
		// Avg pressure in first 5 min
		_fPressureAvg[0] = MainDS.Data[4];
	}
	else if (_nMinuteCnt == 35) {
		// Avg pressure in 30 min
		_fPressureAvg[1] = MainDS.Data[4];
		float change = (_fPressureAvg[1] - _fPressureAvg[0]);
		if (_bFirstRound) // first time initial 3 hour
			_fDP_dt = ((65.0 / 1023.0) * 2 * change); // note this is for t = 0.5hour
		else
			_fDP_dt = (((65.0 / 1023.0) * change) / 1.5); // divide by 1.5 as this is the difference in time from 0 value.
	}
	else if (_nMinuteCnt == 60) {
		// Avg pressure at end of the hour
		_fPressureAvg[2] = MainDS.Data[4];
		float change = (_fPressureAvg[2] - _fPressureAvg[0]);
		if (_bFirstRound) //first time initial 3 hour
			_fDP_dt = ((65.0 / 1023.0) * change); //note this is for t = 1 hour
		else
			_fDP_dt = (((65.0 / 1023.0) * change) / 2); //divide by 2 as this is the difference in time from 0 value
	}
	else if (_nMinuteCnt == 95) {
		// Avg pressure at end of the hour
		_fPressureAvg[3] = MainDS.Data[4];
		float change = (_fPressureAvg[3] - _fPressureAvg[0]);
		if (_bFirstRound) // first time initial 3 hour
			_fDP_dt = (((65.0 / 1023.0) * change) / 1.5); // note this is for t = 1.5 hour
		else
			_fDP_dt = (((65.0 / 1023.0) * change) / 2.5); // divide by 2.5 as this is the difference in time from 0 value
	}
	else if (_nMinuteCnt == 120) {
		// Avg pressure at end of the hour
		_fPressureAvg[4] = MainDS.Data[4];
		float change = (_fPressureAvg[4] - _fPressureAvg[0]);
		if (_bFirstRound) // first time initial 3 hour
			_fDP_dt = (((65.0 / 1023.0) * change) / 2); // note this is for t = 2 hour
		else
			_fDP_dt = (((65.0 / 1023.0) * change) / 3); // divide by 3 as this is the difference in time from 0 value
	}
	else if (_nMinuteCnt == 155) {
		// Avg pressure at end of the hour
		_fPressureAvg[5] = MainDS.Data[4];
		float change = (_fPressureAvg[5] - _fPressureAvg[0]);
		if (_bFirstRound) // first time initial 3 hour
			_fDP_dt = (((65.0 / 1023.0) * change) / 2.5); // note this is for t = 2.5 hour
		else
			_fDP_dt = (((65.0 / 1023.0) * change) / 3.5); // divide by 3.5 as this is the difference in time from 0 value
	}
	else if (_nMinuteCnt == 180) {
		// Avg pressure at end of the hour
		_fPressureAvg[6] = MainDS.Data[4];
		float change = (_fPressureAvg[6] - _fPressureAvg[0]);
		if (_bFirstRound) // first time initial 3 hour
			_fDP_dt = (((65.0 / 1023.0) * change) / 3); // note this is for t = 3 hour
		else
			_fDP_dt = (((65.0 / 1023.0) * change) / 4); // divide by 4 as this is the difference in time from 0 value
		_fPressureAvg[0] = _fPressureAvg[5]; // Equating the pressure at 0 to the pressure at 2 hour after 3 hours have past.
		_bFirstRound = false; // flag to let you know that this is on the past 3 hour mark. Initialized to 0 outside main loop.
	}

	if (_nMinuteCnt < 35 && _bFirstRound) //if time is less than 35 min on the first 3 hour interval.
		byForecast = 5; // Unknown, more time needed
	else if (_fDP_dt < (-0.25))
		byForecast = 4; // Quickly falling LP, Thunderstorm, not stable
	else if (_fDP_dt > 0.25)
		byForecast = 3; // Quickly rising HP, not stable weather
	else if ((_fDP_dt >(-0.25)) && (_fDP_dt < (-0.05)))
		byForecast = 2; // Slowly falling Low Pressure System, stable rainy weather
	else if ((_fDP_dt > 0.05) && (_fDP_dt < 0.25))
		byForecast = 1; // Slowly rising HP stable good weather
	else if ((_fDP_dt > (-0.05)) && (_fDP_dt < 0.05))
		byForecast = 0; // Stable weather
	else
		byForecast = 5; // Unknown
}

/// <summary>
/// Calculate running sum of rain fall from the last hour. This needs to be called every 60s.
/// </summary>
void getRainPerHour() {
	static unsigned int _nLastTickCnt = nRainTicks;

	if (nRainTicks < _nLastTickCnt) _nLastTickCnt = nRainTicks; //in case we restart the remote unit and main unit keeps runnig

	unsigned int _nTicks = nRainTicks - _nLastTickCnt;
	nRainTicksSum[0] += _nTicks;
	Rain[0].push(byte(_nTicks));
	if (Rain[0].count() > Settings.UpdateRainInterval[0]) nRainTicksSum[0] -= Rain[0].pop();
	_nLastTickCnt = nRainTicks;
	RemoteDS.Data[6] = float(nRainTicksSum[0]) * 0.3;
	writeSDRain(0);
};


/// <summary>
/// Calculate running sum of rain fall from the las 24h. This is called by default every 10 minutes.
/// The often it is call, the bigger FIFO it needs, so be careful not to waste all your RAM.
/// </summary>
void getRainPerDay() {
	static unsigned int _nLastTickCnt = nRainTicks;

	if (nRainTicks < _nLastTickCnt) _nLastTickCnt = nRainTicks;

	unsigned int _nTicks = nRainTicks - _nLastTickCnt;
	nRainTicksSum[1] += _nTicks;
	Rain[1].push(byte(_nTicks));
	if (Rain[1].count() > 86400 / Settings.UpdateRainInterval[1]) nRainTicksSum[1] -= Rain[1].pop(); //if the interval is set to 10 min, fifo is 144 bytes long
	_nLastTickCnt = nRainTicks;
	RemoteDS.Data[7] = float(nRainTicksSum[1]) * 0.3;
	writeSDRain(1);
};

/// <summary>
/// Writes rainfall data fifo to sdcard so it can be restored after reboot
/// </summary>
/// <param name="nArrPtr">hourly=0/daily=1</param>
/// <returns>sd card writing success</returns>
bool writeSDRain(int nArrPtr) {

	SD.remove(Settings.Rain[nArrPtr]);
	file = SD.open(Settings.Rain[nArrPtr], FILE_WRITE);

	if (!file)
	{
		return false;
	}
	else {
		file.println(now());
		int j = Rain[nArrPtr].count();
		for (int i = j - 1; i >= 0; i--) //must go from end to start, because restoring is doing push - first line in file will appear last in fifo, see readSDRain
		{
			file.print(Rain[nArrPtr].contents[i]);
			if (i > 0)
			{
				file.println();
			}
		}
		file.close();
		return true;
	}
}
/// <summary>
/// Reads rainfall data from sdcard and restores sum of rain tips and fifo holding the data
/// </summary>
/// <param name="nArrPtr">hourly=0/daily=1</param>
/// <returns>sd card writing success</returns>
bool readSDRain(int nArrPtr) {
	file = SD.open(Settings.Rain[nArrPtr]);
	if (!file) return false;
	else {
		int _nCnt = 0;
		int _nToBeSkipped = 0;

		while (file.available()) {

			if (_nCnt == 0) //first line cotains timestamp
			{
				time_t _lFileAge = now() - (time_t)file.parseInt();
				_nToBeSkipped = _lFileAge / Settings.UpdateRainInterval[nArrPtr];
			}

			if (_nCnt > _nToBeSkipped) //rain data following
			{
				byte _byVal = file.parseInt();
				Rain[nArrPtr].push(_byVal);
				nRainTicksSum[nArrPtr] += _byVal;
			}
			else file.parseInt();
			_nCnt++;
		}

		//fill the rest of the fifo with zeroes (presuming that there was no rain during the time we were turned off, this is very naive but hey...)
		//main purpose of this feature is to keep data over reboot, not over longer period of inactivity - we cant possibly know how much rain has fallen over the period of inactivity
		int _nFifoLen = (nArrPtr == 0) ? 60 : 86400 / Settings.UpdateRainInterval[1];
		while (Rain[nArrPtr].count() < _nFifoLen)
		{
			Rain[nArrPtr].push((byte)0);
		}

		// close the file:
		file.close();

		RemoteDS.Data[6 + nArrPtr] = float(nRainTicksSum[nArrPtr]) * 0.3;

		return true;
	}
}


