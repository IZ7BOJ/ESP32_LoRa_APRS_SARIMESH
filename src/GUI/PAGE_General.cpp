// PAGE_General.cpp
#include "PAGE_General.h"



String get_device_id(void){
    String _apName = WiFi.macAddress();
    _apName = "ESP32-" + _apName ;
    _apName.replace(":","");
    return (_apName);
}


// Functions for this Page
void send_devicename_value_html() {
	String values ="";
	values += "devicename|" + (String) ESP_Config.DeviceName + "|div\n";
  	values += "DeviceId|" + (String) ESP_Config.DeviceId + "|div\n";
  	values += "cpu_type|" + (String) ESP_Config.cpu_type + "|div\n";  
	//  values += "SW_Revision|" + (String) ESP_Config.SW_Revision + "|div\n";

	web_server.send ( 200, "text/plain", values);
   if(WebConfig_debug)  debugA("%s",__FUNCTION__); 	
}

void send_general_html() {
	if (web_server.args() > 0 ) { // Save Settings
		ESP_Config.AutoTurnOn = false;
		ESP_Config.AutoTurnOff = false;
		String temp = "";
		for ( uint8_t i = 0; i < web_server.args(); i++ ) {
			if (web_server.argName(i) == "devicename") ESP_Config.DeviceName = urldecode(web_server.arg(i)); 
      		if (web_server.argName(i) == "DeviceId")  ESP_Config.DeviceId   = (String) get_device_id(); 
      		if (web_server.argName(i) == "cpu_type")  ESP_Config.cpu_type   = (String) get_device_id(); 
			// if (web_server.argName(i) == "SW_Revision") ESP_Config.SW_Revision = urldecode(web_server.arg(i));    
			// if (web_server.argName(i) == "DeviceId")  ESP_Config.DeviceId   = urldecode(web_server.arg(i)); 
			if (web_server.argName(i) == "tonenabled") ESP_Config.AutoTurnOn = true; 
			if (web_server.argName(i) == "toffenabled") ESP_Config.AutoTurnOff = true; 
			if (web_server.argName(i) == "tonhour") ESP_Config.TurnOnHour =  web_server.arg(i).toInt(); 
			if (web_server.argName(i) == "tonminute") ESP_Config.TurnOnMinute =  web_server.arg(i).toInt(); 
			if (web_server.argName(i) == "toffhour") ESP_Config.TurnOffHour =  web_server.arg(i).toInt(); 
			if (web_server.argName(i) == "toffminute") ESP_Config.TurnOffMinute =  web_server.arg(i).toInt(); 
		}

    bool save_c = save_config_enable ;
    save_config_enable = true ;
    WriteConfig();
    save_config_enable = save_c ;    
    firstStart = true;
	}
	web_server.send ( 200, "text/html", PAGE_AdminGeneralSettings ); 
	if(WebConfig_debug) Serial.println(__FUNCTION__); 
}

void send_general_configuration_values_html() {
	String values ="";
	values += "devicename|" +  (String)  ESP_Config.DeviceName +  "|input\n";
	//  values += "SW_Revision|" +  (String)  ESP_Config.SW_Revision +  "|input\n";
  	values += "SW_Revision|" +  (String)  SW_Revision +  "|input\n";
   	values += "DeviceId|" +  (String)  get_device_id() +  "|input\n";
   	values += "cpu_type|" +  (String)  ESP_Config.cpu_type +  "|input\n";
	values += "tonhour|" +  (String)  ESP_Config.TurnOnHour +  "|input\n";
	values += "tonminute|" +   (String) ESP_Config.TurnOnMinute +  "|input\n";
	values += "toffhour|" +  (String)  ESP_Config.TurnOffHour +  "|input\n";
	values += "toffminute|" +   (String)  ESP_Config.TurnOffMinute +  "|input\n";
	values += "toffenabled|" +  (String) (ESP_Config.AutoTurnOff ? "checked" : "") + "|chk\n";
	values += "tonenabled|" +  (String) (ESP_Config.AutoTurnOn ? "checked" : "") + "|chk\n";
	web_server.send ( 200, "text/plain", values);
	if(WebConfig_debug) Serial.println(__FUNCTION__); 
}

