//************************************
// Method:   	 switchRelays
// Description:  Switch relays according to current values in Settings
// Access:   	 public 
// Returns:  	 void
// Qualifier:	
//************************************
void switchRelays() {
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
}

//************************************
// Method:   	 getRelayState
// Description:  Return current relay state true/false (must be inverted because HIGH = off)
// Access:   	 public 
// Returns:  	 bool
// Qualifier:	
// Parameter:	 int relay
//************************************
bool getRelayState(int relay){
	return !digitalRead(RELAY_PIN[relay]);
}

//************************************
// Method:   	 serviceSchedulers
// Description:  This method is turning relays on or off according to scheduler settings. Must be called every second or so.
// Access:   	 public 
// Returns:  	 void
// Qualifier:	
// Parameter:	 int relay
//************************************
void serviceSchedulers(DateTime t, int relay){

	if (Sched[relay].Variable != 0) //is not pir
	{
		//set current interval
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
				if (*TargetVarPtr[Sched[relay].Variable] != -255) //do something only if current value is valid
				{
					//switch relays according to target var
					if (*TargetVarPtr[Sched[relay].Variable] <= Sched[relay].Value[Sched[relay].CurrentInterval][0]) {
						digitalWrite(RELAY_PIN[relay], LOW); //LOW is active
					}

					if (*TargetVarPtr[Sched[relay].Variable] >= Sched[relay].Value[Sched[relay].CurrentInterval][1]) {
						digitalWrite(RELAY_PIN[relay], HIGH);
					}
				}
			}
			else { //reversed mode (cooling...)			
				if (*TargetVarPtr[Sched[relay].Variable] != -255)
				{
					//switch relays according to target var
					if (*TargetVarPtr[Sched[relay].Variable] >= Sched[relay].Value[Sched[relay].CurrentInterval][0]) {
						digitalWrite(RELAY_PIN[relay], LOW); //LOW is active
					}

					if (*TargetVarPtr[Sched[relay].Variable] <= Sched[relay].Value[Sched[relay].CurrentInterval][1]) {
						digitalWrite(RELAY_PIN[relay], HIGH);
					}
				}
			}
		}
		else if (Settings.InvalidDSAction)//remote ds is not valid and we have in settings that we need to turn opff relay when ds invalid
		{
			digitalWrite(RELAY_PIN[relay], HIGH); //turn off relay
		} 
		else //when we do nothing when ds invalid
		{
			// do nothing
		}
	}
	else  { //target variable is pir
		if (getMainPir()) digitalWrite(RELAY_PIN[relay], LOW);
		else digitalWrite(RELAY_PIN[relay], HIGH);
	}
}
