//PAGE_hwadj.cpp
#include "PAGE_hwadj.h"


//
//  SEND HTML PAGE OR IF A FORM SUMBITTED VALUES, PROCESS THESE VALUES
// 

void send_hwadj_configuration_html() {
  if (web_server.args() > 0 ) {  // Save Settings
    String temp = "";
    for ( uint8_t i = 0; i < web_server.args(); i++ ) {
      if (web_server.argName(i) == "i2c_sda") ESP_Config.i2c_sda =  web_server.arg(i).toInt();
      if (web_server.argName(i) == "i2c_scl") ESP_Config.i2c_scl =  web_server.arg(i).toInt();
      if (web_server.argName(i) == "spi_sck") ESP_Config.spi_sck =  web_server.arg(i).toInt();
      if (web_server.argName(i) == "spi_miso") ESP_Config.spi_miso =  web_server.arg(i).toInt();
      if (web_server.argName(i) == "spi_mosi") ESP_Config.spi_mosi =  web_server.arg(i).toInt();
      if (web_server.argName(i) == "oled_addr") ESP_Config.oled_addr =  web_server.arg(i).toInt();
      if (web_server.argName(i) == "oled_rst") ESP_Config.oled_rst =  web_server.arg(i).toInt();
      if (web_server.argName(i) == "oled_orient") ESP_Config.oled_orient =  web_server.arg(i).toInt();
      if (web_server.argName(i) == "lora_cs") ESP_Config.lora_cs =  web_server.arg(i).toInt();
      if (web_server.argName(i) == "lora_rst") ESP_Config.lora_rst =  web_server.arg(i).toInt();
      if (web_server.argName(i) == "lora_dio") ESP_Config.lora_dio =  web_server.arg(i).toInt();
      if (web_server.argName(i) == "gps_rx") ESP_Config.gps_rx =  web_server.arg(i).toInt();
      if (web_server.argName(i) == "gps_tx") ESP_Config.gps_tx =  web_server.arg(i).toInt();
      } ;

    web_server.send ( 200, "text/html", PAGE_HWadjWaitAndReload );
    WriteConfig();
    UpdateLocalConfigData();
    AdminTimeOutCounter=0;
    }
  else {
    web_server.send ( 200, "text/html", PAGE_HW_SetupAdjustment ); 
    } ;
  if(WebConfig_debug)  debugA("%s",__FUNCTION__); 
}


//
//   FILL THE PAGE WITH VALUES
//

void send_hwadj_configuration_values_html() {
  String hwadjvalues ="";
/* 
   hwadjvalues += "i2c_sda|" + (String) ESP_Config.i2c_sda + "|input\n";
   hwadjvalues += "i2c_scl|" + (String) ESP_Config.i2c_scl + "|input\n";
   hwadjvalues += "spi_sck|" + (String) ESP_Config.spi_sck + "|input\n";
   hwadjvalues += "spi_miso|" + (String) ESP_Config.spi_miso + "|input\n";
   hwadjvalues += "spi_mosi|" + (String) ESP_Config.spi_mosi + "|input\n";
   hwadjvalues += "oled_addr|" + (String) ESP_Config.oled_addr + "|input\n";
   hwadjvalues += "oled_rst|" + (String) ESP_Config.oled_rst + "|input\n";
   hwadjvalues += "oled_orient|" + (String) ESP_Config.oled_orient + "|input\n";
   hwadjvalues += "lora_cs|" + (String) ESP_Config.lora_cs + "|input\n";
   hwadjvalues += "lora_rst|" + (String) ESP_Config.lora_rst + "|input\n";
   hwadjvalues += "lora_dio|" + (String) ESP_Config.lora_dio + "|input\n";
   hwadjvalues += "gps_rx|" + (String) ESP_Config.gps_rx + "|input\n";
   hwadjvalues += "gps_tx|" + (String) ESP_Config.gps_tx + "|input\n";
*/

   hwadjvalues += "i2c_sda|" + (String) i2c_sda + "|input\n";
   hwadjvalues += "i2c_scl|" + (String) i2c_scl + "|input\n";
   hwadjvalues += "spi_sck|" + (String) spi_sck + "|input\n";
   hwadjvalues += "spi_miso|" + (String) spi_miso + "|input\n";
   hwadjvalues += "spi_mosi|" + (String) spi_mosi + "|input\n";
   hwadjvalues += "oled_addr|" + (String) oled_addr + "|input\n";
   hwadjvalues += "oled_rst|" + (String) oled_rst + "|input\n";
   hwadjvalues += "oled_orient|" + (String) oled_orient + "|input\n";
   hwadjvalues += "lora_cs|" + (String) lora_cs + "|input\n";
   hwadjvalues += "lora_rst|" + (String) lora_rst + "|input\n";
   hwadjvalues += "lora_dio|" + (String) lora_dio + "|input\n";
   hwadjvalues += "gps_rx|" + (String) gps_rx + "|input\n";
   hwadjvalues += "gps_tx|" + (String) gps_tx + "|input\n";

   web_server.send ( 200, "text/plain", hwadjvalues);
   if(WebConfig_debug)  debugA("%s",__FUNCTION__); 
}



