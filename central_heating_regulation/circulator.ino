void circulatorOff(void)
{
	if (millis() >=  pumpButtonStateChange + DELAY_PUMP_BUTTON && digitalRead(RELAY_PUMP) == HIGH) 
	{
		digitalWrite(RELAY_PUMP, LOW);
		goTo(19); 
		Serial.write(5); // print an X
		pumpMode();
	}
}

void pumpIsRunning(void)
{
	if (millis() >=  pumpButtonStateChange + DELAY_PUMP_BUTTON && digitalRead(RELAY_PUMP) == LOW) 
	{	
		digitalWrite(RELAY_PUMP, HIGH); // relay on
		goTo(19); 
		Serial.write(6); // print a check
		pumpMode();
	}
}

void pumpInPulseMode(void)
{	
	if (millis() >=  pumpButtonStateChange + DELAY_PUMP_BUTTON) 
	{	
		if (digitalRead(RELAY_PUMP) == LOW && millis() >= pumpChangeState + PUMP_STOPS) 
		{
			digitalWrite(RELAY_PUMP, HIGH);
			goTo(19);
			Serial.write(6);
			pumpChangeState = millis();
		}
		else if (digitalRead(RELAY_PUMP) == HIGH && millis() >= pumpChangeState + PUMP_RUNS) 
		{
			digitalWrite(RELAY_PUMP, LOW);
			goTo(19);
			Serial.write(5);
			pumpChangeState = millis();
		}
	pumpMode();
	}
}

void pumpMode(void)
{
	goTo(14);
	Serial.write(3); // custom char
	Serial.print(mode_names[pumpON]); 
}
