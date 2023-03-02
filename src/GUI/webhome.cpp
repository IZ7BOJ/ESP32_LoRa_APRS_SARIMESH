// webhome.cpp

#include "webhome.h"

void filldynamicdata(){        
    String values ="";
    values += "mydynamicdata|" + (String) + "Powered by AJAX. Uptime(millis): " + (String) millis() + "|div\n";   // Build a string, like this:  ID|VALUE|TYPE
    web_server.send ( 200, "text/plain", values);   
}

void processHome(){        
  if (web_server.args() > 0 ) {  // Are there any POST/GET Fields ? 
    for ( uint8_t i = 0; i < web_server.args(); i++ ) {  // Iterate through the fields
      if (web_server.argName(i) == "firstname") {
        // Your processing for the transmitted form-variable 
        String fName = web_server.arg(i);
        };
      };
    } ;
  web_server.send ( 200, "text/html", PAGE_HOME  ); 
}


void send_home_html() {
  if (web_server.args() > 0 ) {    // Save Settings 
    String temp = "";
    for ( uint8_t i = 0; i < web_server.args(); i++ ) {
      if (web_server.argName(i) == "devicename") ESP_Config.DeviceName = urldecode(web_server.arg(i)); 
      if (web_server.argName(i) == "DeviceId")  ESP_Config.DeviceId   =  DeviceId; 
      // if (web_server.argName(i) == "SW_Revision") ESP_Config.SW_Revision   = urldecode(web_server.arg(i));     
      };
    WriteConfig();
    firstStart = true;
  }
  web_server.send ( 200, "text/html", PAGE_HOME ); 
  if(WebConfig_debug) Serial.println(__FUNCTION__); 
}

void send_example_configuration_values_html() {
  String examplevalues ="";
  examplevalues += "devicename|" +  (String)  ESP_Config.DeviceName +  "|input\n";
  examplevalues += "DeviceId|" +  DeviceId +  "|input\n";
  //  examplevalues += "SW_Revision|" +  (String)  ESP_Config.SW_Revision + "|input\n";
  examplevalues += "SW_Revision|" +  (String) SW_Revision + "|input\n";

  web_server.send ( 200, "text/plain", examplevalues);
  if(WebConfig_debug) Serial.println(__FUNCTION__); 
}

