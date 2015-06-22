Use these libs https://github.com/jetcz/ArduinoLibraries 

Changes:

RadioHead library - RH_ASK.cpp:
Arduino ethernet library probably uses timer 1 somehow therefore you MUST add this to RH_ASK.cpp:
#define RH_ASK_ARDUINO_USE_TIMER2

Webduino library - WebServer.h:
change WEBDUINO_COMMANDS_COUNT to 16

TimeAlarms library
Implement Alarm.active() function

EmonLib library
Completely custom implementation (non-blocking behaviour, calculate two outlets at once)

Ethernet library
Shorter DHCP lease request timeout, shorter connect() timout

DHT library
Use non-Adafruit library, add noInterrupt()

