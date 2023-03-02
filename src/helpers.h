#ifndef HELP1_H
#define HELP1_H

#include <Arduino.h>
#include <WiFi.h>

static const uint8_t monthDays[]={31,28,31,30,31,30,31,31,30,31,30,31}; 
#define LEAP_YEAR(Y) ( ((1970+Y)>0) && !((1970+Y)%4) && ( ((1970+Y)%100) || !((1970+Y)%400) ) )

struct  strDateTime
{
	byte hour;
	byte minute;
	byte second;
	int year;
	byte month;
	byte day;
	byte wday;

} ;

extern strDateTime DateTime;                // Global DateTime structure, will be refreshed every Second

bool summertime(int year, byte month, byte day, byte hour, byte tzHours);
bool checkRange(String Value) ;
void ConvertUnixTimeStamp( unsigned long TimeStamp, struct strDateTime* DateTime) ;
String GetMacAddress() ;
unsigned char h2int(char c) ;
String urldecode(String input) ;  // (based on https://code.google.com/p/avr-netino/) ;
 
#endif
