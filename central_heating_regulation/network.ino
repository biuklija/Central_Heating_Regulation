#if NETWORK
void webServer(void)
{
	word len = ether.packetReceive();
	word pos = ether.packetLoop(len);

	if (pos) { // check if valid tcp data is received
		// data received from ethernet
		char* data = (char *) Ethernet::buffer + pos;

		#if DEBUG       // display incoming data    
		goOne();
		Serial.print(data);
		#endif

		// "on" command received     
		if (strncmp( "GET /p" , data , 6 ) == 0) { // 6 - query length
			pumpON = 1;  
			regulationON = 0;
			pumpButtonStateChange = millis();
			buttonRegulationStateChange = millis();
			pumpMode();
			regulationStateOnLCD();
			#if DEBUG
			goTwo();
			Serial.print("on received");
			#endif
		}

		// "off" command received     
		if (strncmp( "GET /off" , data , 8 ) == 0) {
			pumpON = 0;  
			regulationON = 0;
			pumpButtonStateChange = millis();
			buttonRegulationStateChange = millis();
			pumpMode();
			regulationStateOnLCD();
			#if DEBUG
			goTwo;
			Serial.print("off received");
			#endif
		}

		// "impulse" command received     
		if (strncmp( "GET /i" , data , 6 ) == 0) {
			pumpON = 2;
			regulationON = 0;
			pumpButtonStateChange = millis();
			buttonRegulationStateChange = millis();
			pumpMode();
			regulationStateOnLCD();
			#if DEBUG
			goTwo;
			Serial.print("imp received");
			#endif
		}
		if (strncmp( "GET /pr" , data , 7 ) == 0) {
			pumpON = 1;
			regulationON = 1;
			pumpButtonStateChange = millis();
			buttonRegulationStateChange = millis();
			pumpMode();
			regulationStateOnLCD();
			#if DEBUG
			goTwo;
			Serial.print("int received");
			#endif
		}
		if (strncmp( "GET /ir" , data , 7 ) == 0) {
			pumpON = 2;
			regulationON = 1;
			pumpButtonStateChange = millis();
			buttonRegulationStateChange = millis();
			pumpMode();
			regulationStateOnLCD();
			#if DEBUG
			goTwo;
			Serial.print("int received");
			#endif
		}
			if (strncmp( "GET /is" , data , 7 ) == 0) {
			pumpON = 2;
			regulationON = 2;
			pumpButtonStateChange = millis();
			buttonRegulationStateChange = millis();
			pumpMode();
			regulationStateOnLCD();
			#if DEBUG
			goTwo;
			Serial.print("int received");
			#endif
		}
			if (strncmp( "GET /ps" , data , 7 ) == 0) {
			pumpON = 1;
			regulationON = 2;
			pumpButtonStateChange = millis();
			buttonRegulationStateChange = millis();
			pumpMode();
			regulationStateOnLCD();
			#if DEBUG
			goTwo;
			Serial.print("int received");
			#endif
		}
		ether.httpServerReply(homePage()); // send web page data
	}
}

void logTemperatures(void)
{
	// clear out the stash if subsequent logging attempts fail 
	if (stash.freeCount() <= 3) {
		Stash::initMap(56);
	}
	byte sd = stash.create(); 
	int valve = EEPROM.read(0); 
	int actualPumpState = 0;
	int actualRegulationState = 0;
	if (digitalRead(RELAY_PUMP) == HIGH) // we want to be able to see all changes on a line chart
	{
		actualPumpState = pumpON + 3; 
	}
	else
	{
		actualPumpState = pumpON; 
	}

	if (digitalRead(SERVO_ON) == HIGH)
	{
		actualRegulationState = regulationON;
	}
	else
	{
		actualRegulationState = regulationON;
	}
	for (int i=0; i < NUMBER_OF_SENSORS; i++)
	{
		stash.print("&t");
		stash.print(i);
		stash.print("=");
		stash.print(temperature[i]);
	}
	stash.print("&info=");
	stash.print(actualRegulationState);
	stash.print("&pumpa=");
	stash.print(actualPumpState);
	stash.print("&ventil=");
	stash.print(valve);
	stash.save();
	Stash::prepare(PSTR("GET http://$F/tlog/post.php?$H HTTP/1.0" "\r\n"
	"Content-Length: $D" "\r\n"
	"User-Agent: Arduino RedBox" "\r\n" 
	"\r\n"
	"$H"), 
	website, sd, stash.size(), website);
	ether.tcpSend();
	lastLogged = millis(); 
}
#endif
