// PAGE_IoT_Configuration.cpp

#include "PAGE_IoT_Configuration.h"
//
//  SEND HTML PAGE OR IF A FORM SUMBITTED VALUES, PROCESS THESE VALUES
// 

void send_iot_configuration_html() {
	if (web_server.args() > 0 ) { // Save Settings
		String temp = "";
		for ( uint8_t i = 0; i < web_server.args(); i++ ) {
      if (web_server.argName(i) == "IoT_Host") ESP_Config.IoT_Host =   urldecode(web_server.arg(i));  
      if (web_server.argName(i) == "IoT_Port") ESP_Config.IoT_Port =  web_server.arg(i).toInt();         
      if (web_server.argName(i) == "IoT_Login") ESP_Config.IoT_Login =   urldecode(web_server.arg(i));
      if (web_server.argName(i) == "IoT_Key") ESP_Config.IoT_Key =   urldecode(web_server.arg(i));
      if (web_server.argName(i) == "feed_1") ESP_Config.feed_1 =   urldecode(web_server.arg(i));
      if (web_server.argName(i) == "feed_2") ESP_Config.feed_2 =   urldecode(web_server.arg(i));
      if (web_server.argName(i) == "feed_3") ESP_Config.feed_3 =   urldecode(web_server.arg(i));
      if (web_server.argName(i) == "feed_4") ESP_Config.feed_4 =   urldecode(web_server.arg(i));
      if (web_server.argName(i) == "feed_5") ESP_Config.feed_5 =   urldecode(web_server.arg(i));
      if (web_server.argName(i) == "feed_6") ESP_Config.feed_6 =   urldecode(web_server.arg(i));
		  } ;

		web_server.send ( 200, "text/html", PAGE_IoT_WaitAndReload );
		WriteConfig();
		AdminTimeOutCounter=0;
   	}
	else {
		web_server.send ( 200, "text/html", PAGE_IoT_Configuration ); 
	  } ;
  if(WebConfig_debug)  debugA("%s",__FUNCTION__); 
}


//
//   FILL THE PAGE WITH VALUES
//

void send_iot_configuration_values_html() {
	String IoT_values ="";
  IoT_values += "IoT_Host|" + (String) ESP_Config.IoT_Host + "|input\n";
  IoT_values += "IoT_Port|" +  (String) ESP_Config.IoT_Port + "|input\n";
  IoT_values += "IoT_Login|" + (String) ESP_Config.IoT_Login + "|input\n";
  IoT_values += "IoT_Key|" +  (String) ESP_Config.IoT_Key + "|input\n";
  IoT_values += "feed_1|" +  (String) ESP_Config.feed_1 + "|input\n";
  IoT_values += "feed_2|" +  (String) ESP_Config.feed_2 + "|input\n";
  IoT_values += "feed_3|" +  (String) ESP_Config.feed_3 + "|input\n";
  IoT_values += "feed_4|" +  (String) ESP_Config.feed_4 + "|input\n";
  IoT_values += "feed_5|" +  (String) ESP_Config.feed_5 + "|input\n";
  IoT_values += "feed_6|" +  (String) ESP_Config.feed_6 + "|input\n";
  
	web_server.send ( 200, "text/plain", IoT_values);
  if(WebConfig_debug)  debugA("%s",__FUNCTION__); 
  return;
}


