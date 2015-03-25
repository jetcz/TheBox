
void receiveData() {

	uint8_t buf[RH_ASK_MAX_MESSAGE_LEN];
	uint8_t buflen = sizeof(buf);

	if (driver.recv(buf, &buflen)) // Non-blocking
	{
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
	}
}
