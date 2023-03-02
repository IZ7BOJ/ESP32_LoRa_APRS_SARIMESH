// PAGE_DebugConfiguration.cpp
#include "PAGE_DebugConfiguration.h"

//
//  SEND HTML PAGE OR IF A FORM SUMBITTED VALUES, PROCESS THESE VALUES
//
//  // force_config_init= true ; // to test forcing default configuration
//  if (LoadConfig( force_config_init ) == false) Serial.print("Failed loading Device configurations: forced default configuration\r\n");
 
bool dummy1 = true ;

void send_debug_configuration_html() {
	if (web_server.args() > 0 ) {  // Save Settings
    bool BT_Mode_old = ESP_Config.BT_KISS_Mode ;
    bool TCP_Mode_old = ESP_Config.TCP_KISS_Mode ;
    bool Serial_Mode_old = ESP_Config.Serial_KISS_Mode ;    
    bool Admin_Mode_old = ESP_Config.Admin_Mode ;
		String temp = "";
    ESP_Config.gps_debug = false ;
    ESP_Config.LoRa_debug = false;
    ESP_Config.APRS_debug = false;
    ESP_Config.RTC_debug = false;
    ESP_Config.ezTime_debug = false;
    ESP_Config.pps_debug = false;
    ESP_Config.PE_debug = false;
    ESP_Config.BT_KISS_Mode = false;
    ESP_Config.TCP_KISS_Mode = false;
    ESP_Config.Serial_KISS_Mode = false;
    ESP_Config.Tracker_Mode = false;
    ESP_Config.IoT_enable = false;
    ESP_Config.mqtt_enable = false;
    ESP_Config.WebConfig_debug = false;
    ESP_Config.standalone = false;
    ESP_Config.syslog_enable = false;
    ESP_Config.no_gps = false;  
    ESP_Config.iGate_Mode = false;
    ESP_Config.Beacon_Mode = false;
    ESP_Config.Admin_Mode = false;
    ESP_Config.Reboot_Now = false;
    force_config_init = 0 ;
    
		for ( uint8_t i = 0; i < web_server.args(); i++ ) {
      if (web_server.argName(i) == "gps_debug") ESP_Config.gps_debug = true;
      if (web_server.argName(i) == "LoRa_debug") ESP_Config.LoRa_debug = true;
      if (web_server.argName(i) == "APRS_debug") ESP_Config.APRS_debug = true;
      if (web_server.argName(i) == "RTC_debug") ESP_Config.RTC_debug = true;
      if (web_server.argName(i) == "ezTime_debug") ESP_Config.ezTime_debug = true;
      if (web_server.argName(i) == "pps_debug") ESP_Config.pps_debug = true;
      if (web_server.argName(i) == "PE_debug") ESP_Config.PE_debug = true;
      // if (web_server.argName(i) == "BT_KISS_Mode") ESP_Config.BT_KISS_Mode = true;
      if (web_server.argName(i) == "TCP_KISS_Mode") ESP_Config.TCP_KISS_Mode = true;
      if (web_server.argName(i) == "Serial_KISS_Mode") ESP_Config.Serial_KISS_Mode = true;
      if (web_server.argName(i) == "Tracker_Mode") ESP_Config.Tracker_Mode = true;
      if (web_server.argName(i) == "IoT_enable") ESP_Config.IoT_enable = true;
      if (web_server.argName(i) == "mqtt_enable") ESP_Config.mqtt_enable = true;
      if (web_server.argName(i) == "WebConfig_debug") ESP_Config.WebConfig_debug = true;
      if (web_server.argName(i) == "standalone") ESP_Config.standalone = true;
      if (web_server.argName(i) == "syslog_enable") ESP_Config.syslog_enable = true;
      if (web_server.argName(i) == "no_gps") ESP_Config.no_gps = true;
      if (web_server.argName(i) == "dummy1") dummy1 = true;
      if (web_server.argName(i) == "iGate_Mode") ESP_Config.iGate_Mode = true;
      if (web_server.argName(i) == "Beacon_Mode") ESP_Config.Beacon_Mode = true;
      if (web_server.argName(i) == "Admin_Mode") ESP_Config.Admin_Mode = true;
      if (web_server.argName(i) == "Reboot_Now") ESP_Config.Reboot_Now = true;
      if (web_server.argName(i) == "Load_Default_Conf") force_config_init = true;
		  } ;

   // consistency checks

    if(ESP_Config.iGate_Mode) {
       ESP_Config.BT_KISS_Mode = false;
       ESP_Config.TCP_KISS_Mode = false;
       ESP_Config.Serial_KISS_Mode = false;
       ESP_Config.Tracker_Mode = false;       
       ESP_Config.EnableRfToIs = true;
       ESP_Config.EnableIsToRf = true;
       // ESP_Config.EnableRfToIs = false;
       // ESP_Config.EnableIsToRf = false;
       ESP_Config.EnableRepeater = true;
       }
    else{
       ESP_Config.EnableRfToIs = false;
       ESP_Config.EnableIsToRf = false;
       ESP_Config.EnableRepeater = false;
       ESP_Config.Tracker_Mode = true;          
       };

    if(ESP_Config.TCP_KISS_Mode) {
       ESP_Config.EnableRfToIs = false;
       ESP_Config.EnableIsToRf = false;
       ESP_Config.EnableRepeater = false;
       ESP_Config.Tracker_Mode = false;       
       // ESP_Config.no_gps = true;
       };
       
    if(ESP_Config.BT_KISS_Mode) {
       ESP_Config.EnableRfToIs = false;
       ESP_Config.EnableIsToRf = false;
       ESP_Config.EnableRepeater = false;
       ESP_Config.Tracker_Mode = false;       
       //   ESP_Config.no_gps = true;
       };

    if(ESP_Config.Serial_KISS_Mode) {
       ESP_Config.EnableRfToIs = false;
       ESP_Config.EnableIsToRf = false;
       ESP_Config.EnableRepeater = false; 
       ESP_Config.BT_KISS_Mode = false;
       // ESP_Config.no_gps = true;
       ESP_Config.Tracker_Mode = false;              
       };


    if(force_config_init) {    // This is just to load default device configuration  
       write_event_to_log("========== Load Default Configuration by GUI =============");
       if (LoadConfig( force_config_init ) == false) Serial.print("Forced default configuration\r\n");
       debugA(">>>>>>>>>>> Now Rebooting... \r");
       web_server.send ( 200, "text/html", PAGE_Home );
       vTaskDelay(pdMS_TO_TICKS(5000));
       if(reboot_pin){
          hard_reboot();   
          }
       else{
         ESP.restart();
        };      
      };

    if(ESP_Config.Reboot_Now) {    // This is just to hard reboot ESP32
       write_event_to_log("========== system reboot by GUI =============");
       debugA(">>>>>>>>>>> Now Rebooting... \r");
       web_server.send ( 200, "text/html", PAGE_Home );
       vTaskDelay(pdMS_TO_TICKS(5000));
       if(reboot_pin){
          hard_reboot();  
          }
       else{
         ESP.restart();
        };      
      };

		web_server.send ( 200, "text/html", PAGE_DebugWaitAndReload );
	  WriteConfig();

    if( (ESP_Config.TCP_KISS_Mode && ! TCP_Mode_old  )  || (ESP_Config.BT_KISS_Mode && ! BT_Mode_old  )  || ( ! ESP_Config.BT_KISS_Mode &&  BT_Mode_old ) || (ESP_Config.Serial_KISS_Mode && ! Serial_Mode_old )  || ( ! ESP_Config.Serial_KISS_Mode &&  Serial_Mode_old ) || (ESP_Config.Admin_Mode && ! Admin_Mode_old ) || ( ! ESP_Config.Admin_Mode &&  Admin_Mode_old ) ){
       debugA("===> ending aux serial interface...\r");
       web_server.send ( 200, "text/html", PAGE_Home );
       write_event_to_log("========== system reboot by GUI =============");
       debugA(">>>>>>>>>>> Rebooting in 5 secs... \r");
       vTaskDelay(pdMS_TO_TICKS(5000));
       WiFi.softAPdisconnect(true);
       WiFi.disconnect(true);
       debugA(">>>>>>>>>>> Now Rebooting... \r");  
       vTaskDelay(pdMS_TO_TICKS(5000)); 
       if(reboot_pin){
           hard_reboot();   // force reboot we are entering BT_KISS mod
           }
       else{
           ESP.restart();
           };         
       };
       
		AdminTimeOutCounter=0;
	  }
	else{
		web_server.send ( 200, "text/html", PAGE_DebugConfiguration );
 	  };
	if(WebConfig_debug) debugA("%s\r",__FUNCTION__); 
}



