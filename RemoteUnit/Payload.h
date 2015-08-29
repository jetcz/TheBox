/// <summary>
/// Structure holding data which came from remote unit - this must be the same as Payload in Remote unit
/// Some values must be divided by 10 because we are transfering them as a whole number to save memory (int 2B vs float 4B)
/// </summary>
struct Payload
{
	int AirTemp; //must divide by 10
	int AirHum; //must divide by 10
	int AirHumidex; //must divide by 10
	int SoilTemp; //must divide by 10
	int SoilHum; //must divide by 10
	int Light; //must divide by 10
	unsigned int RainTips;
	unsigned long Uptime;
	unsigned int FreeRam;
	unsigned int Vcc;
	unsigned int FailedMsgs = 0;

	/// <summary>
	/// Print the payload values
	/// </summary>
	void print() {
		Serial.println();
		Serial.print(F("Air Temperature: "));
		Serial.print(AirTemp / 10.0, 1);
		Serial.println(F("C"));
		Serial.print(F("Air Humidity: "));
		Serial.print(AirHum / 10.0, 1);
		Serial.println(F("%RH"));
		Serial.print(F("Air Humidex: "));
		Serial.print(AirHumidex / 10.0, 1);
		Serial.println(F("C"));
		Serial.print(F("Soil Temperature: "));
		Serial.print(SoilTemp / 10.0, 1);
		Serial.println(F("C"));
		Serial.print(F("Soil Humidity: "));
		Serial.print(SoilHum / 10.0, 1);
		Serial.println(F("%RH"));
		Serial.print(F("Light: "));
		Serial.print(Light / 10.0, 1);
		Serial.println(F("%"));
		Serial.print(F("Rain Tips: "));
		Serial.print(RainTips);
		Serial.println();
		Serial.print(F("Uptime: "));
		Serial.print(Uptime);
		Serial.println(F("s"));
		Serial.print(F("Free RAM: "));
		Serial.print(FreeRam);
		Serial.println(F("B"));
		Serial.print(F("Vcc: "));
		Serial.print(Vcc);
		Serial.println(F("mV"));
		Serial.print(F("Failed messages: "));
		Serial.print(FailedMsgs);
		Serial.println();
	}
}; typedef struct Payload Payload;