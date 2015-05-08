String floatToString(float val) {
	dtostrf(val, 1, 1, buff1);  //1 is mininum width, 1 is precision; float value is copied onto buff
	return buff1;
}

//convert int to string using registers, supposed to be fast and memory friendly
String intToString(register int i) {
	register unsigned char L = 1;
	register char c;
	register bool m = false;
	register char b;  // lower-byte of i
	// negative
	if (i < 0) {
		buff1[0] = '-';
		i = -i;
	}
	else L = 0;
	// ten-thousands
	if (i > 9999) {
		c = i < 20000 ? 1
			: i < 30000 ? 2
			: 3;
		buff1[L++] = c + 48;
		i -= c * 10000;
		m = true;
	}
	// thousands
	if (i > 999) {
		c = i < 5000
			? (i < 3000
			? (i < 2000 ? 1 : 2)
			: i < 4000 ? 3 : 4
			)
			: i < 8000
			? (i < 6000
			? 5
			: i < 7000 ? 6 : 7
			)
			: i < 9000 ? 8 : 9;
		buff1[L++] = c + 48;
		i -= c * 1000;
		m = true;
	}
	else if (m) buff1[L++] = '0';
	// hundreds
	if (i > 99) {
		c = i < 500
			? (i < 300
			? (i < 200 ? 1 : 2)
			: i < 400 ? 3 : 4
			)
			: i < 800
			? (i < 600
			? 5
			: i < 700 ? 6 : 7
			)
			: i < 900 ? 8 : 9;
		buff1[L++] = c + 48;
		i -= c * 100;
		m = true;
	}
	else if (m) buff1[L++] = '0';
	// decades (check on lower byte to optimize code)
	b = char(i);
	if (b > 9) {
		c = b < 50
			? (b < 30
			? (b < 20 ? 1 : 2)
			: b < 40 ? 3 : 4
			)
			: b < 80
			? (i < 60
			? 5
			: i < 70 ? 6 : 7
			)
			: i < 90 ? 8 : 9;
		buff1[L++] = c + 48;
		b -= c * 10;
		m = true;
	}
	else if (m) buff1[L++] = '0';
	// last digit
	buff1[L++] = b + 48;
	// null terminator
	buff1[L] = 0;
	return buff1;
}

//c = input char array, b = output byte array
void chArrToByteArr(char* c, byte* b) {

	//replace all , . : with white character space for strtol function
	for (int i = 0; i < buffLen1; i++)
	{
		if (c[i] == ',' || c[i] == '.' || c[i] == ':')
		{
			c[i] = ' ';
		}
	}

	//convert char array to byte array
	char* pEnd;
	for (int i = 0; i < sizeof(b) * 2; i++)
	{
		b[i] = strtol(c, &pEnd, 10);
		c = pEnd;
	}
}
