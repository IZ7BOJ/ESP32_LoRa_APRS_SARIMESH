// PAGE_NetworkConfiguration.cpp

#include "PAGE_NetworkConfiguration.h"



//
//  SEND HTML PAGE OR IF A FORM SUMBITTED VALUES, PROCESS THESE VALUES
// 

void send_network_configuration_html() {
	if (web_server.args() > 0 ) { // Save Settings
		String temp = "";
		ESP_Config.dhcp = false;
		for ( uint8_t i = 0; i < web_server.args(); i++ ) {
			if (web_server.argName(i) == "ssid") ESP_Config.ssid =   urldecode(web_server.arg(i));
			if (web_server.argName(i) == "password") ESP_Config.password =    urldecode(web_server.arg(i)); 
			if (web_server.argName(i) == "ip_0") if (checkRange(web_server.arg(i))) 	ESP_Config.IP[0] =  web_server.arg(i).toInt();
			if (web_server.argName(i) == "ip_1") if (checkRange(web_server.arg(i))) 	ESP_Config.IP[1] =  web_server.arg(i).toInt();
			if (web_server.argName(i) == "ip_2") if (checkRange(web_server.arg(i))) 	ESP_Config.IP[2] =  web_server.arg(i).toInt();
			if (web_server.argName(i) == "ip_3") if (checkRange(web_server.arg(i))) 	ESP_Config.IP[3] =  web_server.arg(i).toInt();
			if (web_server.argName(i) == "nm_0") if (checkRange(web_server.arg(i))) 	ESP_Config.Netmask[0] =  web_server.arg(i).toInt();
			if (web_server.argName(i) == "nm_1") if (checkRange(web_server.arg(i))) 	ESP_Config.Netmask[1] =  web_server.arg(i).toInt();
			if (web_server.argName(i) == "nm_2") if (checkRange(web_server.arg(i))) 	ESP_Config.Netmask[2] =  web_server.arg(i).toInt();
			if (web_server.argName(i) == "nm_3") if (checkRange(web_server.arg(i))) 	ESP_Config.Netmask[3] =  web_server.arg(i).toInt();
			if (web_server.argName(i) == "gw_0") if (checkRange(web_server.arg(i))) 	ESP_Config.Gateway[0] =  web_server.arg(i).toInt();
			if (web_server.argName(i) == "gw_1") if (checkRange(web_server.arg(i))) 	ESP_Config.Gateway[1] =  web_server.arg(i).toInt();
			if (web_server.argName(i) == "gw_2") if (checkRange(web_server.arg(i))) 	ESP_Config.Gateway[2] =  web_server.arg(i).toInt();
			if (web_server.argName(i) == "gw_3") if (checkRange(web_server.arg(i))) 	ESP_Config.Gateway[3] =  web_server.arg(i).toInt();
      		if (web_server.argName(i) == "dns_0") if (checkRange(web_server.arg(i)))   ESP_Config.Dns[0] =  web_server.arg(i).toInt();
      		if (web_server.argName(i) == "dns_1") if (checkRange(web_server.arg(i)))   ESP_Config.Dns[1] =  web_server.arg(i).toInt();
      		if (web_server.argName(i) == "dns_2") if (checkRange(web_server.arg(i)))   ESP_Config.Dns[2] =  web_server.arg(i).toInt();
      		if (web_server.argName(i) == "dns_3") if (checkRange(web_server.arg(i)))   ESP_Config.Dns[3] =  web_server.arg(i).toInt();
			if (web_server.argName(i) == "dhcp") ESP_Config.dhcp = true;
			} ;
	  	web_server.send ( 200, "text/html", PAGE_WaitAndReload );
    	bool save_c = save_config_enable ;
    	save_config_enable = true ;
		WriteConfig();
    	save_config_enable = save_c ;    
		AdminTimeOutCounter=0;
		}
	else {
		web_server.send ( 200, "text/html", PAGE_NetworkConfiguration ); 
		} ;
   if(WebConfig_debug)  debugA("%s",__FUNCTION__); 
}



//
//   FILL THE PAGE WITH VALUES
//

