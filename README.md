This is a weather/home automation oriented project based on arduinos.
It consists of main unit (Arduino Mega 2560) and remote unit (Arduino Mini Pro 328P).
Remote unit is just battery/solar powered box with some sensors and radio module which sends sensor readings to main unit.
Main unit consists of some sensors as well as some relays. It is conected to ThingSpeak website,
which creates nice graphs from sensor readings from both remote and main unit. Main unit can also control relays based on user input from web interface, or automation rules based on sensor readings.
