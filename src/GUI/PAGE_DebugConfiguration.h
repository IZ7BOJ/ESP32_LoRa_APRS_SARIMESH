// PAGE_DebugConfiguration.h 
#ifndef PDC10_H
#define PDC10_H

#include "rdbg.h"
#include <WebServer.h>  
#include "helpers.h"
#include "../configuration_data_definition.h"

extern strConfig ESP_Config;
extern WebServer web_server;
extern volatile bool WebConfig_debug ;  
extern volatile uint16_t AdminTimeOutCounter ;
extern bool force_config_init ;
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

void WriteConfig(void) ;
bool LoadConfig( bool force_defaults );
void hard_reboot(void) ;
void write_event_to_log(String my_record);


//
//  HTML PAGE
//
const char PAGE_DebugConfiguration[] PROGMEM = R"=====(
<meta name="viewport" content="width=device-width, initial-scale=1" />
<meta http-equiv="Content-Type" content="text/html; charset=utf-8" />
<a href="admin.html"  class="btn btn--s"><</a>&nbsp;&nbsp;<strong>Operation and Debug Functions</strong>
<hr>
<!-- Operation and Debug Functions:<br> -->
<form action="" method="get">
<table border="0"  cellspacing="0" cellpadding="3" style="width:310px" >
<tr> <td align="left" colspan="2">Debug Mode</td></tr>
<tr><td align="right">gps_debug:</td><td><input type="checkbox" id="gps_debug" name="gps_debug"></td></tr>
<tr><td align="right">LoRa_debug:</td><td><input type="checkbox" id="LoRa_debug" name="LoRa_debug"></td></tr>
<tr><td align="right">APRS_debug:</td><td><input type="checkbox" id="APRS_debug" name="APRS_debug"></td></tr>
<tr><td align="right">RTC_debug:</td><td><input type="checkbox" id="RTC_debug" name="RTC_debug"></td></tr>
<tr><td align="right">ezTime_debug:</td><td><input type="checkbox" id="ezTime_debug" name="ezTime_debug"></td></tr>
<tr><td align="right">pps_debug:</td><td><input type="checkbox" id="pps_debug" name="pps_debug"></td></tr>
<tr><td align="right">PE_debug:</td><td><input type="checkbox" id="PE_debug" name="PE_debug"></td></tr>
<tr><td align="right">WebConfig_debug:</td><td><input type="checkbox" id="WebConfig_debug" name="WebConfig_debug"></td></tr>
<tr><td align="right">act_flag:</td><td><input type="checkbox" id="dummy1" name="dummy1"  ></td></tr> 

<tr> <td align="left" colspan="2"><hr></td></tr>
<tr> <td align="left" colspan="2">Operation Mode</td></tr>

<tr><td align="right">Admin_Mode:</td><td><input type="checkbox" id="Admin_Mode" name="Admin_Mode"></td></tr>
<tr><td align="right">Beacon_Mode:</td><td><input type="checkbox" id="Beacon_Mode" name="Beacon_Mode" ></td></tr>

<tr><td align="right">iGate_Mode:</td><td><input type="checkbox" id="iGate_Mode" name="iGate_Mode"></td></tr>
<tr><td align="right">TCP_KISS_Mode:</td><td><input type="checkbox" id="TCP_KISS_Mode" name="TCP_KISS_Mode"></td></tr>

<!-- 
<tr><td align="right">BT_KISS_Mode:</td><td><input type="checkbox" id="BT_KISS_Mode" name="BT_KISS_Mode"></td></tr> 
-->
<tr><td align="right">Serial_KISS_Mode:</td><td><input type="checkbox" id="Serial_KISS_Mode" name="Serial_KISS_Mode"></td></tr>
<tr><td align="right">Tracker_Mode:</td><td><input type="checkbox" id="Tracker_Mode" name="Tracker_Mode" disabled></td></tr>

<tr><td align="right">standalone:</td><td><input type="checkbox" id="standalone" name="standalone"></td></tr>
<tr><td align="right">mqtt_ctrl_enable:</td><td><input type="checkbox" id="mqtt_enable" name="mqtt_enable"></td></tr>
<tr><td align="right">syslog_enable:</td><td><input type="checkbox" id="syslog_enable" name="syslog_enable"></td></tr>
<tr><td align="right">IoT_enable:</td><td><input type="checkbox" id="IoT_enable" name="IoT_enable"></td></tr>

<tr><td align="right">no_gps:</td><td><input type="checkbox" id="no_gps" name="no_gps"></td></tr>
<tr> <td align="left" colspan="2"><hr></td></tr>
<tr> <td align="left" colspan="2">Maintenance</td></tr>

<tr><td align="right">Load_Default_Conf:</td><td><input type="checkbox" id="Load_Default_Conf" name="Load_Default_Conf"></td></tr>
<tr><td align="right">Reboot_Now:</td><td><input type="checkbox" id="Reboot_Now" name="Reboot_Now"></td></tr>

<tr><td colspan="2" align="center"><input type="submit" style="width:150px" class="btn btn--m btn--blue" value="Save"></td></tr>
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
					setValues("/admin/debugvalues");
		});
	});
}
function load(e,t,n){if("js"==t){var a=document.createElement("script");a.src=e,a.type="text/javascript",a.async=!1,a.onload=function(){n()},document.getElementsByTagName("head")[0].appendChild(a)}else if("css"==t){var a=document.createElement("link");a.href=e,a.rel="stylesheet",a.type="text/css",a.async=!1,a.onload=function(){n()},document.getElementsByTagName("head")[0].appendChild(a)}}


</script>


)=====";


const char PAGE_DebugWaitAndReload[] PROGMEM = R"=====(
<meta http-equiv="refresh" content="5; URL=debug.html">
Please Wait....Configuring and Restarting.
)=====";


const char PAGE_Home[] PROGMEM = R"=====(
<meta http-equiv="refresh" content="5; URL=admin.html">
Please Wait.....
)=====";


//
//  SEND HTML PAGE OR IF A FORM SUMBITTED VALUES, PROCESS THESE VALUES
// 
void send_debug_configuration_html() ;
//
//   FILL THE PAGE WITH VALUES
//
void send_debug_configuration_values_html() ;


#endif

