/*
* © Francesco Potort? 2013 - GPLv3 - Revision: 1.13
*
* Send an NTP packet and wait for the response, return the Unix time
*
* To lower the memory footprint, no buffers are allocated for sending
* and receiving the NTP packets.  Four bytes of memory are allocated
* for transmision, the rest is random garbage collected from the data
* memory segment, and the received packet is read one byte at a time.
* The Unix time is returned, that is, seconds from 1970-01-01T00:00.
*/
unsigned long ntpUnixTime(UDP &udp)
{
	static int _nUDPInited = udp.begin(123); // open socket on arbitrary port

	// Only the first four bytes of an outgoing NTP packet need to be set
	// appropriately, the rest can be whatever.
	const long _lNTPFirstFourBytes = 0xEC0600E3; // NTP request header

	// Fail if WiFiUdp.begin() could not init a socket
	if (!_nUDPInited)
		return 0;

	// Clear received data from possible stray received packets
	udp.flush();

	// Send an NTP request
	if (!(udp.beginPacket(Settings.NTPServer, 123) // 123 is the NTP port
		&& udp.write((byte *)&_lNTPFirstFourBytes, 48) == 48
		&& udp.endPacket()))
		return 0;				// sending request failed

	// Wait for response; check every pollIntv ms up to maxPoll times
	const int _nPollIntv = 150;		// poll every this many ms
	const byte _byMaxPoll = 10;		// poll up to this many times
	int _nPktLen;				// received packet length
	for (byte i = 0; i < _byMaxPoll; i++) {
		if ((_nPktLen = udp.parsePacket()) == 48)
			break;
		Alarm.delay(_nPollIntv);
	}
	if (_nPktLen != 48)
		return 0;				// no correct packet received

	// Read and discard the first useless bytes
	// Set useless to 32 for speed; set to 40 for accuracy.
	const byte _byUseless = 40;
	for (byte i = 0; i < _byUseless; ++i)
		udp.read();

	// Read the integer part of sending time
	unsigned long _lTime = udp.read();	// NTP time
	for (byte i = 1; i < 4; i++)
		_lTime = _lTime << 8 | udp.read();

	// Round to the nearest second if we want accuracy
	// The fractionary part is the next byte divided by 256: if it is
	// greater than 500ms we round to the next second; we also account
	// for an assumed network delay of 50ms, and (0.5-0.05)*256=115;
	// additionally, we account for how much we delayed reading the packet
	// since its arrival, which we assume on average to be pollIntv/2.
	_lTime += (udp.read() > 115 - _nPollIntv / 8);

	// Discard the rest of the packet
	udp.flush();

	return _lTime - 2208988800ul;		// convert NTP time to Unix time
}

