struct DataSet
{
	float Data[9] = {0};
	byte Size;
	boolean Valid;
	String APIkey;
	DateTime Timestamp;
}; typedef struct DataSet DataSet;

struct RelayScheduler
{	
	float CurrentValue[2];
	byte CurrentInterval=0;  //this is pointer to arrays Alarm, Value, Enabled
	int Alarm[5];
	
	byte Variable;
	boolean Enabled[5];
	byte Time[5][2]; //5x time of day - consists of two values h:m
	float Value[5][2]; //5x pair of valuemin and valuemax

}; typedef struct RelayScheduler RelayScheduler;
