
#if DEBUG
/// <summary>
/// Helper method for handling error messages while reading ini's from SD card
/// </summary>
/// <param name="e"></param>
/// <param name="eol"></param>
void printErrorMessage(uint8_t e, bool eol = true)
{
	switch (e) {
	case IniFile::errorNoError:
		Serial.print(F("no error"));
		break;
	case IniFile::errorFileNotFound:
		Serial.print(F("file not found"));
		break;
	case IniFile::errorFileNotOpen:
		Serial.print(F("file not open"));
		break;
	case IniFile::errorBufferTooSmall:
		Serial.print(F("buffer too small"));
		break;
	case IniFile::errorSeekError:
		Serial.print(F("seek error"));
		break;
	case IniFile::errorSectionNotFound:
		Serial.print(F("section not found"));
		break;
	case IniFile::errorKeyNotFound:
		Serial.print(F("key not found"));
		break;
	case IniFile::errorEndOfFile:
		Serial.print(F("end of file"));
		break;
	case IniFile::errorUnknownError:
		Serial.print(F("unknown error"));
		break;
	default:
		Serial.print(F("unknown error value"));
		break;
	}
	if (eol)
		Serial.println();
}
#endif

/// <summary>
/// Universal method for reading various setting files from SD card (ethernet, relays, offsets, general).
/// </summary>
/// <param name="path">Path to the ini</param>
/// <returns>true if reading succeeded, otherwise false</returns>
bool readSDSettings(char *path) {

	IniFile ini(path);
	if (!ini.open()) {
		return false;
	}
	else {

#pragma region ethernet

		if (path == Settings.EthernetPath)
		{
			if (ini.getValue(NULL, "dhcp", cBuff1, nBuffLen1)) {
				Settings.DHCP = cBuff1[0] != '0';

			}
			else {
#if DEBUG
				Serial.print(F("Could not read 'dhcp', error was "));
				printErrorMessage(ini.getError());
#endif
				return false;
			}

			if (ini.getValue(NULL, "ip", cBuff1, nBuffLen1)) {
				chArrToByteArr(cBuff1, Settings.IP);

			}
			else {
#if DEBUG
				Serial.print(F("Could not read 'ip', error was "));
				printErrorMessage(ini.getError());
#endif
				return false;
			}

			if (ini.getValue(NULL, "subnet", cBuff1, nBuffLen1)) {
				chArrToByteArr(cBuff1, Settings.Mask);
			}
			else {
#if DEBUG
				Serial.print(F("Could not read 'subnet', error was "));
				printErrorMessage(ini.getError());
#endif
				return false;
			}
			if (ini.getValue(NULL, "gw", cBuff1, nBuffLen1)) {
				chArrToByteArr(cBuff1, Settings.GW);
			}
			else {
#if DEBUG
				Serial.print(F("Could not read 'gw', error was "));
				printErrorMessage(ini.getError());
#endif
				return false;
			}
			if (ini.getValue(NULL, "dns", cBuff1, nBuffLen1)) {
				chArrToByteArr(cBuff1, Settings.DNS);
			}
			else {
#if DEBUG
				Serial.print(F("Could not read 'dns', error was "));
				printErrorMessage(ini.getError());
#endif
				return false;
			}

			return true;
		}

#pragma endregion ethernet

#pragma region relays

		if (path == Settings.RelaysPath) {
			if (ini.getValue(NULL, "modes", cBuff1, nBuffLen1)) { //modes=0,1,1,0
				chArrToByteArr(cBuff1, Settings.RelayMode);
			}
			else {
#if DEBUG
				Serial.print(F("Could not read 'modes', error was "));
				printErrorMessage(ini.getError());
#endif
				return false;
			}
		}

#pragma endregion relays

#pragma region offsets

		if (path == Settings.OffsetsPath) {
			if (ini.getValue(NULL, "SysTempOffset", cBuff1, nBuffLen1)) {
				Settings.SysTempOffset = atof(cBuff1);
			}
			else {
#if DEBUG
				Serial.print(F("Could not read 'SysTempOffset', error was "));
				printErrorMessage(ini.getError());
#endif
				return false;
			}
			if (ini.getValue(NULL, "PressureOffset", cBuff1, nBuffLen1)) {
				Settings.PressureOffset = atof(cBuff1);
			}
			else {
#if DEBUG
				Serial.print(F("Could not read 'PressureOffset', error was "));
				printErrorMessage(ini.getError());
#endif
				return false;
			}
			if (ini.getValue(NULL, "MainTempOffset", cBuff1, nBuffLen1)) {
				Settings.MainTempOffset = atof(cBuff1);
			}
			else {
#if DEBUG
				Serial.print(F("Could not read 'MainTempOffset', error was "));
				printErrorMessage(ini.getError());
#endif
				return false;
			}
			if (ini.getValue(NULL, "RemoteTempOffset", cBuff1, nBuffLen1)) {
				Settings.RemoteTempOffset = atof(cBuff1);
			}
			else {
#if DEBUG
				Serial.print(F("Could not read 'RemoteTempOffset', error was "));
				printErrorMessage(ini.getError());
#endif
				return false;
			}
			if (ini.getValue(NULL, "SoilTempOffset", cBuff1, nBuffLen1)) {
				Settings.SoilTempOffset = atof(cBuff1);
			}
			else {
#if DEBUG
				Serial.print(F("Could not read 'SoilTempOffset', error was "));
				printErrorMessage(ini.getError());
#endif
				return false;
			}
		}
#pragma endregion offsets

#pragma region general

		if (path == Settings.SettingsPath) {
			if (ini.getValue(NULL, "RDSTimeout", cBuff1, nBuffLen1)) {
				Settings.RemoteDataSetTimeout = atoi(cBuff1);
			}
			else {
#if DEBUG
				Serial.print(F("Could not read 'RDSTimeout', error was "));
				printErrorMessage(ini.getError());
#endif
				return false;
			}
			if (ini.getValue(NULL, "invalidDSAction", cBuff1, nBuffLen1)) {
				Settings.InvalidDSAction = cBuff1[0] != '0';
			}
			else {
#if DEBUG
				Serial.print(F("Could not read 'invalidDSaction', error was "));
				printErrorMessage(ini.getError());
#endif
				return false;
			}
			if (ini.getValue(NULL, "TSEnabled", cBuff1, nBuffLen1)) {
				Settings.TSenabled = cBuff1[0] != '0';
			}
			else {
#if DEBUG
				Serial.print(F("Could not read 'TSEnabled', error was "));
				printErrorMessage(ini.getError());
#endif
				return false;
			}
			if (ini.getValue(NULL, "TSAddress", cBuff1, nBuffLen1)) {
				memcpy(&Settings.ThingSpeakAddress, cBuff1, nBuffLen1);
			}
			else {
#if DEBUG
				Serial.print(F("Could not read 'TSAddress', error was "));
				printErrorMessage(ini.getError());
#endif
				return false;
			}
			if (ini.getValue(NULL, "ntp", cBuff1, nBuffLen1)) {
				memcpy(&Settings.NTPServer, cBuff1, nBuffLen1);
			}
			else {
#if DEBUG
				Serial.print(F("Could not read 'ntp', error was "));
				printErrorMessage(ini.getError());
#endif
				return false;
			}
			if (ini.getValue(NULL, "NRFChannel", cBuff1, nBuffLen1)) {
				Settings.NRFChannel = atoi(cBuff1);
			}
			else {
#if DEBUG
				Serial.print(F("Could not read 'NRFChannel', error was "));
				printErrorMessage(ini.getError());
#endif
				return false;
			}
		}
#pragma endregion general

	}
}

