#if RTC_CLOCK
void printTime()
{
	DateTime now = RTC.now();  
	goTo(71);
	if (now.hour() < 10) {Serial.write(48);} // nula
	Serial.print(now.hour());
	Serial.write(58); // :
	if (now.minute() < 10) {Serial.write(48);}
	Serial.print(now.minute());
	Serial.write(58);
	if (now.second() < 10) {Serial.write(48);}
	Serial.print(now.second());
}
#endif