void send_network_configuration_values_html() {
	String values ="";
  	if( ESP_Config.ssid == NULL) ESP_Config.ssid = "PleaseSetMe";
	values += "ssid|" + (String) ESP_Config.ssid + "|input\n";
	values += "password|" +  (String) ESP_Config.password + "|input\n";
	values += "ip_0|" +  (String) ESP_Config.IP[0] + "|input\n";
	values += "ip_1|" +  (String) ESP_Config.IP[1] + "|input\n";
	values += "ip_2|" +  (String) ESP_Config.IP[2] + "|input\n";
	values += "ip_3|" +  (String) ESP_Config.IP[3] + "|input\n";
	values += "nm_0|" +  (String) ESP_Config.Netmask[0] + "|input\n";
	values += "nm_1|" +  (String) ESP_Config.Netmask[1] + "|input\n";
	values += "nm_2|" +  (String) ESP_Config.Netmask[2] + "|input\n";
	values += "nm_3|" +  (String) ESP_Config.Netmask[3] + "|input\n";
	values += "gw_0|" +  (String) ESP_Config.Gateway[0] + "|input\n";
	values += "gw_1|" +  (String) ESP_Config.Gateway[1] + "|input\n";
	values += "gw_2|" +  (String) ESP_Config.Gateway[2] + "|input\n";
	values += "gw_3|" +  (String) ESP_Config.Gateway[3] + "|input\n";
  	values += "dns_0|" +  (String) ESP_Config.Dns[0] + "|input\n";
  	values += "dns_1|" +  (String) ESP_Config.Dns[1] + "|input\n";
  	values += "dns_2|" +  (String) ESP_Config.Dns[2] + "|input\n";
  	values += "dns_3|" +  (String) ESP_Config.Dns[3] + "|input\n";
	values += "dhcp|" +  (String) (ESP_Config.dhcp ? "checked" : "") + "|chk\n";

	web_server.send ( 200, "text/plain", values);
   	if(WebConfig_debug)  debugA("%s",__FUNCTION__); 	
}


//
//   FILL THE PAGE WITH NETWORKSTATE & NETWORKS
//

void send_connection_state_values_html() {
	String state = "N/A";
	String Networks = "";
	if (WiFi.status() == 0) state = "Idle";
	else if (WiFi.status() == 1) state = "NO SSID AVAILBLE";
	else if (WiFi.status() == 2) state = "SCAN COMPLETED";
	else if (WiFi.status() == 3) state = "CONNECTED";
	else if (WiFi.status() == 4) state = "CONNECT FAILED";
	else if (WiFi.status() == 5) state = "CONNECTION LOST";
	else if (WiFi.status() == 6) state = "DISCONNECTED";

	int n = WiFi.scanNetworks();
	if (n == 0){
		Networks = "<font color='#FF0000'>No networks found!</font>";
	 	}
	else {
		Networks = "Found " +String(n) + " Networks<br>";
		Networks += "<table border='0' cellspacing='0' cellpadding='3'>";
		Networks += "<tr bgcolor='#DDDDDD' ><td><strong>Name</strong></td><td><strong>Quality</strong></td><td><strong>Enc</strong></td><tr>";
		for (int i = 0; i < n; ++i) {
			int quality=0;
			if(WiFi.RSSI(i) <= -100) {
				quality = 0;
				}
			else if(WiFi.RSSI(i) >= -50) {
				quality = 100;
				}
			else {
				quality = 2 * (WiFi.RSSI(i) + 100);
				}
//			Networks += "<tr><td><a href='javascript:selssid(\""  +  String(WiFi.SSID(i))  + "\")'>"  +  String(WiFi.SSID(i))  + "</a></td><td>" +  String(quality) + "%</td><td>" +  String((WiFi.encryptionType(i) == ENC_TYPE_NONE)?" ":"*")  + "</td></tr>";
      	Networks += "<tr><td><a href='javascript:selssid(\""  +  String(WiFi.SSID(i))  + "\")'>"  +  String(WiFi.SSID(i))  + "</a></td><td>" +  String(quality) + "%</td><td>" +  String(" ")  + "</td></tr>";
		}
		Networks += "</table>";
	}
   
	String values ="";
	values += "connectionstate|" +  state + "|div\n";
	values += "networks|" +  Networks + "|div\n";

	web_server.send ( 200, "text/plain", values);
	if(WebConfig_debug) Serial.println(__FUNCTION__); 
}

