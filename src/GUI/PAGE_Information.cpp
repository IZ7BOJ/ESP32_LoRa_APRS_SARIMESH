// PAGE_Information.cpp
#include "PAGE_Information.h"


//
// FILL WITH INFOMATION
// 

void send_information_values_html () {
	String values ="";
	values += "x_ssid|" + (String)WiFi.SSID() +  "|div\n";
	values += "x_ip|" +  (String) WiFi.localIP()[0] + "." +  (String) WiFi.localIP()[1] + "." +  (String) WiFi.localIP()[2] + "." + (String) WiFi.localIP()[3] +  "|div\n";
	values += "x_gateway|" +  (String) WiFi.gatewayIP()[0] + "." +  (String) WiFi.gatewayIP()[1] + "." +  (String) WiFi.gatewayIP()[2] + "." + (String) WiFi.gatewayIP()[3] +  "|div\n";
	values += "x_netmask|" +  (String) WiFi.subnetMask()[0] + "." +  (String) WiFi.subnetMask()[1] + "." +  (String) WiFi.subnetMask()[2] + "." + (String) WiFi.subnetMask()[3] +  "|div\n";
  	values += "x_dns|" +  (String) WiFi.dnsIP()[0] + "." +  (String) WiFi.dnsIP()[1] + "." +  (String) WiFi.dnsIP()[2] + "." + (String) WiFi.dnsIP()[3] +  "|div\n";
	values += "x_mac|" + GetMacAddress() +  "|div\n";
	values += "x_ntp|" +  (String) DateTime.hour + ":" + (String) + DateTime.minute +  ":"  + (String)  DateTime.second + " " + (String)   DateTime.year + "-" + (String)  DateTime.month + "-" + (String)  DateTime.day +  "|div\n";
	web_server.send ( 200, "text/plain", values);
   	if(WebConfig_debug)  debugA("%s",__FUNCTION__); 
}

