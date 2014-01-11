void readAndPrintTemperatures(void)
{
	if (millis() >= lastTemperatureRead + TEMP_READ_INTERVAL) 
	{
		byte position = 0; // auxiliary variable with the position of the array being loaded 
		byte address[8];
		for (byte i=0; i < NUMBER_OF_SENSORS; i++) // loop through 'rows' which equal the number of sensors
		{
			for (byte j=0; j < 8; j++) // individual bytes in an address
			{
				address[j] = adresses[position];
				position++; 
			}

			temperature[i] = sensors.getTempC(address); // store temperature data into position [i] of the array 
		}
		lastTemperatureRead = millis(); // time of the last temperature polling
		sensors.requestTemperatures(); // request temperatures for the next read
		for (byte i=0; i < 4; i++) // loop for the first four sensors
		{
			goTo(i * 20); // 0, 20, 40, 60 - beginnings of rows
			int tprint = temperature[i]*10; // remove decimals so we can fit more data onto the LCD
			Serial.print(tprint); // print temperatures
		}
		// A bit of drawing
		goTo(3);
		Serial.write(1);
		Serial.write(4);
		Serial.write(75);//ascii K
		Serial.write(4);
		goTo(23);
		Serial.write(1);
		Serial.write(7);
		Serial.write(79); //ascii O
		Serial.write(7);
		goTo(43);
		Serial.write(1);
		Serial.write(4);
		Serial.write(80); //ascii P
		Serial.write(4);
		goTo(63);
		Serial.write(1);
		Serial.write(4);
		Serial.write(80);//ascii P
		Serial.write(4);
		// print out other sensors 
		goTo(7);
		int tprintK = temperature[6]*10; 
		Serial.print(tprintK);
		goTo(27);
		int tprintO = temperature[4]*10; 
		Serial.print(tprintO);
		goTo(47);
		int tprintP1 = temperature[5]*10; 
		Serial.print(tprintP1);
		goTo(67);
		int tprintP2 = temperature[7]*10; 
		Serial.print(tprintP2);
	}
}
