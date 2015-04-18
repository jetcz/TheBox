
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


boolean readSettings(char *path) {

	IniFile ini(path);
	if (!ini.open()) {
		return false;
	}
	else {

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
		if (path == relays) {
			if (ini.getValue(NULL, "modes", buffer, bufferLen)) { //modes=0,1,1,0
				chArrToByteArr(buffer, byRelay);
			}
			else {
#if DEBUG
				Serial.print(F("Could not read 'modes', error was "));
				printErrorMessage(ini.getError());
#endif
				return false;
			}
		}
	}
}

// Writes A Configuration file
boolean writeSDRelaySettings(char *path) {
	SD.remove(path);
	myFile = SD.open(path, FILE_WRITE);
	if (!myFile)
	{
		return false;
	}
	else {
		myFile.print(F("modes="));
		myFile.print(byRelay[0]);
		myFile.print(",");
		myFile.print(byRelay[1]);
		myFile.print(",");
		myFile.print(byRelay[2]);
		myFile.print(",");
		myFile.print(byRelay[3]);
		myFile.close();
		return true;
	}
}

boolean writeSDEthernetSettings() {
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