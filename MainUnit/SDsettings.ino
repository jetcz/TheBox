
#if DEBUG
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


bool readSDSettings(char *path) {

	IniFile ini(path);
	if (!ini.open()) {
		return false;
	}
	else {

#pragma region ethernet

		if (path == Settings.EthernetPath)
		{
			if (ini.getValue(NULL, "dhcp", buff1, buffLen1)) {
				Eth.DHCP = buff1[0] != '0';

			}
			else {
#if DEBUG
				Serial.print(F("Could not read 'dhcp', error was "));
				printErrorMessage(ini.getError());
#endif
				return false;
			}

			if (ini.getValue(NULL, "ip", buff1, buffLen1)) {
				chArrToByteArr(buff1, Eth.IP);

			}
			else {
#if DEBUG
				Serial.print(F("Could not read 'ip', error was "));
				printErrorMessage(ini.getError());
#endif
				return false;
			}

			if (ini.getValue(NULL, "subnet", buff1, buffLen1)) {
				chArrToByteArr(buff1, Eth.Mask);
			}
			else {
#if DEBUG
				Serial.print(F("Could not read 'subnet', error was "));
				printErrorMessage(ini.getError());
#endif
				return false;
			}
			if (ini.getValue(NULL, "gw", buff1, buffLen1)) {
				chArrToByteArr(buff1, Eth.GW);
			}
			else {
#if DEBUG
				Serial.print(F("Could not read 'gw', error was "));
				printErrorMessage(ini.getError());
#endif
				return false;
			}
			if (ini.getValue(NULL, "dns", buff1, buffLen1)) {
				chArrToByteArr(buff1, Eth.DNS);
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
			if (ini.getValue(NULL, "modes", buff1, buffLen1)) { //modes=0,1,1,0
				chArrToByteArr(buff1, Settings.RelayMode);
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

#pragma region general

		if (path == Settings.SettingsPath) {
			if (ini.getValue(NULL, "RDSTimeout", buff1, buffLen1)) {
				Settings.RemoteDataSetTimeout = atoi(buff1);
			}
			else {
#if DEBUG
				Serial.print(F("Could not read 'RDSTimeout', error was "));
				printErrorMessage(ini.getError());
#endif
				return false;
			}
			if (ini.getValue(NULL, "invalidDSAction", buff1, buffLen1)) {
				bInvalidDSAction = buff1[0] != '0';
			}
			else {
#if DEBUG
				Serial.print(F("Could not read 'invalidDSaction', error was "));
				printErrorMessage(ini.getError());
#endif
				return false;
			}
			if (ini.getValue(NULL, "TSEnabled", buff1, buffLen1)) {
				bTSenabled = buff1[0] != '0';
			}
			else {
#if DEBUG
				Serial.print(F("Could not read 'TSEnabled', error was "));
				printErrorMessage(ini.getError());
#endif
				return false;
			}
			if (ini.getValue(NULL, "TSAddress", buff1, buffLen1)) {
				memcpy(&Settings.ThingSpeakAddress, buff1, buffLen1);
			}
			else {
#if DEBUG
				Serial.print(F("Could not read 'TSAddress', error was "));
				printErrorMessage(ini.getError());
#endif
				return false;
			}
			if (ini.getValue(NULL, "ntp", buff1, buffLen1)) {
				memcpy(&Settings.NTPServer, buff1, buffLen1);
			}
			else {
#if DEBUG
				Serial.print(F("Could not read 'ntp', error was "));
				printErrorMessage(ini.getError());
#endif
				return false;
			}
		}
#pragma endregion general

	}
}

bool writeSDRelaySettings() {
	SD.remove(Settings.RelaysPath);
	myFile = SD.open(Settings.RelaysPath, FILE_WRITE);
	if (!myFile)
	{
		return false;
	}
	else {
		myFile.print(F("modes="));
		myFile.print(Settings.RelayMode[0]);
		myFile.print(",");
		myFile.print(Settings.RelayMode[1]);
		myFile.print(",");
		myFile.print(Settings.RelayMode[2]);
		myFile.print(",");
		myFile.print(Settings.RelayMode[3]);
		myFile.close();
		return true;
	}
}

bool writeSDEthernetSettings() {
	SD.remove(Settings.EthernetPath);
	myFile = SD.open(Settings.EthernetPath, FILE_WRITE);
	if (!myFile)
	{
		return false;
	}
	else {
		myFile.print(F("dhcp="));
		myFile.println(Eth.DHCP);
		myFile.print(F("ip="));
		for (int i = 0; i < 4; i++)
		{
			myFile.print(Eth.IP[i]);
			if (i < 3)
			{
				myFile.print(F("."));
			}
		}
		myFile.println();

		myFile.print(F("subnet="));
		for (int i = 0; i < 4; i++)
		{
			myFile.print(Eth.Mask[i]);
			if (i < 3)
			{
				myFile.print(F("."));
			}
		}
		myFile.println();

		myFile.print(F("gw="));
		for (int i = 0; i < 4; i++)
		{
			myFile.print(Eth.GW[i]);
			if (i < 3)
			{
				myFile.print(F("."));
			}
		}
		myFile.println();

		myFile.print(F("dns="));
		for (int i = 0; i < 4; i++)
		{
			myFile.print(Eth.DNS[i]);
			if (i < 3)
			{
				myFile.print(F("."));
			}
		}
		myFile.close();
		return true;
	}
}

bool writeSDSettings() {
	SD.remove(Settings.SettingsPath);
	myFile = SD.open(Settings.SettingsPath, FILE_WRITE);
	if (!myFile)
	{
		return false;
	}
	else {
		myFile.print(F("RDSTimeout="));
		myFile.println(Settings.RemoteDataSetTimeout);
		myFile.print(F("invalidDSAction="));
		myFile.println(bInvalidDSAction);
		myFile.print(F("TSEnabled="));
		myFile.println(bTSenabled);
		myFile.print(F("TSAddress="));
		myFile.println(Settings.ThingSpeakAddress);
		myFile.print(F("ntp="));
		myFile.println(Settings.NTPServer);
		myFile.close();
		return true;
	}
}