/// <summary>
/// Writes current relay settings to SD card
/// </summary>
/// <returns>true if writing succeeded, otherwise false</returns>
bool writeSDRelaySettings() {
	SD.remove(Settings.RelaysPath);
	file = SD.open(Settings.RelaysPath, FILE_WRITE);
	if (!file)
	{
		return false;
	}
	else {
		file.print(F("modes="));
		file.print(Settings.RelayMode[0]);
		file.print(",");
		file.print(Settings.RelayMode[1]);
		file.print(",");
		file.print(Settings.RelayMode[2]);
		file.print(",");
		file.print(Settings.RelayMode[3]);
		file.close();
		return true;
	}
}

/// <summary>
/// Writes current ethernet settings to SD card
/// </summary>
/// <returns>true if writing succeeded, otherwise false</returns>
bool writeSDEthernetSettings() {
	SD.remove(Settings.EthernetPath);
	file = SD.open(Settings.EthernetPath, FILE_WRITE);
	if (!file)
	{
		return false;
	}
	else {
		file.print(F("dhcp="));
		file.println(Settings.DHCP);
		file.print(F("ip="));
		for (int i = 0; i < 4; i++)
		{
			file.print(Settings.IP[i]);
			if (i < 3)
			{
				file.print(F("."));
			}
		}
		file.println();

		file.print(F("subnet="));
		for (int i = 0; i < 4; i++)
		{
			file.print(Settings.Mask[i]);
			if (i < 3)
			{
				file.print(F("."));
			}
		}
		file.println();

		file.print(F("gw="));
		for (int i = 0; i < 4; i++)
		{
			file.print(Settings.GW[i]);
			if (i < 3)
			{
				file.print(F("."));
			}
		}
		file.println();

		file.print(F("dns="));
		for (int i = 0; i < 4; i++)
		{
			file.print(Settings.DNS[i]);
			if (i < 3)
			{
				file.print(F("."));
			}
		}
		file.close();
		return true;
	}
}

/// <summary>
/// Writes general settings to SD card
/// </summary>
/// <returns>true if writing succeeded, otherwise false</returns>
bool writeSDSettings() {
	SD.remove(Settings.SettingsPath);
	file = SD.open(Settings.SettingsPath, FILE_WRITE);
	if (!file)
	{
		return false;
	}
	else {		
		file.print(F("RDSTimeout="));
		file.println(Settings.RemoteDataSetTimeout);
		file.print(F("invalidDSAction="));
		file.println(Settings.InvalidDSAction);
		file.print(F("TSEnabled="));
		file.println(Settings.TSenabled);
		file.print(F("TSAddress="));
		file.println(Settings.ThingSpeakAddress);
		file.print(F("ntp="));
		file.println(Settings.NTPServer);
		file.print(F("NRFChannel="));
		file.println(Settings.NRFChannel);
		file.close();
		return true;
	}
}

/// <summary>
/// Writes sensor offsets settings to SD card.
/// </summary>
/// <returns>true if writing succeeded, otherwise false</returns>
bool writeSDOffsets() {
	SD.remove(Settings.OffsetsPath);
	file = SD.open(Settings.OffsetsPath, FILE_WRITE);
	if (!file)
	{
		return false;
	}
	else {
		file.print(F("SysTempOffset="));
		file.println(Settings.SysTempOffset);
		file.print(F("PressureOffset="));
		file.println(Settings.PressureOffset);
		file.print(F("MainTempOffset="));
		file.println(Settings.MainTempOffset);
		file.print(F("RemoteTempOffset="));
		file.println(Settings.RemoteTempOffset);
		file.print(F("SoilTempOffset="));
		file.println(Settings.SoilTempOffset);
		file.close();
		return true;
	}
}