//
//   FILL THE PAGE WITH VALUES
//

void send_debug_configuration_values_html() {
	String debugvalues ="";
	debugvalues += "gps_debug|" + (String) (ESP_Config.gps_debug ? "checked" : "") + "|chk\n";
  debugvalues += "LoRa_debug|" + (String) (ESP_Config.LoRa_debug ? "checked" : "") + "|chk\n";
  debugvalues += "APRS_debug|" + (String) (ESP_Config.APRS_debug ? "checked" : "") + "|chk\n";
  debugvalues += "RTC_debug|" + (String) (ESP_Config.RTC_debug ? "checked" : "") + "|chk\n";
  debugvalues += "ezTime_debug|" + (String) (ESP_Config.ezTime_debug ? "checked" : "") + "|chk\n";
  debugvalues += "pps_debug|" + (String) (ESP_Config.pps_debug ? "checked" : "") + "|chk\n";
  debugvalues += "PE_debug|" + (String) (ESP_Config.PE_debug ? "checked" : "") + "|chk\n";
  //  debugvalues += "BT_KISS_Mode|" + (String) (ESP_Config.BT_KISS_Mode ? "checked" : "") + "|chk\n";
  debugvalues += "TCP_KISS_Mode|" + (String) (ESP_Config.TCP_KISS_Mode ? "checked" : "") + "|chk\n";
  debugvalues += "Serial_KISS_Mode|" + (String) (ESP_Config.Serial_KISS_Mode ? "checked" : "") + "|chk\n";
  debugvalues += "mqtt_enable|" + (String) (ESP_Config.mqtt_enable ? "checked" : "") + "|chk\n";
  debugvalues += "WebConfig_debug|" + (String) (ESP_Config.WebConfig_debug ? "checked" : "") + "|chk\n";
  debugvalues += "standalone|" + (String) (ESP_Config.standalone ? "checked" : "") + "|chk\n";
  debugvalues += "syslog_enable|" + (String) (ESP_Config.syslog_enable ? "checked" : "") + "|chk\n";
  debugvalues += "no_gps|" + (String) (ESP_Config.no_gps ? "checked" : "") + "|chk\n";
  debugvalues += "iGate_Mode|" + (String) (ESP_Config.iGate_Mode ? "checked" : "") + "|chk\n";
  debugvalues += "Beacon_Mode|" + (String) (ESP_Config.Beacon_Mode ? "checked" : "") + "|chk\n";
  debugvalues += "dummy1|" + (String) (dummy1 ? "checked" : "") + "|chk\n";
  debugvalues += "Tracker_Mode|" + (String) (ESP_Config.Tracker_Mode ? "checked" : "") + "|chk\n";
  debugvalues += "IoT_enable|" + (String) (ESP_Config.IoT_enable ? "checked" : "") + "|chk\n";
  debugvalues += "Admin_Mode|" + (String) (ESP_Config.Admin_Mode ? "checked" : "") + "|chk\n";
  debugvalues += "Reboot_Now|" + (String) (ESP_Config.Reboot_Now ? "checked" : "") + "|chk\n";
  debugvalues += "Load_Default_Conf|" + (String) (force_config_init ? "checked" : "") + "|chk\n";

	web_server.send ( 200, "text/plain", debugvalues);
 	if(WebConfig_debug)	debugA("%s\r",__FUNCTION__); 
}

