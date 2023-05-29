// PAGE_PayloadEncapConfiguration.cpp

#include "PAGE_PayloadEncapConfiguration.h"



//
//  SEND HTML PAGE OR IF A FORM SUMBITTED VALUES, PROCESS THESE VALUES
// 

void send_encap_configuration_html() {
	if (web_server.args() > 0 ) { // Save Settings
		for ( uint8_t i = 0; i < web_server.args(); i++ ) {
      if (web_server.argName(i) == "EncapType") ESP_Config.EncapType =  web_server.arg(i).toInt();
      if (web_server.argName(i) == "PayloadStyle") ESP_Config.PayloadStyle =  web_server.arg(i).toInt();
      if (web_server.argName(i) == "RepeaterOperation") ESP_Config.RepeaterOperation =  web_server.arg(i).toInt();
      if (web_server.argName(i) == "LocationCompression") ESP_Config.LocationCompression =  web_server.arg(i).toInt();
       if (web_server.argName(i) == "BlackList") ESP_Config.BlackList =  web_server.arg(i).toInt();
   
      if (web_server.argName(i) == "AgileBeaconing") ESP_Config.AgileBeaconing =  web_server.arg(i).toInt();
    
      if (web_server.argName(i) == "LoraSync") ESP_Config.LoraSync =  web_server.arg(i).toInt();
		  } ;

		web_server.send ( 200, "text/html", PAGE_EncapWaitAndReload );
		WriteConfig();
		AdminTimeOutCounter=0;
    EncapType = ESP_Config.EncapType;
    PayloadStyle = ESP_Config.PayloadStyle;
    RepeaterOperation = ESP_Config.RepeaterOperation;
    LocationCompression = ESP_Config.LocationCompression;
    BlackList = ESP_Config.BlackList;

    AgileBeaconing = ESP_Config.AgileBeaconing;

    LoraSync = ESP_Config.LoraSync;
	  }
	else{
		web_server.send ( 200, "text/html", PAGE_PayloadEncapConfiguration ); 
	  };
  if(WebConfig_debug)  debugA("%s",__FUNCTION__); 
}


void send_encap_configuration_html_kiss()
{
  
  if (web_server.args() > 0 )  // Save Settings
  {
    for ( uint8_t i = 0; i < web_server.args(); i++ ) {
     if (web_server.argName(i) == "EncapType") ESP_Config.EncapType =  web_server.arg(i).toInt();
     if (web_server.argName(i) == "PayloadStyle") ESP_Config.PayloadStyle =  web_server.arg(i).toInt();
     if (web_server.argName(i) == "RepeaterOperation") ESP_Config.RepeaterOperation =  web_server.arg(i).toInt();
     if (web_server.argName(i) == "LocationCompression") ESP_Config.LocationCompression =  web_server.arg(i).toInt();
     if (web_server.argName(i) == "BlackList") ESP_Config.BlackList =  web_server.arg(i).toInt();

     if (web_server.argName(i) == "AgileBeaconing") ESP_Config.AgileBeaconing =  web_server.arg(i).toInt();


     if (web_server.argName(i) == "LoraSync") ESP_Config.LoraSync =  web_server.arg(i).toInt();
    }
     web_server.send ( 200, "text/html", PAGE_EncapWaitAndReload );
    WriteConfig();
    AdminTimeOutCounter=0;
    EncapType = ESP_Config.EncapType;
    PayloadStyle = ESP_Config.PayloadStyle;
    RepeaterOperation = ESP_Config.RepeaterOperation;
    LocationCompression = ESP_Config.LocationCompression;
    BlackList = ESP_Config.BlackList;
   
    AgileBeaconing = ESP_Config.AgileBeaconing;
  
    LoraSync = ESP_Config.LoraSync;
    }
  else{
    web_server.send ( 200, "text/html", PAGE_PayloadEncapConfiguration_kiss ); 
    };
   if(WebConfig_debug)  debugA("%s",__FUNCTION__); 
}


//
//   FILL THE PAGE WITH VALUES
//

void send_encap_configuration_values_html()
{
	String Encapvalues ="";
  Encapvalues += "EncapType|" +  (String) ESP_Config.EncapType + "|input\n";
  Encapvalues += "PayloadStyle|" +  (String) ESP_Config.PayloadStyle + "|input\n";
  Encapvalues += "RepeaterOperation|" +  (String) ESP_Config.RepeaterOperation + "|input\n";
  Encapvalues += "LocationCompression|" +  (String) ESP_Config.LocationCompression + "|input\n";
   Encapvalues += "BlackList|" +  (String) ESP_Config.BlackList + "|input\n";
 
  Encapvalues += "AgileBeaconing|" +  (String) ESP_Config.AgileBeaconing + "|input\n";
 
  Encapvalues += "LoraSync|" + (String) ESP_Config.LoraSync + "|input\n";

	web_server.send ( 200, "text/plain", Encapvalues);
   if(WebConfig_debug)  debugA("%s",__FUNCTION__); 
}
