void setupSerial() {
#if DEBUG
	Serial.begin(9600);
	Serial.println(F("Serial initialized"));
#endif
}

void setupSD() {
	ledLight(1, 'y');
	if (!SD.begin(SD_SELECT_PIN)) {
#if DEBUG
		Serial.println(F("SD failed, or not present!"));
#endif
		lcd.clear();
		lcd.setCursor(0, 0);
		lcd.print(F("SD failed,"));
		lcd.setCursor(0, 1);
		lcd.print(F("or not present!"));
		ledLight(1, 'c');
		while (1);
	}
#if DEBUG
	else Serial.println(F("SD card initialized"));
#endif
	ledLight(1, 'g');
}

void setupPins(){

	pinMode(PIR_PIN, INPUT);			//pir pin

	pinMode(SD_SELECT_PIN, OUTPUT);		// SD chip select at pin 4
	digitalWrite(SD_SELECT_PIN, HIGH);

	pinMode(ETH_SELECT_PIN, OUTPUT);    // W5100 chip select at pin 10
	digitalWrite(ETH_SELECT_PIN, HIGH);

	pinMode(LCD_SWITCH_PWR_PIN, OUTPUT);//lcd switch current sink
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

	//current sensor right socket
	pinMode(CURRENT_RIGHT_PWR_PIN, OUTPUT);	//vcc
	digitalWrite(CURRENT_RIGHT_PWR_PIN, HIGH);
	pinMode(CURRENT_RIGHT_GND_PIN, OUTPUT);	//gnd
	digitalWrite(CURRENT_RIGHT_GND_PIN, LOW);

	//current sensor left socket
	pinMode(CURRENT_LEFT_PWR_PIN, OUTPUT);	//vcc
	digitalWrite(CURRENT_LEFT_PWR_PIN, HIGH);
	pinMode(CURRENT_LEFT_GND_PIN, OUTPUT);	//gnd
	digitalWrite(CURRENT_LEFT_GND_PIN, LOW);

#if DEBUG
	Serial.println(F("Pins initialized and set"));
#endif
}

void setupWire() {
	Wire.begin();
#if DEBUG
	Serial.println(F("Wire initialized"));
#endif
}

void setupBMP(){
	ledLight(1, 'y');
	if (!bmp.begin())
	{
#if DEBUG
		Serial.print(F("BMP180 failed, or not present!"));
#endif
		lcd.clear();
		lcd.setCursor(0, 0);
		lcd.print(F("BMP180 failed,"));
		lcd.setCursor(0, 1);
		lcd.print(F("or not present!"));
		ledLight(1, 'r');
		Alarm.delay(3000);
	}
	else {
#if DEBUG
		Serial.println(F("BMP180 initialized"));
#endif
	};
	ledLight(1, 'g');
}

void setupRTC(){
	ledLight(1, 'y');
	if (!rtc.begin())
	{
#if DEBUG
		Serial.print(F("RTC module failed, or not present!"));
#endif
		lcd.clear();
		lcd.setCursor(0, 0);
		lcd.print(F("RTC module failed,"));
		lcd.setCursor(0, 1);
		lcd.print(F("or not present!"));
		ledLight(1, 'r');
		rtc.adjust(DateTime(1970, 1, 1));
		Alarm.delay(3000);
	}
	if (!rtc.isrunning()) {
#if DEBUG
		Serial.println(F("RTC is not running!"));
#endif
		lcd.clear();
		lcd.setCursor(0, 0);
		lcd.print(F("RTC is not running!"));
		rtc.adjust(DateTime(1970, 1, 1));
		Alarm.delay(2000);
	}
	else {
		//rtc.adjust(DateTime(F(__DATE__), F(__TIME__))); //set RTC clock to compile date MUST COMMENT OUT
#if DEBUG
		Serial.println(F("RTC initialized and clock adjusted"));
#endif
		ledLight(1, 'g');
	} //sync interval for system clock from RTC module
	setSyncProvider(syncProvider); //sync system clock from RTC module
	setSyncInterval(60);
	dtSysStart = now();
}

void setupRadio(){
	bool _bSuccess = false;
	_bSuccess = radio.begin();
	radio.setAutoAck(1);                    // Ensure autoACK is enabled
	//radio.setChannel(24);
	radio.setCRCLength(RF24_CRC_8);
	_bSuccess = _bSuccess | radio.setDataRate(RF24_1MBPS);
	radio.setPALevel(RF24_PA_MAX);
	radio.setRetries(1, 15);                 // Smallest time between retries, max no. of retries
	radio.setPayloadSize(22);
	radio.openWritingPipe(pipes[1]);
	radio.openReadingPipe(1, pipes[0]);
	radio.startListening();                 // Start listening	

	if (_bSuccess)
	{
#if DEBUG
		Serial.println(F("Radio initialized"));
#endif
	}
	else {
#if DEBUG
		Serial.println(F("Radio failed to initialize or not present!"));
#endif
		lcd.clear();
		lcd.setCursor(0, 0);
		lcd.print(F("Radio failed to"));
		lcd.setCursor(0, 1);
		lcd.print(F("initialize!"));
		ledLight(2, 'm');
		Alarm.delay(2000);
	}
}

