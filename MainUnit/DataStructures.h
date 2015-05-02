struct DataSet
{
	float Data[9] = { 0 };
	byte Size;
	bool Valid;
	String APIkey;
	DateTime Timestamp;
}; typedef struct DataSet DataSet;

struct RelayScheduler
{
	byte CurrentInterval = 0;  //this is pointer to arrays Alarm, Value, Enabled

	byte Variable = 0;
	bool Enabled[5] = { false };
	byte Time[5][2] = { { 0 }, { 0 } }; //5x time of day - consists of two values h:m
	float Value[5][2] = { { 0 }, { 0 } }; //5x pair of valuemin and valuemax

}; typedef struct RelayScheduler RelayScheduler;
