
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

		if (path == ethernet)
		{
			if (ini.getValue(NULL, "ip", buffer, bufferLen)) {
				chArrToByteArr(buffer, ip);

			}
			else {
#if DEBUG
				Serial.print(F("Could not read 'ip', error was "));
				printErrorMessage(ini.getError());
#endif
				return false;
			}

			if (ini.getValue(NULL, "subnet", buffer, bufferLen)) {
				chArrToByteArr(buffer, subnet);
			}
			else {
#if DEBUG
				Serial.print(F("Could not read 'subnet', error was "));
				printErrorMessage(ini.getError());
#endif
				return false;
			}
			if (ini.getValue(NULL, "gw", buffer, bufferLen)) {
				chArrToByteArr(buffer, gw);
			}
			else {
#if DEBUG
				Serial.print(F("Could not read 'gw', error was "));
				printErrorMessage(ini.getError());
#endif
				return false;
			}
			if (ini.getValue(NULL, "dns", buffer, bufferLen)) {
				chArrToByteArr(buffer, dns1);
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

		if (path == relays) {
			if (ini.getValue(NULL, "modes", buffer, bufferLen)) { //modes=0,1,1,0
				chArrToByteArr(buffer, byRelayMode);
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

		if (path == general) {
			if (ini.getValue(NULL, "invalidDSAction", buffer, bufferLen)) {
				bInvalidDSAction = buffer[0] != '0';
			}
			else {
#if DEBUG
				Serial.print(F("Could not read 'invalidDSaction', error was "));
				printErrorMessage(ini.getError());
#endif
				return false;
			}
			if (ini.getValue(NULL, "thingspeakEnabled", buffer, bufferLen)) {
				bTSenabled = buffer[0] != '0';
			}
			else {
#if DEBUG
				Serial.print(F("Could not read 'thingspeakEnabled', error was "));
				printErrorMessage(ini.getError());
#endif
				return false;
			}
			if (ini.getValue(NULL, "thingspeakAddress", buffer, bufferLen)) {
				memcpy(&cThingSpeakAddress, buffer, bufferLen);
			}
			else {
#if DEBUG
				Serial.print(F("Could not read 'thingspeakAddress', error was "));
				printErrorMessage(ini.getError());
#endif
				return false;
			}
			if (ini.getValue(NULL, "ntp", buffer, bufferLen)) {
				memcpy(&cTimeServer, buffer, bufferLen);
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
	SD.remove(relays);
	myFile = SD.open(relays, FILE_WRITE);
	if (!myFile)
	{
		return false;
	}
	else {
		myFile.print(F("modes="));
		myFile.print(byRelayMode[0]);
		myFile.print(",");
		myFile.print(byRelayMode[1]);
		myFile.print(",");
		myFile.print(byRelayMode[2]);
		myFile.print(",");
		myFile.print(byRelayMode[3]);
		myFile.close();
		return true;
	}
}

bool writeSDEthernetSettings() {
	SD.remove(ethernet);
	myFile = SD.open(ethernet, FILE_WRITE);
	if (!myFile)
	{
		return false;
	}
	else {
		myFile.print(F("ip="));
		for (int i = 0; i < 4; i++)
		{
			myFile.print(ip[i]);
			if (i < 3)
			{
				myFile.print(F("."));
			}
		}
		myFile.println();

		myFile.print(F("subnet="));
		for (int i = 0; i < 4; i++)
		{
			myFile.print(subnet[i]);
			if (i < 3)
			{
				myFile.print(F("."));
			}
		}
		myFile.println();

		myFile.print(F("gw="));
		for (int i = 0; i < 4; i++)
		{
			myFile.print(gw[i]);
			if (i < 3)
			{
				myFile.print(F("."));
			}
		}
		myFile.println();

		myFile.print(F("dns="));
		for (int i = 0; i < 4; i++)
		{
			myFile.print(dns1[i]);
			if (i < 3)
			{
				myFile.print(F("."));
			}
		}
		myFile.close();
		return true;
	}
}

bool writeSDGeneralSettings() {
	SD.remove(general);
	myFile = SD.open(general, FILE_WRITE);
	if (!myFile)
	{
		return false;
	}
	else {
		myFile.print(F("invalidDSAction="));
		myFile.print(bInvalidDSAction);
		myFile.println();
		myFile.print(F("thingspeakEnabled="));
		myFile.print(bTSenabled);
		myFile.println();
		myFile.print(F("thingspeakAddress="));
		myFile.print(cThingSpeakAddress);
		myFile.println();
		myFile.print(F("ntp="));
		myFile.print(cTimeServer);
		myFile.println();
		return true;
	}
}