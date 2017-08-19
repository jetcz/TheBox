/// <summary>
/// structure holding data for scheduling relays
/// </summary>
struct RelayScheduler
{
	byte CurrentInterval = 0;  //this is pointer to arrays enabled, time, value
	byte Variable = 0;	//target variable
	bool Enabled[5] = { false }; //interval enabled?
	byte Time[5][2] = { { 0 },{ 0 } }; //5x time of day - consists of two values h:m
	float Value[5][2] = { { 0 },{ 0 } }; //5x pair of valuemin and valuemax

	void setDefault()
	{
		CurrentInterval = 0;
		Variable = 0;
		for (int i = 0; i < 5; i++)
		{
			Enabled[i] = false;
			Time[i][0] = 0;
			Time[i][1] = 0;
			Value[i][0] = 0;
			Value[i][1] = 0;
		}
	}
}; typedef struct RelayScheduler RelayScheduler;