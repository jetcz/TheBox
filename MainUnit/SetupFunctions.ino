void setupSerial() {
	Serial.begin(9600);
	Serial.println(F("Serial initialized"));
}


void setupSD() {
	ledLight(1, 'y');
	if (!SD.begin(SD_SELECT_PIN)) {
		Serial.println(F("SD failed, or not present!"));
		lcd.clear();
		lcd.setCursor(0, 0);
		lcd.print(F("SD failed,"));
		lcd.setCursor(0, 1);
		lcd.print(F("or not present!"));
		ledLight(1, 'c');
		while (1);
	}
	else Serial.println(F("SD card initialized"));
	ledLight(1, 'g');
}

void setupPins(){


	pinMode(RADIO_CTRL_PIN, OUTPUT);			//radio control sleep pin
	digitalWrite(RADIO_CTRL_PIN, HIGH);

	pinMode(PIR_PIN, INPUT);			//pir pin

	pinMode(SD_SELECT_PIN, OUTPUT);       // SD chip select at pin 4
	digitalWrite(SD_SELECT_PIN, HIGH);

	pinMode(ETH_SELECT_PIN, OUTPUT);     // W5100 chip select at pin 10
	digitalWrite(ETH_SELECT_PIN, HIGH);

	pinMode(LCD_SWITCH_PWR_PIN, OUTPUT); //lcd switch current sink
	digitalWrite(LCD_SWITCH_PWR_PIN, LOW);

	pinMode(PIR_PIN, INPUT);			//pir pin
	//turn off relays
	for (int i = 0; i < 4; i++)
	{
		digitalWrite(RELAY_PIN[i], HIGH);
		pinMode(RELAY_PIN[i], OUTPUT);
	}

	for (int i = 0; i < 3; i++)			//setup led pins and switch pins
	{
		pinMode(LED1[i], OUTPUT);
		pinMode(LED2[i], OUTPUT);
		pinMode(LED3[i], OUTPUT);
		ledLight(i, 'k');
		pinMode(LCD_SWITCH[i], INPUT_PULLUP);
	}
	Serial.println(F("Pins initialized and set"));
}

void setupWire() {
	Wire.begin();
	Serial.println(F("Wire initialized"));
	ledLight(1, 'g');
}


void setupDHT(){
	dht.begin();
	Serial.println(F("DHT22 initialized"));
}

void setupBMP(){
	ledLight(1, 'y');
	if (!bmp.begin())
	{
		Serial.print(F("BMP180 failed, or not present!"));
		lcd.clear();
		lcd.setCursor(0, 0);
		lcd.print(F("BMP180 failed,"));
		lcd.setCursor(0, 1);
		lcd.print(F("or not present!"));
		ledLight(1, 'r');
		while (1);
	}
	else Serial.println(F("BMP180 initialized"));
	ledLight(1, 'g');
}

void setupRTC(){
	ledLight(1, 'y');
	if (!rtc.begin())
	{
		Serial.print(F("RTC module failed, or not present!"));
		lcd.clear();
		lcd.setCursor(0, 0);
		lcd.print(F("RTC module failed,"));
		lcd.setCursor(0, 1);
		lcd.print(F("or not present!"));
		ledLight(1, 'r');
		while (1);
	}
	if (!rtc.isrunning()) {
		Serial.println(F("RTC is not running!"));
		lcd.clear();
		lcd.setCursor(0, 0);
		lcd.print(F("RTC is not running!"));

	}
	else {
		//rtc.adjust(DateTime(F(__DATE__), F(__TIME__))); //set RTC clock to compile date MUST COMMENT OUT
		Serial.println(F("RTC initialized and clock adjusted"));
		setSyncProvider(syncProvider); //sync system clock from RTC module
		setSyncInterval(600);
		ledLight(1, 'g');
	} //sync interval for system clock from RTC module
	sysStart = now();
}

void setupRadio(){
	if (!driver.init()) {
		Serial.println(F("Radio failed"));
	}
	else Serial.println(F("Radio initialized"));
}

void setupEthernet() {
	ledLight(1, 'y');
	resetEthShield(RESET_ETH_SHIELD_PIN);	//we have to manuly reset eth shield since we disabled autoreset by bending reset ping and icsp reset pin

	if (bDhcp)
	{
		lcd.clear();
		lcd.setCursor(0, 0);
		lcd.print(F("Obtaining DHCP lease"));

		if (Ethernet.begin(mac) == 0) {
			bConnectivityCheck = false;
			Serial.println(F("Failed to initialize ethernet using DHCP"));
			lcd.clear();
			lcd.setCursor(0, 0);
			lcd.print(F("Failed to initialize"));
			lcd.setCursor(0, 1);
			lcd.print(F("ethernet using DHCP!"));
			ledLight(1, 'm');
		}
		else {
			bConnectivityCheck = true;
			Serial.println(F("Ethernet DHCP initialized"));
			ledLight(1, 'g');
			lcd.clear();
			Serial.print(F("IP: "));
			Serial.println(Ethernet.localIP());
			Serial.print(F("GW: "));
			Serial.println(Ethernet.gatewayIP());
			Serial.print(F("DNS: "));
			Serial.println(Ethernet.dnsServerIP());
		}
	}
	else {
		bConnectivityCheck = true;
		Ethernet.begin(mac, ip, dns1, gateway, subnet);
		Serial.println(F("Ethernet static initialized"));
		ledLight(1, 'g');
		lcd.clear();
		Serial.print(F("IP: "));
		Serial.println(Ethernet.localIP());
		Serial.print(F("GW: "));
		Serial.println(Ethernet.gatewayIP());
		Serial.print(F("DNS: "));
		Serial.println(Ethernet.dnsServerIP());
	}
	//this gives client.connect() max timeout approx 3s
	W5100.setRetransmissionTime(0x07D0);
	W5100.setRetransmissionCount(3); 
}


void setupLCD(){
	lcd.begin(20, 4);
	lcd.clear();
	Serial.println(F("LCD initialized"));
}

void setupTimers() {
	Alarm.timerOnce(1, prepareDataSetArrays);
	byAlarm[0] = Alarm.timerRepeat(1, system);
	byAlarm[1] = Alarm.timerRepeat(iUpdateSensorsInterval, prepareDataSetArrays); //get sensor data every x ms
	byAlarm[2] = Alarm.timerRepeat(iUpdateSensorsInterval, printSensorDataSerial); //print sensor data to serial every x ms
	byAlarm[3] = Alarm.timerRepeat(iUpdateThingSpeakInterval, thingSpeak); //update ThingSpeak every x ms
	byAlarm[4] = Alarm.timerRepeat(60, weatherForecastTimer); //update weather forecast every minute - this MUST be interval 60s
	byAlarm[5] = Alarm.timerRepeat(1, printLcd); //refresh sensor data to lcd every second
	if (bDhcp)
	{
		byAlarm[6] = Alarm.timerRepeat(100, dhcp); //refresh dhcp lease (if needed) every 100 sec (THIS IS BLOCKING!!!)

	}	Serial.println(F("Timers initialized"));
//	byAlarm[7] = Alarm.timerRepeat(5, printDebug);
}

