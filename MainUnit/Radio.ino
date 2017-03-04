/// <summary>
/// Receive data from radio module and fill our datasets and related variables
/// </summary>
inline void receiveData()
{
	static unsigned int nInitialFailedMsgCnt = 0;
	while (radio.available())
	{ //receiving data		

		Payload p; //my custom struct to hold radio data.
		radio.read(&p, sizeof(p));
		if (p.Uptime == 0 && p.Vcc == 0 && p.FreeRam == 0)
		{//in case all is zero, we got some bogus data from interference
			RemoteDS.isValid = false;
			return;
		}
		else
		{
			RemoteDS.isValid = true;
		}

		ledLight(2, 'b');
#if DEBUG
		Serial.println();
		Serial.println(F("Received radio message"));
#endif
		//apply offsets only for valid values (not -255)
		*RemoteDS.Temperature = (p.AirTemp == Settings.InvalidValue) || (p.AirTemp > 1000)
			? *RemoteDS.Temperature : (p.AirTemp / 10.0 + Settings.RemoteTempOffset);	//remoteTemperature

		*RemoteDS.Humidity = (p.AirHum == Settings.InvalidValue) || (p.AirHum > 1000)
			? *RemoteDS.Humidity : (p.AirHum / 10.0);									//remoteHumidity

		*RemoteDS.Humidex = (p.AirHumidex == Settings.InvalidValue) || (p.AirHumidex > 1000)
			? *RemoteDS.Humidex : (p.AirHumidex / 10.0);									//remoteHumidex

		RemoteDS.Data[3] = (p.SoilTemp == Settings.InvalidValue) || (p.SoilTemp > 1000)
			? RemoteDS.Data[3] : (p.SoilTemp / 10.0 + Settings.SoilTempOffset);			//remoteSoilTemperature

		RemoteDS.Data[4] = p.SoilHum / 10.0;											//remoteSoilHumidity
		RemoteDS.Data[5] = p.Light / 10.0;												//remoteLight
		SystemDS.Data[6] = p.Vcc;														//vcc
		SystemDS.Data[7] = p.Uptime;													//uptime

		//handle failed messages
		if (!bReceivedRadioMsg || nInitialFailedMsgCnt > p.FailedMsgs) nInitialFailedMsgCnt = p.FailedMsgs;
		nFailedCntRadioTotal = p.FailedMsgs - nInitialFailedMsgCnt;

		nRainTicks = p.RainTips;
		nRemoteFreeRam = p.FreeRam;
		RemoteDS.TimeStamp = now();
		sRemoteUptime = getUptimeString(TimeSpan(SystemDS.Data[7]));

		bReceivedRadioMsg = true;

		//this gets executed after first received radio msg
		if (!Alarm.active(rainPerDayAlarm)) Alarm.enable(rainPerDayAlarm);
		if (!Alarm.active(rainPerHourAlarm)) Alarm.enable(rainPerHourAlarm);
	}
}

/// <summary>
/// Calculate how many radio transmissions failed since main unit startup.
/// This needs to be called every radio msg interval
/// </summary>
void getFailedRadioMessages()
{
	if (now() - RemoteDS.TimeStamp.unixtime() > Settings.RadioMsgInterval + 2 && bReceivedRadioMsg)
		nFailedCntRadioTotal++;
}
