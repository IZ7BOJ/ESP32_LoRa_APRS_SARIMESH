// PAGE_LoRaConfiguration.cpp

#include "PAGE_LoRaConfiguration.h"

//
//  SEND HTML PAGE OR IF A FORM SUMBITTED VALUES, PROCESS THESE VALUES
// 

void send_lora_configuration_html() {
	if (web_server.args() > 0 ) {  // Save Settings
		for ( uint8_t i = 0; i < web_server.args(); i++ ) {
      if (web_server.argName(i) == "LoraSf") ESP_Config.LoraSf =  web_server.arg(i).toInt();
      if (web_server.argName(i) == "LoraCodingRate") ESP_Config.LoraCodingRate =  web_server.arg(i).toInt();
      if (web_server.argName(i) == "LoraPreambleLen") ESP_Config.LoraPreambleLen =  web_server.arg(i).toInt();
      if (web_server.argName(i) == "LoraSync") ESP_Config.LoraSync =  web_server.arg(i).toInt();
      if (web_server.argName(i) == "LoraPower") ESP_Config.LoraPower =  web_server.arg(i).toInt();
      if (web_server.argName(i) == "LoraFreq") ESP_Config.LoraFreq =   urldecode(web_server.arg(i));
      if (web_server.argName(i) == "LoraBw") ESP_Config.LoraBw =   urldecode(web_server.arg(i));
      if (web_server.argName(i) == "LoraFreqCorr") ESP_Config.LoraFreqCorr =  web_server.arg(i).toInt();
	  	};
	  web_server.send ( 200, "text/html", PAGE_LoRaWaitAndReload );
		WriteConfig();
		AdminTimeOutCounter=0;		
	  }
	else{
    if(LoraDeviceType == 0){  // first generation lora chips
		  web_server.send ( 200, "text/html", PAGE_LoRaConfiguration_1g ); 
      }
    else{
 		  web_server.send ( 200, "text/html", PAGE_LoRaConfiguration_2g );      
      };
	  };
   if(WebConfig_debug)  debugA("%s",__FUNCTION__); 
}



//
//   FILL THE PAGE WITH VALUES
//

void send_lora_configuration_values_html() {
  char TEMP3[3];
  sprintf(TEMP3,"%d",(int) ppm_runtime) ;
  //  sprintf(TEMP3,"%5.2f",ppm_runtime) ;
  String LoRa_FreqJitter = TEMP3;
  String LoRavalues ="";
  LoRavalues += "LoraFreq|" + (String) ESP_Config.LoraFreq + "|input\n";
  LoRavalues += "LoraBw|" +  (String) ESP_Config.LoraBw + "|input\n";
  LoRavalues += "LoraSf|" + (String) ESP_Config.LoraSf + "|input\n";
  LoRavalues += "LoraCodingRate|" +  (String) ESP_Config.LoraCodingRate + "|input\n";
  LoRavalues += "LoraPreambleLen|" +  (String) ESP_Config.LoraPreambleLen + "|input\n";
  LoRavalues += "LoraSync|" + (String) ESP_Config.LoraSync + "|input\n";
  LoRavalues += "LoraPower|" +  (String) ESP_Config.LoraPower + "|input\n";
  LoRavalues += "LoraFreqCorr|" +  (String) ESP_Config.LoraFreqCorr + "|input\n";
  LoRavalues += "x_LoRa_FreqJitter|" + (String) LoRa_FreqJitter +  "|div\n";

	web_server.send ( 200, "text/plain", LoRavalues);
  if(WebConfig_debug)  debugA("%s",__FUNCTION__); 
}
