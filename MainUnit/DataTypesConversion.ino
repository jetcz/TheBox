String floatToString(float val) {

	dtostrf(val, 3, 2, charVal);  //3 is mininum width, 2 is precision; float value is copied onto buff

	/*
	//convert chararray to string
	String stringVal = "";     //data on buff is copied to this string
	for (int i = 0; i < sizeof(charVal); i++)
	{
		//Serial.println(charVal[i]);
		stringVal += charVal[i];
	}
	return stringVal;
	*/
	return charVal;
}


String intToString(register int i) {
	register unsigned char L = 1;
	register char c;
	register boolean m = false;
	register char b;  // lower-byte of i
	// negative
	if (i < 0) {
		charVal[0] = '-';
		i = -i;
	}
	else L = 0;
	// ten-thousands
	if (i > 9999) {
		c = i < 20000 ? 1
			: i < 30000 ? 2
			: 3;
		charVal[L++] = c + 48;
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
		charVal[L++] = c + 48;
		i -= c * 1000;
		m = true;
	}
	else if (m) charVal[L++] = '0';
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
		charVal[L++] = c + 48;
		i -= c * 100;
		m = true;
	}
	else if (m) charVal[L++] = '0';
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
		charVal[L++] = c + 48;
		b -= c * 10;
		m = true;
	}
	else if (m) charVal[L++] = '0';
	// last digit
	charVal[L++] = b + 48;
	// null terminator
	charVal[L] = 0;
	return charVal;
}
