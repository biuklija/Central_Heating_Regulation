void readButtons()
{
	if (buttonPump.update() == true) { 
		if (buttonPump.read() == LOW) {
			if (pumpON == 2)
			{
				pumpON = 0; // set circulator pump state - 0=OFF, 1=ON, 2=PULSE
			}
			else 
			{
				pumpON++;
			}
			pumpButtonStateChange = millis(); // remember when we changed pump state, so we can delay that 
			pumpMode();
		}
	}
#if REGULATION	
	if (buttonRegulation.update() == true) { 
		if (buttonRegulation.read() == LOW) { 
		    if (regulationON == 2)
            {
                regulationON = 0;
            }
            else 
			{
				regulationON++;
			}
			buttonRegulationStateChange = millis();
			regulationStateOnLCD();
		}
	}
#endif
}
