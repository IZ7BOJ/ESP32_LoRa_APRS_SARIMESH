// helpers.cpp

#include "helpers.h"

//
// Summertime calculates the daylight saving for a given date.
//
// input parameters: "normal time" for year, month, day, hour and tzHours (0=UTC, 1=MEZ)

bool summertime(int year, byte month, byte day, byte hour, byte tzHours)
{
 if (month<3 || month>10) return false; // keine Sommerzeit in Jan, Feb, Nov, Dez
 if (month>3 && month<10) return true; // Sommerzeit in Apr, Mai, Jun, Jul, Aug, Sep
 if (month==3 && (hour + 24 * day)>=(1 + tzHours + 24*(31 - (5 * year /4 + 4) % 7)) || month==10 && (hour + 24 * day)<(1 + tzHours + 24*(31 - (5 * year /4 + 1) % 7)))
   return true;
 else
   return false;
}

//
// Check the Values is between 0-255
//
bool checkRange(String Value){
	if (Value.toInt() < 0 || Value.toInt() > 255){
		return false;
	 	}
	else {
		return true;
	 	};
}


void ConvertUnixTimeStamp( unsigned long TimeStamp, struct strDateTime* DateTime) {
	uint8_t year;
	uint8_t month, monthLength;
	uint32_t time;
	unsigned long days;
	time = (uint32_t)TimeStamp;
	DateTime->second = time % 60;
	time /= 60; // now it is minutes
	DateTime->minute = time % 60;
	time /= 60; // now it is hours
	DateTime->hour = time % 24;
	time /= 24; // now it is days
	DateTime->wday = ((time + 4) % 7) + 1;  // Sunday is day 1 
  
	year = 0;  
	days = 0;
	while((unsigned)(days += (LEAP_YEAR(year) ? 366 : 365)) <= time) {
		year++;
		};
	DateTime->year = year; // year is offset from 1970 
  
	days -= LEAP_YEAR(year) ? 366 : 365;
	time  -= days; // now it is days in this year, starting at 0
  	days=0;
	month=0;
	monthLength=0;
	for (month=0; month<12; month++) {
		if (month==1) { // february
		  	if (LEAP_YEAR(year)) {
				monthLength=29;
		  		} 
			else {
				monthLength=28;
		  		};
			} 
		else {
		  	monthLength = monthDays[month];
			};
    
		if (time >= monthLength) {
		  	time -= monthLength;
			} 
		else {
			break;
			};
	  	};
	DateTime->month = month + 1;  // jan is month 1  
	DateTime->day = time + 1;     // day of month
	DateTime->year += 1970;	 
}


	
String GetMacAddress() {
	uint8_t mac[6];
    char macStr[18] = {0};
	WiFi.macAddress(mac);
    sprintf(macStr, "%02X:%02X:%02X:%02X:%02X:%02X", mac[0],  mac[1], mac[2], mac[3], mac[4], mac[5]);
    return  String(macStr);
}

// convert a single hex digit character to its integer value (from https://code.google.com/p/avr-netino/)
unsigned char h2int(char c) {
    if (c >= '0' && c <='9'){
        return((unsigned char)c - '0');
    	};
    if (c >= 'a' && c <='f'){
        return((unsigned char)c - 'a' + 10);
    	};
    if (c >= 'A' && c <='F'){
        return((unsigned char)c - 'A' + 10);
    	};
    return(0);
}

String urldecode(String input) { // (based on https://code.google.com/p/avr-netino/)
	char c;
	String ret = "";
	 
	for(byte t=0;t<input.length();t++) {
		c = input[t];
		if (c == '+') c = ' ';
        if (c == '%') {
    	    t++;
     	   	c = input[t];
    	  	t++;
       	 	c = (h2int(c) << 4) | h2int(input[t]);
			};
		ret.concat(c);
		};
	return ret;
}


