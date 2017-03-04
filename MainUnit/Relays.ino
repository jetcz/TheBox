/// <summary>
/// Switch relays according to current values in Settings
/// </summary>
void switchRelays()
{
#if DEBUG
	Serial.print(F("Setting relays:"));
#endif
	for (int i = 0; i < 4; i++)
	{

		switch (Settings.RelayMode[i])
		{
		case 0:
#if DEBUG
			Serial.print(F(" OFF"));
#endif
			digitalWrite(RELAY_PIN[i], HIGH); //HIGH is inactive

			break;
		case 1:
#if DEBUG
			Serial.print(F(" ON"));
#endif
			digitalWrite(RELAY_PIN[i], LOW); //LOW is active
			break;
		default:
#if DEBUG
			Serial.print(F(" AUTO"));
#endif
			serviceSchedulers(now(), i);
			break;
		}
	}

#if DEBUG
	Serial.println();
#endif

	if (bSetupDone) Alarm.enable(ethShieldFreezeDetectAlarm);

}

/// <summary>
/// Return current relay state true/false (must be inverted because HIGH = off)
/// </summary>
/// <param name="relay">relay</param>
/// <returns>on/off</returns>
bool getRelayState(int relay)
{
	return !digitalRead(RELAY_PIN[relay]);
}

/// <summary>
/// This method is turning relays on or off according to scheduler settings. Must be called every second or so.
/// </summary>
/// <param name="t">Current date time</param>
/// <param name="relay">relay</param>
/// <returns>If any relay switch happened</returns>
bool serviceSchedulers(DateTime t, int relay)
{
	static bool bLastState[4] = { 0 }; //previous relay states
	bool bSwitched = false; //if this variable is true in the end of this method, it means that at least one relay switch happened and we should check ethernet shield if it is still running

	if (Sched[relay].Variable != 0) //is not pir
	{
		//set current interval where we are at this time of day
		unsigned long _lCurrSec = long(t.hour()) * 60 * 60 + long(t.minute()) * 60 + long(t.second());
		bool _bSet = false; //in case we use all 5 intervals we need to have this aux variable
		for (int i = 0; i < 5; i++)
		{
			if (Sched[relay].Enabled[i])
			{
				unsigned long lSchedSec = long(Sched[relay].Time[i][0]) * 60 * 60 + long(Sched[relay].Time[i][1]) * 60;

				if ((_lCurrSec >= lSchedSec) || (i == 4 && !_bSet))
				{
					Sched[relay].CurrentInterval = i;
					_bSet = true;
				}
			}
		}

		//control relays (all the magic goes here)
		if ((Sched[relay].Variable > 3 && RemoteDS.isValid) || (Sched[relay].Variable <= 3)) //if targer var is from remote unit, remote ds must be valid to be processed
		{
			if (Sched[relay].Value[Sched[relay].CurrentInterval][0] < Sched[relay].Value[Sched[relay].CurrentInterval][1]) //normal mode (heating...)
			{
				if (*TargetVarPtr[Sched[relay].Variable] != Settings.InvalidValue) //do something only if current value is valid
				{
					//switch relays according to target var
					if (*TargetVarPtr[Sched[relay].Variable] <= Sched[relay].Value[Sched[relay].CurrentInterval][0])
					{
						bSwitched = bLastState[relay] != true;
						digitalWrite(RELAY_PIN[relay], LOW); //LOW is active
						bLastState[relay] = true;
					}

					if (*TargetVarPtr[Sched[relay].Variable] >= Sched[relay].Value[Sched[relay].CurrentInterval][1])
					{
						bSwitched = bLastState[relay] != false;
						digitalWrite(RELAY_PIN[relay], HIGH);
						bLastState[relay] = false;
					}
				}
			}
			else
			{ //reversed mode (cooling...)			
				if (*TargetVarPtr[Sched[relay].Variable] != Settings.InvalidValue)
				{
					//switch relays according to target var
					if (*TargetVarPtr[Sched[relay].Variable] >= Sched[relay].Value[Sched[relay].CurrentInterval][0])
					{
						bSwitched = bLastState[relay] != true;
						digitalWrite(RELAY_PIN[relay], LOW); //LOW is active
						bLastState[relay] = true;
					}

					if (*TargetVarPtr[Sched[relay].Variable] <= Sched[relay].Value[Sched[relay].CurrentInterval][1])
					{
						bSwitched = bLastState[relay] != false;
						digitalWrite(RELAY_PIN[relay], HIGH);
						bLastState[relay] = false;
					}
				}
			}
		}
		else if (Settings.InvalidDSAction)//remote ds is not valid and we have in settings that we need to turn opff relay when ds invalid
		{
			bSwitched = bLastState[relay] != false;
			digitalWrite(RELAY_PIN[relay], HIGH); //turn off relay
			bLastState[relay] = false;
		}
		else //when we do nothing when ds invalid
		{
			// do nothing
		}
	}
	else
	{ //target variable is pir
		if (getMainPir())
		{
			bSwitched = bLastState[relay] != true;
			digitalWrite(RELAY_PIN[relay], LOW);
			bLastState[relay] = true;
		}
		else
		{
			bSwitched = bLastState[relay] != false;
			digitalWrite(RELAY_PIN[relay], HIGH);
			bLastState[relay] = false;
		}
	}
	return bSwitched;
}
