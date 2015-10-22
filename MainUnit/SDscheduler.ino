/// <summary>
/// Write current scheduler settings to SD card.
/// </summary>
/// <returns>True if writing succeeded, otherwise false</returns>
bool writeSDSched() {
	for (int i = 0; i < 4; i++)
	{
		String _sPath;
		_sPath.reserve(21);
		_sPath = "/settings/sch_r" + intToString(i + 1) + ".ini";
		_sPath.toCharArray(cBuff1, nBuffLen1);

		SD.remove(cBuff1);
		file = SD.open(cBuff1, FILE_WRITE);
		if (!file)
		{
			return false;
		}
		else {
			file.print(F("variable="));
			file.println(Sched[i].Variable);

			for (int j = 0; j < 5; j++)
			{
				file.println();
				file.println("[int" + intToString(j) + "]");
				file.print("enabled=");
				file.println(Sched[i].Enabled[j]);
				file.print("time=");
				file.print(Sched[i].Time[j][0]);
				file.print(":");
				file.println(Sched[i].Time[j][1]);
				file.print("from=");
				file.println(Sched[i].Value[j][0]);
				file.print("to=");
				file.println(Sched[i].Value[j][1]);
			}
			file.close();
		}

	}

	return true;
}

/// <summary>
/// Read scheduler settings from SD card.
/// </summary>
/// <returns>True if reading succeeded, otherwise false</returns>
bool readSDSched() {

	for (int i = 0; i < 4; i++)
	{
		String _sPath;
		_sPath.reserve(21);
		_sPath = "/settings/sch_r" + intToString(i + 1) + ".ini";
		_sPath.toCharArray(cBuff2, nBuffLen2);

		IniFile ini(cBuff2);
		if (!ini.open()) {
			return false;
		}
		else {
			if (ini.getValue(NULL, "variable", cBuff1, nBuffLen1)) {
				Sched[i].Variable = atoi(cBuff1);
			}
			else return false;

			String sec;
			sec.reserve(5);
			for (int j = 0; j < 5; j++)
			{
				sec = "int" + intToString(j);
				sec.toCharArray(cBuff2, nBuffLen2);

				if (ini.getValue(cBuff2, "enabled", cBuff1, nBuffLen1)) {
					Sched[i].Enabled[j] = cBuff1[0] != '0';
				}
				else return false;

				if (ini.getValue(cBuff2, "time", cBuff1, nBuffLen1)) {
					chArrToByteArr(cBuff1, Sched[i].Time[j]);
				}
				else return false;

				if (ini.getValue(cBuff2, "from", cBuff1, nBuffLen1)) {
					Sched[i].Value[j][0] = atof(cBuff1);
				}
				else return false;

				if (ini.getValue(cBuff2, "to", cBuff1, nBuffLen1)) {
					Sched[i].Value[j][1] = atof(cBuff1);
				}
				else return false;
			}
		}
	}
	return true;
}

/// <summary>
/// Deletes scheduler settings from SD card.
/// </summary>
/// <param name="relay">Specified relay</param>

void deleteSDSched(int relay) {
	String _sPath = "/settings/sch_r" + intToString(relay + 1) + ".ini";
	_sPath.toCharArray(cBuff1, nBuffLen1);
	SD.remove(cBuff1);
}