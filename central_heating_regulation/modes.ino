void checkMode(void)
{
	if (pumpON == 0) // if the circulator is completely off, servo is fully open
	{
		circulatorOff();
		setServoTo(10);
	}
	else
	{
		if ((temperature[0] * 100) < 5900 || (temperature[6] * 100) < 5950 ) // lower temperatures - leave the circulator off, although its state is "on" or "pulse"
		{
			circulatorOff();
			setServoTo(10);
		}
		else if ((temperature[0]* 100) >= 5970) // the top of the tank is at 60 degrees 
		{
			switch (regulationON)
			{
			case 1: 
				regulate(); // regulate
				if (pumpON == 1) 
				{
					pumpIsRunning();
				}
				else
				{
					pumpInPulseMode();
				}
				break; 
			case 2:  //static
				if (pumpON == 1)
				{
					pumpIsRunning();
					setServoTo(4);
				}
				else if (pumpON == 2)
				{
					pumpInPulseMode();
					setServoTo(6);
				}
				break; 
			case 0:  // regulation off, leave the servo fully open
				if (pumpON == 1)
				{
					pumpIsRunning();
					setServoTo(10);
				}
				else if (pumpON == 2)
				{
					pumpInPulseMode();
					setServoTo(10);
				}
				break;
			}
		}	
	}
}
