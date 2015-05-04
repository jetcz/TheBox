boolean writeSDSched() {
	for (int i = 0; i < 4; i++)
	{
		String fp = "/settings/sch_r" + intToString(i + 1) + ".ini";
		fp.toCharArray(buffer, bufferLen);

		SD.remove(buffer);
		myFile = SD.open(buffer, FILE_WRITE);
		if (!myFile)
		{
			return false;
		}
		else {
			myFile.print(F("variable="));
			myFile.println(Sched[i].Variable);

			for (int j = 0; j < 5; j++)
			{
				myFile.println();
				myFile.println("[int" + intToString(j) + "]");
				myFile.print("enabled=");
				myFile.println(Sched[i].Enabled[j]);
				myFile.print("time=");
				myFile.print(Sched[i].Time[j][0]);
				myFile.print(":");
				myFile.println(Sched[i].Time[j][1]);
				myFile.print("from=");
				myFile.println(Sched[i].Value[j][0]);
				myFile.print("to=");
				myFile.println(Sched[i].Value[j][1]);
			}
			myFile.close();
		}

	}

	return true;
}

boolean readSDSched() {

	for (int i = 0; i < 4; i++)
	{
		String fp = "/settings/sch_r" + intToString(i + 1) + ".ini";
		fp.toCharArray(buff, buffLen);

		IniFile ini(buff);
		if (!ini.open()) {
			return false;
		}
		else {
			if (ini.getValue(NULL, "variable", buffer, bufferLen)) {
				Sched[i].Variable = atoi(buffer);
			}
			else return false;

			for (int j = 0; j < 5; j++)
			{
				String sec = "int" + intToString(j);
				sec.toCharArray(buff, buffLen);

				if (ini.getValue(buff, "enabled", buffer, bufferLen)) {
					Sched[i].Enabled[j] = buffer[0] != '0';
				}
				else return false;

				if (ini.getValue(buff, "time", buffer, bufferLen)) {
					chArrToByteArr(buffer, Sched[i].Time[j]);
				}
				else return false;

				if (ini.getValue(buff, "from", buffer, bufferLen)) {
					Sched[i].Value[j][0] = atof(buffer);
				}
				else return false;

				if (ini.getValue(buff, "to", buffer, bufferLen)) {
					Sched[i].Value[j][1] = atof(buffer);
				}
				else return false;
			}
		}
	}
	return true;
}

void deleteSDSched() {
	for (int i = 0; i < 4; i++)
	{
		String fp = "/settings/sch_r" + intToString(i + 1) + ".ini";
		fp.toCharArray(buffer, bufferLen);
		SD.remove(buffer);
	}
}