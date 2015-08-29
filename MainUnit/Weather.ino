/// <summary>
/// Calculates weather forecast from relative changes in atmospheric pressure.
/// This needs to be called every 60s.
/// Algorithm found here http://www.freescale.com/files/sensors/doc/app_note/AN3914.pdf
/// Code ripped from here http://forum.micasaverde.com/index.php?topic=23394.0 and modified to use runnig average library to save ram
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


//************************************
// Method:   	 getRainPerHour
// Description:  Calculate running sum of rain fall from the last hour. This needs to be called every 60s.
// Access:   	 public 
// Returns:  	 void
// Qualifier:	
//************************************
void getRainPerHour() {
	static QueueArray <byte> q;
	static unsigned int _nLastTickCnt = nRainTicks;
	static unsigned int _nTicksPerLastHour = 0;
	if (nRainTicks == 0) _nLastTickCnt = 0; //in case we restart the remote unit and main unit keeps runnig

	unsigned int _nTicks = nRainTicks - _nLastTickCnt;
	_nTicksPerLastHour += _nTicks;
	q.push(byte(_nTicks));
	if (q.count() > 60) _nTicksPerLastHour -= q.pop();
	_nLastTickCnt = nRainTicks;
	RemoteDS.Data[6] = float(_nTicksPerLastHour) * 0.3;
};


//************************************
// Method:   	 getRainPerDay
// Description:  Calculate running sum of rain fall from the las 24h. This is called by default every 10 minutes. The often it is call, the bigger FIFO it needs, so be careful not to waste all your RAM.
// Access:   	 public 
// Returns:  	 void
// Qualifier:	
//************************************
void getRainPerDay() {
	static QueueArray <byte> q;
	static unsigned int _nLastTickCnt = nRainTicks;
	static unsigned int _nTicksPerLastDay = 0;
	if (nRainTicks == 0) _nLastTickCnt = 0;

	unsigned int _nTicks = nRainTicks - _nLastTickCnt;
	_nTicksPerLastDay += _nTicks;
	q.push(byte(_nTicks));
	if (q.count() > 86400 / Settings.UpdateRainPerDayInterval) _nTicksPerLastDay -= q.pop(); //if the interval is set to 10 min, fifo is 144 bytes long
	_nLastTickCnt = nRainTicks;
	RemoteDS.Data[7] = float(_nTicksPerLastDay) * 0.3;
};


