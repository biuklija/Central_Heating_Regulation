#if REGULATION
void openValve(void)
{			
	if (servoPosition <= 9 && digitalRead(SERVO_ON) == LOW)
	{
		stopServoAt = millis() + STEP_OPEN;
		digitalWrite(SERVO_DIRECTION,HIGH);
		digitalWrite(SERVO_ON, HIGH);
		servoPosition++;
		servoStatus(1);
                lastRegulation = millis();
	}
}

void closeValve(void)
{			
	if (servoPosition >= 1 && digitalRead(SERVO_ON) == LOW)
	{
		stopServoAt = millis() + STEP_CLOSE;
		digitalWrite(SERVO_DIRECTION,LOW);
		digitalWrite(SERVO_ON, HIGH);
		servoPosition--;
		servoStatus(2); 
                lastRegulation = millis();
	}
}
#endif

void stopServo(void)
{
	if (stopServoAt <= millis() && digitalRead(SERVO_ON) == HIGH)
	{
		digitalWrite(SERVO_DIRECTION,LOW);
		digitalWrite(SERVO_ON, LOW);
#if EEPROM_BACKUP
		byte staro = EEPROM.read(0);
		if (staro != servoPosition)
		{
			EEPROM.write(0,servoPosition);
		}
#endif
		servoStatus(0);
	}
}

void setServoTo(byte requestedPosition)
{
	if (requestedPosition != servoPosition && digitalRead(SERVO_ON) == LOW && millis() >= (buttonRegulationStateChange + DELAY_PUMP_BUTTON) && millis() >=  pumpButtonStateChange + DELAY_PUMP_BUTTON)
	{
		char numberOfSteps = requestedPosition - servoPosition; 
		if (numberOfSteps > 0) // if the number is positive or higher than current, open the valve and use X steps 
		{
			stopServoAt = millis() + (abs(numberOfSteps) * STEP_OPEN);
			digitalWrite(SERVO_DIRECTION, HIGH);
			digitalWrite(SERVO_ON, HIGH);
			servoPosition = servoPosition + numberOfSteps;
			servoStatus(1); // the servo is moving, we can't print out its position, just a small arrow to indicate that it's closing
		}
		else if (numberOfSteps < 0) 
		{
			stopServoAt = millis() + (abs(numberOfSteps)* STEP_CLOSE);
			digitalWrite(SERVO_DIRECTION, LOW);
			digitalWrite(SERVO_ON, HIGH);
			servoPosition = servoPosition - abs(numberOfSteps);
			servoStatus(2); 
		}
		else
		{
			servoStatus(0); 
		}
	}
}

void servoStatus(byte printOut) // Print out the servo position, false if we're allowed to do so, true if the servo is in operation
{
	goTo(34);
	Serial.write(2); // custom char 
	if (printOut == 1)
	{
		Serial.print(F("<<")); //  close valve
	}
	else if (printOut == 2)
        {
               Serial.print(F(">>")); // open valve
	}
        else 
        {
		if (servoPosition < 10) 
		{
			Serial.write(48); // print out a zero if less than 10
		}
		Serial.print(servoPosition);
	}
}
