// PAGE_InventoryConfiguration.h
#ifndef ABCDE_H
#define ABCDE_H

#include "rdbg.h"
#include <WebServer.h>  
#include "helpers.h"
#include "../configuration_data_definition.h"

extern strConfig ESP_Config;
extern WebServer web_server;
extern volatile bool WebConfig_debug ;  
extern volatile uint16_t AdminTimeOutCounter ;
extern String LoRaDevice ;
extern volatile uint8_t LoraDeviceType ;  // 1== first generation 2== second generation
extern volatile bool has_PCF8574 ;              // autodiscovery
extern volatile bool has_DS3231 ;
extern volatile bool has_DS3231_eeprom ;
extern volatile bool has_Si5351 ;
extern volatile bool has_SSD1306 ;
extern volatile bool has_SSD1306 ;
extern volatile bool has_BME280 ;
extern volatile bool has_Si7021 ;
extern volatile bool has_FM24W256 ;
extern volatile bool has_GPS ;
extern volatile bool has_LittleFS ;
extern bool has_ST77XX ;
extern volatile bool Serial_KISS_Mode ;
extern volatile bool TCP_KISS_Mode ;
extern volatile bool LoRa_initialized ;
extern volatile bool iGate_Mode ;
extern volatile bool Tracker_Mode ;
extern volatile bool BT_KISS_Mode ;
extern bool LoRa_debug ;
extern bool APRS_debug ;
extern volatile bool IoT_enable  ;
extern volatile bool Beacon_Mode ;
extern volatile bool EnableRfToIs ;
extern volatile bool EnableIsToRf ;
extern volatile bool EnableRepeater ;
extern volatile uint8_t EncapType ;
extern float lora_rssi ;
extern float lora_snr ;
extern volatile uint8_t reboot_pin ;
extern bool syslog_enable ;
extern char has_SD_Card[10];
extern char SD_Card_Size[10];

void hard_reboot(void) ;
void write_event_to_log(String my_record);
void WriteConfig(void) ;

//
//  HTML PAGE
//
const char PAGE_InventoryConfiguration[] PROGMEM = R"=====(
<meta name="viewport" content="width=device-width, initial-scale=1" />
<meta http-equiv="Content-Type" content="text/html; charset=utf-8" />
<a href="admin.html"  class="btn btn--s"><</a>&nbsp;&nbsp;<strong>Inventory Configuration</strong>
<hr>
Inventory Configuration:<br>
<form action="" method="get">
<table border="0"  cellspacing="0" cellpadding="3" style="width:310px" >
<tr><td align="right">has_PCF8574:</td><td><input type="checkbox" id="has_PCF8574" name="has_PCF8574" disabled ></td></tr>
<tr><td align="right">has_DS3231:</td><td><input type="checkbox" id="has_DS3231" name="has_DS3231" disabled ></td></tr>
<tr><td align="right">has_DS3231_eeprom:</td><td><input type="checkbox" id="has_DS3231_eeprom" name="has_DS3231_eeprom" disabled ></td></tr>
<tr><td align="right">has_Si5351:</td><td><input type="checkbox" id="has_Si5351" name="has_Si5351" disabled ></td></tr>
<tr><td align="right">has_SSD1306:</td><td><input type="checkbox" id="has_SSD1306" name="has_SSD1306" disabled ></td></tr>
<tr><td align="right">has_ST77XX:</td><td><input type="checkbox" id="has_ST77XX" name="has_ST77XX" disabled ></td></tr>
<tr><td align="right">has_BME280:</td><td><input type="checkbox" id="has_BME280" name="has_BME280" disabled ></td></tr>
<tr><td align="right">has_Si7021:</td><td><input type="checkbox" id="has_Si7021" name="has_Si7021" disabled ></td></tr>
<tr><td align="right">has_FM24W256:</td><td><input type="checkbox" id="has_FM24W256" name="has_FM24W256" disabled ></td></tr>

<tr><td align="right">LoRaDevice:</td><td><input type="text" id="LoRaDevice" name="LoRaDevice" value="" disabled ></td></tr>
<tr><td align="right">LoraDeviceType:</td><td><input type="text" id="LoraDeviceType" name="LoraDeviceType" value="" disabled ></td></tr>

<tr><td align="right">has_GPS:</td><td><input type="checkbox" id="has_GPS" name="has_GPS" disabled ></td></tr>
<tr><td align="right">has_LittleFS:</td><td><input type="checkbox" id="has_LittleFS" name="has_LittleFS" disabled ></td></tr>
<tr><td align="right">has_SD_Card:</td><td><input type="text" id="has_SD_Card" name="has_SD_Card" value="" disabled ></td></tr>
<tr><td align="right">SD_Card_Size:</td><td><input type="text" id="SD_Card_Size" name="SD_Card_Size" value="" disabled ></td></tr>
<!-- <tr><td colspan="2" align="center"><input type="submit" style="width:150px" class="btn btn--m btn--blue" value="Save"></td></tr> -->
<tr><td colspan="2" align="center"><a href="inventory.html" class="btn btn--m btn--blue">Refresh</a></td></tr>
</table>
</form>
<hr>


<script>

window.onload = function ()
{
	load("style.css","css", function() 
	{
		load("microajax.js","js", function() 
		{
					setValues("/admin/inventoryvalues");
		});
	});
}
function load(e,t,n){if("js"==t){var a=document.createElement("script");a.src=e,a.type="text/javascript",a.async=!1,a.onload=function(){n()},document.getElementsByTagName("head")[0].appendChild(a)}else if("css"==t){var a=document.createElement("link");a.href=e,a.rel="stylesheet",a.type="text/css",a.async=!1,a.onload=function(){n()},document.getElementsByTagName("head")[0].appendChild(a)}}




</script>


)=====";

const char PAGE_InventoryWaitAndReload[] PROGMEM = R"=====(
<meta http-equiv="refresh" content="5; URL=inventory.html">
Please Wait....Configuring and Restarting.
)=====";


//
//  SEND HTML PAGE OR IF A FORM SUMBITTED VALUES, PROCESS THESE VALUES
// 
void send_inventory_configuration_html() ;
//
//   FILL THE PAGE WITH VALUES
//
void send_inventory_configuration_values_html() ;

#endif