void setupEthernet() {
	resetEthShield(RESET_ETH_SHIELD_PIN);	//we have to manuly reset eth shield since we disabled autoreset by bending reset ping and icsp reset pin

	if (Settings.DHCP)
	{
		ledLight(1, 'y');
		lcd.clear();
		lcd.setCursor(0, 0);
		lcd.print(F("Obtaining DHCP lease"));

		if (Ethernet.begin(Settings.MAC) == 0) {
#if DEBUG
			Serial.println(F("Failed to initialize ethernet using DHCP"));
#endif
			lcd.clear();
			lcd.setCursor(0, 0);
			lcd.print(F("Failed to initialize"));
			lcd.setCursor(0, 1);
			lcd.print(F("ethernet using DHCP!"));
			ledLight(1, 'm');
			Alarm.delay(2000);
		}
		else {
			ledLight(1, 'g');
			lcd.clear();
#if DEBUG
			Serial.println(F("Ethernet DHCP initialized"));
			Serial.print(F("IP: "));
			Serial.println(Ethernet.localIP());
			Serial.print(F("Mask: "));
			Serial.println(Ethernet.subnetMask());
			Serial.print(F("GW: "));
			Serial.println(Ethernet.gatewayIP());
			Serial.print(F("DNS: "));
			Serial.println(Ethernet.dnsServerIP());
#endif

			for (int i = 0; i < 4; i++)
			{
				Settings.IP[i] = Ethernet.localIP()[i];
				Settings.Mask[i] = Ethernet.subnetMask()[i];
				Settings.GW[i] = Ethernet.gatewayIP()[i];
				Settings.DNS[i] = Ethernet.dnsServerIP()[i];
			}
		}
	}
	else {
		Ethernet.begin(Settings.MAC, Settings.IP, Settings.DNS, Settings.GW, Settings.Mask);
		ledLight(1, 'g');
		lcd.clear();
#if DEBUG
		Serial.println(F("Ethernet static initialized"));
		Serial.print(F("IP: "));
		Serial.println(Ethernet.localIP());
		Serial.print(F("Mask: "));
		Serial.println(Ethernet.subnetMask());
		Serial.print(F("GW: "));
		Serial.println(Ethernet.gatewayIP());
		Serial.print(F("DNS: "));
		Serial.println(Ethernet.dnsServerIP());
#endif
	}
	//this gives client.connect() max timeout approx 3s (probably not working as expected)
	W5100.setRetransmissionTime(0x07D0);
	W5100.setRetransmissionCount(3);
}

void setupLCD(){
	lcd.begin(20, 4);
	lcd.clear();
#if DEBUG
	Serial.println(F("LCD initialized"));
#endif
}

void setupAlarms() {
	Alarm.timerOnce(1, prepareDataSetArrays);
	Alarm.timerOnce(60, syncRTCwithNTP);
	updateTSAlarm = Alarm.timerRepeat(Settings.UpdateThingSpeakInterval, thingSpeak);
	Alarm.timerRepeat(Settings.RadioMsgInterval, getFailedRadioMessages); //count failed radio messages by default interval (gets reinitialized once we get first radio msg)
	Alarm.timerRepeat(1, system);
	Alarm.timerRepeat(Settings.UpdateSensorsInterval, prepareDataSetArrays); //get sensor data every x ms
	Alarm.timerRepeat(Settings.UpdatePWRSensorsInterval, getPWRData); //get sensor data every x ms
	if (PRINT_SUMMARY) printSummaryAlarm = Alarm.timerRepeat(Settings.UpdateSensorsInterval, printSensorDataSerial); //print sensor data to serial every x ms
	Alarm.timerRepeat(60, weatherForecast); //update weather forecast every minute - this MUST be interval 60s
	rainPerHourAlarm = Alarm.timerRepeat(60, getRainPerHour); //cumulative rainfall
	rainPerDayAlarm = Alarm.timerRepeat(Settings.UpdateRainPerDayInterval, getRainPerDay); //cumulative rainfall
	Alarm.timerRepeat(86400, syncRTCwithNTP); //sync RTC with NTP every 24h
	dhcpAlarm = Alarm.timerRepeat(100, dhcp); //refresh dhcp lease (if needed) every 100 sec (THIS IS BLOCKING!!!)

	//these get enabled with first radio msg
	Alarm.disable(rainPerHourAlarm);
	Alarm.disable(rainPerDayAlarm);

	if (!Settings.DHCP) Alarm.disable(dhcpAlarm);
	if (!Settings.TSenabled) Alarm.disable(updateTSAlarm);

#if DEBUG
	Serial.println(F("Alarms initialized"));
	Alarm.timerRepeat(3, printDebug);
#endif

}

