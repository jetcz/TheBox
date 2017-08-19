
void setupWatchdog()
{
	cli();  // disable all interrupts
	wdt_reset(); // reset the WDT timer
	MCUSR &= ~(1 << WDRF);  // because the data sheet said to
							/*
							WDTCSR configuration:
							WDIE = 1 :Interrupt Enable
							WDE = 1  :Reset Enable - I won't be using this on the 2560
							WDP3 = 0 :For 1000ms Time-out
							WDP2 = 1 :bit pattern is
							WDP1 = 1 :0110  change this for a different
							WDP0 = 0 :timeout period.
							*/
							// Enter Watchdog Configuration mode:
	WDTCSR = (1 << WDCE) | (1 << WDE);
	// Set Watchdog settings: interrupte enable, 0110 for timer
	WDTCSR = (1 << WDIE) | (0 << WDP3) | (1 << WDP2) | (1 << WDP1) | (0 << WDP0);
	sei();
#if DEBUG
	Serial.println("WatchDog initialized");  
#endif
}


/// <summary>
/// Setup serial
/// </summary>
void setupSerial()
{
#if DEBUG
	Serial.begin(115200);
	Serial.println(F("Serial initialized"));
#endif
}

/// <summary>
/// Setup SD card
/// </summary>
void setupSD()
{
	ledLight(1, 'y');
	file.setTimeout(0);
	if (!SD.begin(SD_SELECT_PIN))
	{
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

/// <summary>
/// Setup pins
/// </summary>
void setupPins()
{

	pinMode(RESET_ETH_SHIELD_PIN, OUTPUT);
	digitalWrite(RESET_ETH_SHIELD_PIN, HIGH);

	pinMode(RESET_WIFI_PIN, OUTPUT);
	digitalWrite(RESET_WIFI_PIN, HIGH);

	pinMode(PIR_PIN, INPUT);			//pir pin

	pinMode(SD_SELECT_PIN, OUTPUT);		// SD chip select at pin 4
	digitalWrite(SD_SELECT_PIN, HIGH);

	pinMode(ETH_SELECT_PIN, OUTPUT);    // W5100 chip select at pin 10
	digitalWrite(ETH_SELECT_PIN, HIGH);

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
	pinMode(CURRENT_LEFT_PIN, INPUT);
	pinMode(CURRENT_RIGHT_PIN, INPUT);

#if DEBUG
	Serial.println(F("Pins initialized and set"));
#endif
}

/// <summary>
/// Setup wire
/// </summary>
void setupWire()
{
	Wire.begin();

	TWSR &= 0xFC;

	//I2C bus speed [kHz]
	//0=1000
	//1=888
	//2=800
	//4=666
	//8=500
	//10=444
	//12=400
	//16=333
	//24=250
	//32=200
	//72=100 (default)
	TWBR = 2;

#if DEBUG
	long i2c_prescaler = pow(4, TWSR & 0x03);
	long i2c_freq = 16000000L / (16 + 2 * TWBR * i2c_prescaler);
	Serial.println(F("Wire initialized"));
	Serial.print(F("I2C clock: "));
	Serial.print(i2c_freq / 1000.0);
	Serial.println(F("kHz"));
#endif
}

/// <summary>
/// Setup BMP180
/// </summary>
void setupBMP()
{
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
	else
	{
#if DEBUG
		Serial.println(F("BMP180 initialized"));
#endif
	};
	ledLight(1, 'g');
}

/// <summary>
/// Setup RTC DS1307
/// </summary>
void setupRTC()
{
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
		Alarm.delay(3000);
		bRTCInitSuccess = false;
	}
	if (!rtc.isrunning())
	{
#if DEBUG
		Serial.println(F("RTC is not running!"));
#endif
		lcd.clear();
		lcd.setCursor(0, 0);
		lcd.print(F("RTC is not running!"));
		ledLight(1, 'r');
		Alarm.delay(3000);
		bRTCInitSuccess = false;
	}
	else
	{
		//rtc.adjust(DateTime(F(__DATE__), F(__TIME__))); //set RTC clock to compile date MUST COMMENT OUT
#if DEBUG
		Serial.println(F("RTC initialized and clock adjusted"));
#endif
		bRTCInitSuccess = true;
		setSyncProvider(syncProvider); //sync system clock from RTC module
		setSyncInterval(60); //sync interval for system clock from RTC module
		dtSysStart = now();
		ledLight(1, 'g');
	}
}

/// <summary>
/// Setup radio NRF24
/// </summary>
void setupRadio()
{

	radio.begin(); //for some reasong it returns false even tough radio initialized successfuly
	radio.setAutoAck(1);  // Ensure autoACK is enabled
	radio.setChannel(Settings.NRFChannel);
	radio.setCRCLength(RF24_CRC_8);
	radio.setDataRate(RF24_1MBPS);
	radio.setPALevel(RF24_PA_MAX);
	radio.setRetries(3, 15);  // Smallest time between retries, max no. of retries
	radio.setPayloadSize(sizeof(Payload));
	radio.openWritingPipe(pipes[1]);
	radio.openReadingPipe(1, pipes[0]);
	radio.startListening();  // Start listening		
}

/// <summary>
/// Setup ethernet shield W5100
/// </summary>
void setupEthernet()
{
	resetEthShield();	//we have to manuly reset eth shield since we disabled autoreset by bending reset ping and icsp reset pin
	if (Settings.DHCP)
	{
		ledLight(1, 'y');
		lcd.clear();
		lcd.setCursor(0, 0);
		lcd.print(F("Obtaining DHCP lease"));

		if (Ethernet.begin(Settings.MAC) == 0)
		{
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
		else
		{
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
	else
	{
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

/// <summary>
/// Setup LCD 20x4 on SPI bus
/// </summary>
void setupLCD()
{
	lcd.begin(20, 4);
	lcd.clear();
#if DEBUG
	Serial.println(F("LCD initialized"));
#endif
}

/// <summary>
/// Setup scheduler which controls the whole program
/// </summary>
void setupAlarms()
{
	Alarm.timerOnce(1, prepareDataSetArrays);
	Alarm.timerOnce(40, syncRTCwithNTP);
	updateTSAlarm = Alarm.timerRepeat(Settings.UpdateThingSpeakInterval, thingSpeak);
	Alarm.timerRepeat(Settings.RadioMsgInterval, getFailedRadioMessages); //count failed radio messages by default interval (gets reinitialized once we get first radio msg)
	Alarm.timerRepeat(1, system);
	Alarm.timerRepeat(Settings.UpdateSensorsInterval, prepareDataSetArrays); //get sensor data every x ms
	Alarm.timerRepeat(Settings.UpdatePWRSensorsInterval, getPWRData); //get sensor data every x ms	
	Alarm.timerRepeat(60, weatherForecast); //update weather forecast every minute - this MUST be interval 60s
	rainPerHourAlarm = Alarm.timerRepeat(Settings.UpdateRainInterval[0], getRainPerHour); //cumulative rainfall
	rainPerDayAlarm = Alarm.timerRepeat(Settings.UpdateRainInterval[1], getRainPerDay); //cumulative rainfall
	Alarm.timerRepeat(21600, syncRTCwithNTP); //sync RTC with NTP
	dhcpAlarm = Alarm.timerRepeat(100, dhcp); //refresh dhcp lease (if needed) every 100 sec (THIS IS BLOCKING!!!)
	ethShieldFreezeDetectAlarm = Alarm.timerRepeat(1, ethShieldFreezeDetect);
	Alarm.disable(ethShieldFreezeDetectAlarm);
	//these get enabled with first radio msg
	Alarm.disable(rainPerHourAlarm);
	Alarm.disable(rainPerDayAlarm);
	if (!Settings.DHCP) Alarm.disable(dhcpAlarm);
	if (!Settings.TSenabled) Alarm.disable(updateTSAlarm);
	if (!bRTCInitSuccess) Alarm.timerOnce(300, setupRTC); //if first initialization of RTC module failed, try to do it one more time later (in case the battery is dead and needs to be charged first)

#if DEBUG
#if PRINT_SUMMARY
	printSummaryAlarm = Alarm.timerRepeat(Settings.UpdateSensorsInterval, printSensorDataSerial); //print sensor data to serial every x ms
#endif
	Serial.println(F("Alarms initialized"));
	Alarm.timerRepeat(2, printDebug);
#endif	
}

