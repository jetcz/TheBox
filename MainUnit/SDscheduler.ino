boolean writeSDSched() {
	for (int i = 0; i < 4; i++)
	{
		String fp = "/settings/sch_r" + intToString(i + 1) + ".ini";
		fp.toCharArray(buff1, buffLen1);

		SD.remove(buff1);
		myFile = SD.open(buff1, FILE_WRITE);
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
		fp.toCharArray(buff2, buffLen2);

		IniFile ini(buff2);
		if (!ini.open()) {
			return false;
		}
		else {
			if (ini.getValue(NULL, "variable", buff1, buffLen1)) {
				Sched[i].Variable = atoi(buff1);
			}
			else return false;

			for (int j = 0; j < 5; j++)
			{
				String sec = "int" + intToString(j);
				sec.toCharArray(buff2, buffLen2);

				if (ini.getValue(buff2, "enabled", buff1, buffLen1)) {
					Sched[i].Enabled[j] = buff1[0] != '0';
				}
				else return false;

				if (ini.getValue(buff2, "time", buff1, buffLen1)) {
					chArrToByteArr(buff1, Sched[i].Time[j]);
				}
				else return false;

				if (ini.getValue(buff2, "from", buff1, buffLen1)) {
					Sched[i].Value[j][0] = atof(buff1);
				}
				else return false;

				if (ini.getValue(buff2, "to", buff1, buffLen1)) {
					Sched[i].Value[j][1] = atof(buff1);
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
		fp.toCharArray(buff1, buffLen1);
		SD.remove(buff1);
	}
}