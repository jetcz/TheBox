void weatherForecast() {
	static int _nMinuteCnt = 0;
	static bool _bFirstRound = true;
	static float _fPressureAvg[7];
	static float _fDP_dt;
	// Algorithm found here
	// http://www.freescale.com/files/sensors/doc/app_note/AN3914.pdf
	//code ripped from here http://forum.micasaverde.com/index.php?topic=23394.0 and modified to use runnig median library to save ram

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
	else if ((_fDP_dt > (-0.25)) && (_fDP_dt < (-0.05)))
		byForecast = 2; // Slowly falling Low Pressure System, stable rainy weather
	else if ((_fDP_dt > 0.05) && (_fDP_dt < 0.25))
		byForecast = 1; // Slowly rising HP stable good weather
	else if ((_fDP_dt >(-0.05)) && (_fDP_dt < 0.05))
		byForecast = 0; // Stable weather
	else
		byForecast = 5; // Unknown
}

void getRainPerHour() {
	if (!bReceivedRadioMsg) return;
	static QueueArray <byte> q;
	static float _fLastTickCnt = RemoteDS.Data[8];
	static float _fTicksPerLastHour = 0;

	float _fTicks = RemoteDS.Data[8] - _fLastTickCnt;
	_fTicksPerLastHour += _fTicks;
	q.push(byte(_fTicks));
	if (q.count() > 60) _fTicksPerLastHour -= q.pop();
	_fLastTickCnt = RemoteDS.Data[8];
	RemoteDS.Data[6] = _fTicksPerLastHour * 0.3;
};

void getRainPerDay() {
	if (!bReceivedRadioMsg) return;
	Alarm.disable(getInitialTipCntAlarm);
	static QueueArray <int> q;
	static float _fLastTickCnt = RemoteDS.Data[8];
	static float _fTicksPerLastDay = 0;

	float _fTicks = RemoteDS.Data[8] - _fLastTickCnt;
	_fTicksPerLastDay += _fTicks;
	q.push(int(_fTicks));
	if (q.count() > 86400 / Settings.UpdateRainPerDayInterval) _fTicksPerLastDay -= q.pop(); //if the interval is set to 10 min, fifo is 144 bytes long
	_fLastTickCnt = RemoteDS.Data[8];
	RemoteDS.Data[7] = _fTicksPerLastDay * 0.3;
};


