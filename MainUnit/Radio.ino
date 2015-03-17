/*  Arduino                         Receiver
*   GND------------------------------GND
*   D11------------------------------Data
*	5V------------------------------ - VCC
*                                    SHUT(not connected)
*                                    WAKEB(not connected)
*                                    GND |
*	ANT | -connect to your antenna syetem
*/

void receiveData() {

	uint8_t buf[RH_ASK_MAX_MESSAGE_LEN];
	uint8_t buflen = sizeof(buf);

	if (radioDriver.recv(buf, &buflen)) // Non-blocking
	{
		int i;
		// Message with a good checksum received, dump it.
		radioDriver.printBuffer("Got:", buf, buflen);
	}
}
