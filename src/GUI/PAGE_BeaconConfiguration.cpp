// PAGE_BeaconConfiguration.cpp
#include "PAGE_BeaconConfiguration.h"

//
//  SEND HTML PAGE OR IF A FORM SUMBITTED VALUES, PROCESS THESE VALUES
// 

void send_beacon_configuration_html() {
	if (web_server.args() > 0 ) {  // Save Settings
		String temp = "";
		for ( uint8_t i = 0; i < web_server.args(); i++ ) {
      if (web_server.argName(i) == "BeaconId") ESP_Config.BeaconId =  web_server.arg(i).toInt();
      if (web_server.argName(i) == "BeaconSeqNbr") ESP_Config.BeaconSeqNbr =  web_server.arg(i).toInt();
      if (web_server.argName(i) == "BeaconLocation") ESP_Config.BeaconLocation =  web_server.arg(i).toInt();
      if (web_server.argName(i) == "BeaconEngineType") ESP_Config.BeaconEngineType =  web_server.arg(i).toInt();
      if (web_server.argName(i) == "BeaconRun") ESP_Config.BeaconRun =  web_server.arg(i).toInt();
      if (web_server.argName(i) == "BCN_LoRa_Vector") ESP_Config.BCN_LoRa_Vector =   urldecode(web_server.arg(i));
      if (web_server.argName(i) == "BeaconUnixTime") ESP_Config.BeaconUnixTime =  web_server.arg(i).toInt();
      if (web_server.argName(i) == "BeaconFreq") ESP_Config.BeaconFreq =  web_server.arg(i).toInt();
      if (web_server.argName(i) == "BeaconPower") ESP_Config.BeaconPower =  web_server.arg(i).toInt();
      if (web_server.argName(i) == "BeaconWorkConditions") ESP_Config.BeaconWorkConditions =  web_server.arg(i).toInt();
      if (web_server.argName(i) == "BeaconStatsCollector_Port") ESP_Config.BeaconStatsCollector_Port =  web_server.arg(i).toInt();
      if (web_server.argName(i) == "BCN_TimeSlotOffset") ESP_Config.BCN_TimeSlotOffset =  web_server.arg(i).toInt();
      if (web_server.argName(i) == "BCN_TimeSlotSync") ESP_Config.BCN_TimeSlotSync =  web_server.arg(i).toInt();
      if (web_server.argName(i) == "ip_0") if (checkRange(web_server.arg(i)))   ESP_Config.BeaconStatsCollector_IP[0] =  web_server.arg(i).toInt();
      if (web_server.argName(i) == "ip_1") if (checkRange(web_server.arg(i)))   ESP_Config.BeaconStatsCollector_IP[1] =  web_server.arg(i).toInt();
      if (web_server.argName(i) == "ip_2") if (checkRange(web_server.arg(i)))   ESP_Config.BeaconStatsCollector_IP[2] =  web_server.arg(i).toInt();
      if (web_server.argName(i) == "ip_3") if (checkRange(web_server.arg(i)))   ESP_Config.BeaconStatsCollector_IP[3] =  web_server.arg(i).toInt();
		}
		
      web_server.send ( 200, "text/html", PAGE_BeaconWaitAndReload );
		WriteConfig();
		AdminTimeOutCounter=0;
	   }
	else {
      if(LoraDeviceType == 0){  // first generation lora chips 
		   web_server.send ( 200, "text/html", PAGE_BeaconConfiguration_1g ); 
         }
      else{
		   web_server.send ( 200, "text/html", PAGE_BeaconConfiguration_2g ); 
         };
   }
	if(WebConfig_debug) debugA("%s",__FUNCTION__); 
}


//
//   FILL THE PAGE WITH VALUES
//

void send_beacon_configuration_values_html() {
	String Beaconvalues ="";
   Beaconvalues += "BeaconId|" + (String) ESP_Config.BeaconId + "|input\n";
   Beaconvalues += "BeaconSeqNbr|" +  (String) ESP_Config.BeaconSeqNbr + "|input\n";
   Beaconvalues += "BeaconLocation|" +  (String) ESP_Config.BeaconLocation + "|input\n";
   Beaconvalues += "BeaconEngineType|" +  (String) ESP_Config.BeaconEngineType + "|input\n";
   Beaconvalues += "BeaconRun|" +  (String) ESP_Config.BeaconRun + "|input\n";
   Beaconvalues += "BeaconUnixTime|" +  (String) ESP_Config.BeaconUnixTime + "|input\n";
   Beaconvalues += "BeaconFreq|" +  (String) ESP_Config.BeaconFreq + "|input\n";
   Beaconvalues += "BeaconPower|" +  (String) ESP_Config.BeaconPower + "|input\n";
   Beaconvalues += "BeaconWorkConditions|" +  (String) ESP_Config.BeaconWorkConditions + "|input\n";
   Beaconvalues += "BCN_LoRa_Vector|" +  (String) ESP_Config.BCN_LoRa_Vector + "|input\n";
   Beaconvalues += "ip_0|" +  (String) ESP_Config.BeaconStatsCollector_IP[0] + "|input\n";
   Beaconvalues += "ip_1|" +  (String) ESP_Config.BeaconStatsCollector_IP[1] + "|input\n";
   Beaconvalues += "ip_2|" +  (String) ESP_Config.BeaconStatsCollector_IP[2] + "|input\n";
   Beaconvalues += "ip_3|" +  (String) ESP_Config.BeaconStatsCollector_IP[3] + "|input\n";
   Beaconvalues += "BCN_TimeSlotOffset|" +  (String) ESP_Config.BCN_TimeSlotOffset + "|input\n";
   Beaconvalues += "BCN_TimeSlotSync|" +  (String) ESP_Config.BCN_TimeSlotSync + "|input\n";
   Beaconvalues += "BeaconStatsCollector_Port|" +  (String) ESP_Config.BeaconStatsCollector_Port + "|input\n";

	web_server.send ( 200, "text/plain", Beaconvalues);
	if(WebConfig_debug)  debugA("%s",__FUNCTION__); 	
}

