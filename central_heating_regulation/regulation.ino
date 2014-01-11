#if REGULATION
void regulate(void)
{
	if (millis () >= buttonRegulationStateChange + DELAY_PUMP_BUTTON && millis() >=  pumpButtonStateChange + DELAY_PUMP_BUTTON) // we'll start with the regulation a few seconds after it was switched on
	{
		if (millis() >= lastRegulation + INTERVAL_REG && digitalRead(SERVO_ON) == LOW && regulationON == 1) // time for regulation if the servo is not running
		{
			if ((temperature[4] * 100) > ((temperature[0] * 100) - 400)) // we're turning temperatures into integers to compare them
			{
				closeValve();
				lastRegulation = millis();
			}
			else if ((temperature[4] * 100) < ((temperature[0] * 100) - 700)) // these values are very random and should not be used by anyone
			{
				openValve();
				lastRegulation = millis();
				
			}
			
		}
	}
}

void regulationStateOnLCD(void)
{
	goTo(54);
	Serial.write((byte)0x00);
	if (regulationON == 2)
	{
		Serial.print(mode_names[4]); 
	}
	else
	{
		Serial.print(mode_names[regulationON]);
	}
}
#endif
