void sendMessage() {
	const char *msg = "hello";
	driver.send((uint8_t *)msg, strlen(msg));
	driver.waitPacketSent();
}
