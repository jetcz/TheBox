/*
Credits go to overSKILL (http://overskill.alexshu.com/saving-loading-settings-on-sd-card-with-arduino/)
The config file muset be in this format:

relay1=0
relay2=1
relay3=0
relay4=2

where 0 is off, 1 is on and anything else is auto
each config file must end by empty line!!!
*/

void readSDSettings(char* path){
	char character;
	String settingName;
	String settingValue;
	myFile = SD.open(path);
	if (myFile) {
		while (myFile.available()) {
			character = myFile.read();
			while (character != '='){
				settingName = settingName + character;
				character = myFile.read();
			}
			character = myFile.read();
			while (character != '\n'){
				settingValue = settingValue + character;
				character = myFile.read();
				if (character == '\n'){
					
					// Apply the value to the parameter
					if (path = "/settings/relays.ini")
					{
						applyRelaySetting(settingName, settingValue);
					}
					if (path = "/settings/ethernet.ini")
					{
						applyEthernetSetting(settingName, settingValue);
					}
					// Reset Strings
					settingName = "";
					settingValue = "";
				}
			}
		}
		// close the file:
		if (path = "/settings/ethernet.ini")
		{
			bDhcp = false;
		}
		myFile.close();
	}
	else {
		// if the file didn't open, print an error:
		Serial.println(F("Error opening file!"));
		if (path = "/settings/ethernet.ini")
		{
			bDhcp = true;
		}
	}
}

// Writes A Configuration file
void writeSDRelaySettings(char* file) {
	// Delete the old One
	SD.remove(file);
	// Create new one
	myFile = SD.open(file, FILE_WRITE);
	// writing in the file works just like regular print()/println() function
	myFile.print(F("relay1="));
	myFile.println(byRelay[0]);
	myFile.print(F("relay2="));
	myFile.println(byRelay[1]);
	myFile.print(F("relay3="));
	myFile.println(byRelay[2]);
	myFile.print(F("relay4="));
	myFile.println(byRelay[3]);
	// close the file:
	myFile.close();
	//Serial.println("Writing done.");
}

// Apply the value to the parameter by searching for the parameter name 
void applyRelaySetting(String settingName, String settingValue) {
	if (settingName == "relay1") {
		byRelay[0] = settingValue.toInt();
	}
	if (settingName == "relay2") {
		byRelay[1] = settingValue.toInt();
	}
	if (settingName == "relay3") {
		byRelay[2] = settingValue.toInt();
	}
	if (settingName == "relay4") {
		byRelay[3] = settingValue.toInt();
	}
}

void applyEthernetSetting(String settingName, String settingValue) {
	
	if (settingName == "ip1") {
		ip[0] = settingValue.toInt();
	}
	if (settingName == "ip2") {
		ip[1] = settingValue.toInt();
	}
	if (settingName == "ip3") {
		ip[2] = settingValue.toInt();
	}
	if (settingName == "ip4") {
		ip[3] = settingValue.toInt();
	}
	if (settingName == "dns1") {
		dns1[0] = settingValue.toInt();
	}
	if (settingName == "dns2") {
		dns1[1] = settingValue.toInt();
	}
	if (settingName == "dns3") {
		dns1[2] = settingValue.toInt();
	}
	if (settingName == "dns4") {
		dns1[3] = settingValue.toInt();
	}

	if (settingName == "gateway1") {
		gateway[0] = settingValue.toInt();
	}
	if (settingName == "gateway2") {
		gateway[1] = settingValue.toInt();
	}
	if (settingName == "gateway3") {
		gateway[2] = settingValue.toInt();
	}
	if (settingName == "gateway4") {
		gateway[3] = settingValue.toInt();
	}

	if (settingName == "subnet1") {
		subnet[0] = settingValue.toInt();
	}
	if (settingName == "subnet2") {
		subnet[1] = settingValue.toInt();
	}
	if (settingName == "subnet3") {
		subnet[2] = settingValue.toInt();
	}
	if (settingName == "subnet4") {
		subnet[3] = settingValue.toInt();
	}
}
