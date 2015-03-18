
void receiveData() {

	uint8_t buf[RH_ASK_MAX_MESSAGE_LEN];
	uint8_t buflen = sizeof(buf);

	if (driver.recv(buf, &buflen)) // Non-blocking
	{
		int i;

		// Message with a good checksum received, dump it.
		driver.printBuffer("Got:", buf, buflen);
	}
}
