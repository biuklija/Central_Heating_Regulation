Central_Heating_Regulation
==========================

Temperature monitoring and regulation in a central heating system,
using a wood-fired boiler, accumulation tank, heat charger, circulator 
pump and a motorized 3-way mixing valve. 

Logging and monitoring is done using a serial 20x04 LCD locally, and
logging to a web server remotely. Remote control via ethernet also included. 
Requirements and hardware: 
DS18B20 temperature sensors (require OneWire and DallasTemperature libraries)
DS1370 RTC (requires RTClib)
ENC28J60 ethernet module (requires EtherCard library)
Serial LCD adapter with a 20x04 LCD
Bounce library for button debouncing
EEPROM library for saving valve states between resets


Probably useless to anyone but the original author, although perhaps someone 
might find bits of code handy as a rough draft. 

Code by Ivan Biuklija. Bits of code sampled from various sources.  
