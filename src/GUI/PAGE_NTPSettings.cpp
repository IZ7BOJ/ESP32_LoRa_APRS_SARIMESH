// PAGE_NTPSettings.cpp

#include "PAGE_NTPSettings.h"

void send_NTP_configuration_html() {
	if (web_server.args() > 0 ) { // Save Settings
		ESP_Config.daylight = false;
		String temp = "";
		for ( uint8_t i = 0; i < web_server.args(); i++ ) {
			if (web_server.argName(i) == "ntpServerName") ESP_Config.ntpServerName = urldecode( web_server.arg(i)); 
			// if (web_server.argName(i) == "update") ESP_Config.Update_Time_Via_NTP_Every =  web_server.arg(i).toInt(); 
			if (web_server.argName(i) == "tz") ESP_Config.timezone =  web_server.arg(i).toInt(); 
			if (web_server.argName(i) == "dst") ESP_Config.daylight = true; 
			} ;
		WriteConfig();
		firstStart = true;
		} ;
	web_server.send ( 200, "text/html", PAGE_NTPConfiguration ); 
   	if(WebConfig_debug)  debugA("%s",__FUNCTION__); 
}


void send_NTP_configuration_values_html() {
	String values ="";
	values += "ntpServerName|" + (String) ESP_Config.ntpServerName + "|input\n";
	//	values += "update|" +  (String) ESP_Config.Update_Time_Via_NTP_Every + "|input\n";
	values += "tz|" +  (String) ESP_Config.timezone + "|input\n";
	values += "dst|" +  (String) (ESP_Config.daylight ? "checked" : "") + "|chk\n";

	web_server.send ( 200, "text/plain", values);
   	if(WebConfig_debug)  debugA("%s",__FUNCTION__); 
}

