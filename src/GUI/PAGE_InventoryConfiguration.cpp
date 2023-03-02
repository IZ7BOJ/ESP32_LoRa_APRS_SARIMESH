// PAGE_InventoryConfiguration.cpp
#include "PAGE_InventoryConfiguration.h"

//
//  SEND HTML PAGE OR IF A FORM SUMBITTED VALUES, PROCESS THESE VALUES
// 

void send_inventory_configuration_html() {
	if (web_server.args() > 0 ){  // Save Settings 
		web_server.send ( 200, "text/html", PAGE_InventoryWaitAndReload );
		WriteConfig();
		AdminTimeOutCounter=0;
  	}
	else {
		web_server.send ( 200, "text/html", PAGE_InventoryConfiguration ); 
	  };
   if(WebConfig_debug)  debugA("%s",__FUNCTION__); 
}

//
//   FILL THE PAGE WITH VALUES
//

void send_inventory_configuration_values_html() {
	String inventoryvalues ="";
	inventoryvalues += "has_PCF8574|" + (String) (has_PCF8574 ? "checked" : "") + "|chk\n";
  inventoryvalues += "has_DS3231|" + (String) (has_DS3231 ? "checked" : "") + "|chk\n";
  inventoryvalues += "has_DS3231_eeprom|" + (String) (has_DS3231_eeprom ? "checked" : "") + "|chk\n";
  inventoryvalues += "has_Si5351|" + (String) (has_Si5351 ? "checked" : "") + "|chk\n";
  inventoryvalues += "has_SSD1306|" + (String) (has_SSD1306 ? "checked" : "") + "|chk\n";
  inventoryvalues += "has_ST77XX|" + (String) (has_ST77XX ? "checked" : "") + "|chk\n";
  inventoryvalues += "has_BME280|" + (String) (has_BME280 ? "checked" : "") + "|chk\n";
  inventoryvalues += "has_Si7021|" + (String) (has_Si7021 ? "checked" : "") + "|chk\n";
  inventoryvalues += "has_FM24W256|" + (String) (has_FM24W256 ? "checked" : "") + "|chk\n";
  inventoryvalues += "LoRaDevice|" +  (String) LoRaDevice + "|input\n";
  inventoryvalues += "LoraDeviceType|" +  (String) LoraDeviceType + "|input\n";
  inventoryvalues += "has_GPS|" + (String) (has_GPS ? "checked" : "") + "|chk\n";
  inventoryvalues += "has_LittleFS|" + (String) (has_LittleFS ? "checked" : "") + "|chk\n";
  inventoryvalues += "has_SD_Card|" +  (String) has_SD_Card + "|input\n";
  inventoryvalues += "SD_Card_Size|" +  (String) SD_Card_Size + "|input\n";

	web_server.send ( 200, "text/plain", inventoryvalues);
  if(WebConfig_debug)  debugA("%s",__FUNCTION__); 
}

