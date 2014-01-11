/* 
Temperature monitoring and regulation in a central heating system,
using a wood-fired boiler, accumulation tank, heat charger, circulator 
pump and a motorized 3-way mixing valve. 
Logging and monitoring is done using a 20x04 LCD and logging to a web
server. Remote control via ethernet also included. Probably useless 
to anyone but the original author, although perhaps someone might find
bits of code useful for their purpose. 

Code by Ivan Biuklija. Parts of code by original authors, whoever they are. 

*/


#define DEBUG 0 // All these switches are for debugging purposes only. Tamper at your own peril.
#define NETWORK 1 // Enable/disable the ethernet module
#define RTC_CLOCK 0 // Enable the RTC which you probably don't need
#define REGULATION 1 //
#define EEPROM_BACKUP 1 // Regulation and this go together, as they're related to the mixing valve actuator. 
#define SET_TIME 0 // Reset the RTC time on next load


#include <SPI.h>
#include <OneWire.h>
#include <DallasTemperature.h>
#include <Bounce.h>
#if NETWORK
#include <EtherCard.h>
#endif
#if EEPROM_BACKUP
#include <EEPROM.h>
#endif

#if RTC_CLOCK
#include <Wire.h>
#include "RTClib.h"
#endif


// Port configuration
#define P_BUTTON 6 // "Pump" button
#define R_BUTTON 7 // "Regulation" button
#define RELAY_PUMP 3 // Relay that turns the circulator on/off
#define SERVO_ON 4 // Relay that powers the 3WV actuator
#define SERVO_DIRECTION 5 // Relay that sets the 3WV actuator direction

// OneWire bus settings 
#define ONE_WIRE_BUS 2
#define TEMPERATURE_PRECISION 12
OneWire oneWire(ONE_WIRE_BUS);
DallasTemperature sensors(&oneWire);

// RTC init
#if RTC_CLOCK
RTC_DS1307 RTC;
#endif

// manually enter the number of available DS18B20 sensors
#define NUMBER_OF_SENSORS 8
// Adrese termometara
static byte adresses[64] = { // multiply rows by 8 to define the array size
	0x28, 0xB1, 0xDF, 0x84, 0x02, 0x00, 0x00, 0x87, // fill this in with your data, please 
	0x28, 0x90, 0xB2, 0x84, 0x02, 0x00, 0x00, 0xFB, // these are my examples
	0x28, 0x67, 0xCF, 0x84, 0x02, 0x00, 0x00, 0x8D, // where I've used fixed addresses
	0x28, 0x41, 0xDE, 0x84, 0x02, 0x00, 0x00, 0x38, // for the eight sensors installed. 
	0x28, 0xF1, 0xC4, 0x84, 0x02, 0x00, 0x00, 0xFE, 
	0x28, 0x4F, 0xAD, 0x84, 0x02, 0x00, 0x00, 0x08,  
	0x28, 0xA0, 0x9B, 0x84, 0x02, 0x00, 0x00, 0x1D, 
0x28, 0xBB, 0xA1, 0x84, 0x02, 0x00, 0x00, 0x87};

#if NETWORK
//Network settings - here you can set the IP for your ethernet shield and your server address 
static byte mymac[] = { 0x74,0x69,0x69,0x2D,0xFA,0xAA };
static byte myip[] = { 10,0,0,200 }; 
static byte gwip[] = { 10,0,0,1 }; // Gateway IP if you're logging outside of your LAN
static byte hisip[] = { 10,0,0,100 }; // IP of the server you're logging to
byte Ethernet::buffer[600];
char website[] PROGMEM = "10.0.0.100"; // or the hostname of the server you're logging to
Stash stash;
#endif

// store html header in flash to save memory
#if NETWORK
BufferFiller bfill;
char htmlHeader[] PROGMEM =
"HTTP/1.0 503 test page\r\n"
"Content-Type: text/html\r\n"
"Retry-After: 600\r\n\r\n"
"<html>"
"<head>"
"</head>"; 
#endif

// Temperature data will be stored in this array
float temperature[NUMBER_OF_SENSORS];

// How often will we read temperature data from sensors
unsigned long lastTemperatureRead = -2000; // We want to read the sensors as soon as possible after a reset 
#define TEMP_READ_INTERVAL 3241 // Usually we will read the sensors after this period. 

// How often will we log the data to the server? 
unsigned long lastLogged = -5000; // this is to speed up the first instance of logging after a reset
#define LOGGING_INTERVAL 15000 // 15 seconds

// Circulator pump variables
byte pumpON = 0;
Bounce buttonPump = Bounce(P_BUTTON,5); // debounce 
unsigned long pumpButtonStateChange; // We need to know when the pump button was last pressed, to delay turning it on/off 
#define DELAY_PUMP_BUTTON 5000 // How long to delay turning the circulator on? 

