 // PAGE_AprsConfiguration.cpp
#include "PAGE_AprsConfiguration.h"

//
//  SEND HTML PAGE OR IF A FORM SUMBITTED VALUES, PROCESS THESE VALUES
// 

void send_aprs_configuration_html()
{
	if (web_server.args() > 0 )  // Save Settings
	{
		String temp = "";
//		ESP_Config.dhcp = false;
		for ( uint8_t i = 0; i < web_server.args(); i++ ) {
      if (web_server.argName(i) == "Loc_Lat") ESP_Config.Loc_Lat =   urldecode(web_server.arg(i));    
      if (web_server.argName(i) == "Loc_Lon") ESP_Config.Loc_Lon =   urldecode(web_server.arg(i));    
      if (web_server.argName(i) == "AprsHost") ESP_Config.AprsHost =   urldecode(web_server.arg(i)); 
      if (web_server.argName(i) == "AprsLoggerAddr") ESP_Config.AprsLoggerAddr =   urldecode(web_server.arg(i)); 
      if (web_server.argName(i) == "AprsLogin"){ ESP_Config.AprsLogin =   urldecode(web_server.arg(i)); NodeName = ESP_Config.AprsLogin ;  };
      if (web_server.argName(i) == "AprsPass") ESP_Config.AprsPass =   urldecode(web_server.arg(i));
      if (web_server.argName(i) == "AprsFilter") ESP_Config.AprsFilter =   urldecode(web_server.arg(i));
      if (web_server.argName(i) == "AprsRawBeaconIGATE") ESP_Config.AprsRawBeaconIGATE =   urldecode(web_server.arg(i));
      if (web_server.argName(i) == "AprsRawBeaconTRACK") ESP_Config.AprsRawBeaconTRACK =   urldecode(web_server.arg(i));
      if (web_server.argName(i) == "AprsPort") ESP_Config.AprsPort =  web_server.arg(i).toInt();
      if (web_server.argName(i) == "AprsLoggerPort") ESP_Config.AprsLoggerPort =  web_server.arg(i).toInt();
      if (web_server.argName(i) == "AprsRawBeaconPeriodSecondsIGATE") ESP_Config.AprsRawBeaconPeriodSecondsIGATE =  web_server.arg(i).toInt();
      if (web_server.argName(i) == "AprsRawBeaconPeriodSecondsTRACK") ESP_Config.AprsRawBeaconPeriodSecondsTRACK =  web_server.arg(i).toInt();
		  } ;

		web_server.send ( 200, "text/html", PAGE_AprsWaitAndReload );
		WriteConfig();
		AdminTimeOutCounter=0;
	  }
	else {
		web_server.send ( 200, "text/html", PAGE_AprsConfiguration ); 
	  };
	if(WebConfig_debug) debugA("%s",__FUNCTION__); 
}

//
//   FILL THE PAGE WITH VALUES
//

void send_aprs_configuration_values_html() {
	String Aprsvalues ="";
  Aprsvalues += "Loc_Lat|" + (String) ESP_Config.Loc_Lat + "|input\n";
  Aprsvalues += "Loc_Lon|" + (String) ESP_Config.Loc_Lon + "|input\n";
  Aprsvalues += "AprsHost|" + (String) ESP_Config.AprsHost + "|input\n";
  Aprsvalues += "AprsPort|" +  (String) ESP_Config.AprsPort + "|input\n";
  Aprsvalues += "AprsLoggerAddr|" + (String) ESP_Config.AprsLoggerAddr + "|input\n";
  Aprsvalues += "AprsLoggerPort|" +  (String) ESP_Config.AprsLoggerPort + "|input\n";  
  Aprsvalues += "AprsLogin|" + (String) ESP_Config.AprsLogin + "|input\n";
  Aprsvalues += "AprsPass|" +  (String) ESP_Config.AprsPass + "|input\n";
  Aprsvalues += "AprsFilter|" + (String) ESP_Config.AprsFilter + "|input\n";
  Aprsvalues += "AprsRawBeaconIGATE|" +  (String) ESP_Config.AprsRawBeaconIGATE + "|input\n";
  Aprsvalues += "AprsRawBeaconPeriodSecondsIGATE|" +  (String) ESP_Config.AprsRawBeaconPeriodSecondsIGATE + "|input\n";
  Aprsvalues += "AprsRawBeaconTRACK|" +  (String) ESP_Config.AprsRawBeaconTRACK + "|input\n";
  Aprsvalues += "AprsRawBeaconPeriodSecondsTRACK|" +  (String) ESP_Config.AprsRawBeaconPeriodSecondsTRACK + "|input\n";

	web_server.send ( 200, "text/plain", Aprsvalues);
	if(WebConfig_debug)  debugA("%s",__FUNCTION__); 
}

