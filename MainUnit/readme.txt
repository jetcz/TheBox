RadioHead library:
Arduino ethernet library probably uses timer 1 somehow therefore you MUST add this to RH_ASK.cpp:
#define RH_ASK_ARDUINO_USE_TIMER2

Ethernet library:
It is recommended that you decrease timeout in Dhcp.h in beginWithDHCP method to something like 10-20s