#if REGULATION
// variables related to regulation 
byte regulationON = 0;
unsigned long lastRegulation = -30000;
#define INTERVAL_REG 40000 // Interval between temperature checks
Bounce buttonRegulation = Bounce(R_BUTTON,5);
unsigned long buttonRegulationStateChange;
#endif

// variable related to pulse operation of the circulator pump
#define PUMP_RUNS 30002 // how long should the circulator pump run?
#define PUMP_STOPS  61232 // how long should the circulator pause? 
unsigned long pumpChangeState; // when did the circulator change state?

// variables for the 3WV actuator
#define STEP_CLOSE 15000ul // duration of a single step while closing the valve in ms 
#define STEP_OPEN 14400ul // duration of a single step while opening the valve in ms 
byte servoPosition; // position of the actuator
unsigned long stopServoAt; // when to switch off the actuator


#if NETWORK
static word homePage() { // homepage to be served on requests. Returns system state variables
	bfill = ether.tcpOffset();
	bfill.emit_p( PSTR (
	"<body>$F<p><em>$D,$D,$D"    // $F = htmlheader in flash memory; $D for numbers, $S for text
	"</em></p></body></html>"
	) , htmlHeader , pumpON, regulationON, servoPosition ) ;
	return bfill.position();
}
#endif
static const char *mode_names[] = {"Off ", "On  ", "Puls", "Time", "Stat" }; // A pointer with names of circulator pump modes for the LCD. Some remain unused

void setup(void)
{
	Serial.begin(9600); // initialize the serial port with the LCD
	writecharLCD(); // Create custom chars in the LCD
	delay(200);
	clearLCD();
	sensors.begin();	// Initialize DS18B20 sensors
	sensors.setWaitForConversion(false); // Use async mode for temperature sensors without breaking the sketch 
	sensors.setResolution(TEMPERATURE_PRECISION);
	sensors.requestTemperatures(); // Initial request to the sensors, it takes them about 1 second to calculate the temperature at 12bit precision
	delay(200);
	#if DEBUG
	Serial.print(F("Sensors init"));
	#endif
	// define pins
	pinMode(P_BUTTON, INPUT_PULLUP); // The button is wired to GND, we're using the internal pullups 
	pinMode(R_BUTTON, INPUT_PULLUP); 
	pinMode(RELAY_PUMP, OUTPUT); 
	digitalWrite(RELAY_PUMP, LOW); // relays are LOW when off, HIGH when on. Check if it's true in your case
	pinMode(SERVO_DIRECTION, OUTPUT); 	// servo relays
	pinMode(SERVO_ON, OUTPUT);
	digitalWrite(SERVO_DIRECTION, LOW);
	digitalWrite(SERVO_ON, LOW);
	#if RTC_CLOCK
	// Intialize the DS1307
	Wire.begin();
	RTC.begin();
	if (! RTC.isrunning()) {RTC.adjust(DateTime(__DATE__, __TIME__));}// If RTC is inactive, initialize with compilation time
	#if SET_TIME
	RTC.adjust(DateTime(__DATE__, __TIME__));
	#endif
	#endif
	#if NETWORK
	ether.begin(sizeof Ethernet::buffer, mymac);
	#if DEBUG
	Serial.print( "NoETH");
	#endif
	ether.staticSetup(myip, gwip);
	ether.copyIp(ether.hisip, hisip);
	#if DEBUG
	ether.printIp("S:", ether.hisip);
	#endif
	while (ether.clientWaitingGw())
	ether.packetLoop(ether.packetReceive());
	#if DEBUG
	Serial.print("GW set");
	#endif
	#endif
	pumpMode(); // print out the pump state on the LCD
	#if EEPROM_BACKUP
	byte eepzero = EEPROM.read(0); // read position 0 from eeprom
	if (eepzero == 255) // if empty, set to 10
	{
		EEPROM.write(0,10);
		servoPosition = 10;
	}
	else
	{
		servoPosition = eepzero;
	}
	
	#endif
	goTo(34); 
	Serial.write(2); // print servo state, a custom char 
	Serial.print(servoPosition);
	#if REGULATION
	regulationStateOnLCD();
	#endif
}

void loop(void)
{
	#if NETWORK
	webServer(); // wait for packages 
	#endif
	readButtons(); // check if a button is pressed
	stopServo(); // stop the servo if necessary
	checkMode(); // checking conditions
	readAndPrintTemperatures(); //
	// Is it time to log data? 
	#if NETWORK
	if(millis() - lastLogged > LOGGING_INTERVAL)
	{
		logTemperatures();
	}
	#endif
	#if DEBUG
	goTo(71);
	Serial.print(millis());
	Serial.print(freeRam());
	#endif
}


int freeRam () {
	extern int __heap_start, *__brkval;
	int v;
	return (int) &v - (__brkval == 0 ? (int) &__heap_start : (int) __brkval);
}