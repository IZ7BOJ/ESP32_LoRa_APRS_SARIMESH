// PAGE_IoT_Configuration.h
#ifndef P_IOT_H
#define P_IOT_H

#include "rdbg.h"
#include <Arduino.h>
#include "ezTime_loc.h"                   // local fork for the ezTime library to support RTC and GPS in addition to NTP synch
#include "helpers.h"
#include <WebServer.h>  
#include "../configuration_data_definition.h"
#include <TinyGPS++.h>

extern TinyGPSPlus gps;
extern strConfig ESP_Config;
extern WebServer web_server;
extern volatile bool WebConfig_debug ;  
extern strDateTime DateTime;
extern Timezone Italy;
extern Timezone UTC;
extern volatile uint16_t AdminTimeOutCounter ;

void WriteConfig(void) ;
void UpdateLocalConfigData(void ) ;


//
//  HTML PAGE
//
const char PAGE_IoT_Configuration[] PROGMEM = R"=====(
<meta name="viewport" content="width=device-width, initial-scale=1" />
<meta http-equiv="Content-Type" content="text/html; charset=utf-8" />
<a href="admin.html"  class="btn btn--s"><</a>&nbsp;&nbsp;<strong>IoT-MQTT Configuration</strong>
<hr>
<!-- IoT Configuration:<br> -->
<!--
  // iGate
  cfg.IoT_Host = "rotate.aprs2.net";
  cfg.IoT_Port = 14580;
  cfg.IoT_Login = "IQ8SO-10";
  cfg.IoT_Pass = "19936";
  cfg.IoT_Filter = "m/40"; // solo stazioni entro 20 Km dalla mia posizione
  cfg.IoT_RawBeacon = "IQ8SO-10>APLS01,WIDE1-1:!GPS_LAT/GPS_LON#iGate-LoRa, 15.6/SF9 ";
  cfg.IoT_RawBeaconPeriodSeconds = 300;
  // Tracker
  cfg.IoT_RawBeacon = "I8FUC-7>APLS01,WIDE1-1:!GPS_LAT/GPS_LON#LoRa,15.6/SF9";  
  cfg.IoT_RawBeaconPeriodSeconds = 31;  
-->
<form action="" method="get">
<table border="0"  cellspacing="0" cellpadding="3" style="width:310px" >
<tr><td align="left"> </td><td></td></tr>

<tr><td align="left">IoT-MQTT Portal Configuration</td><td></td></tr>
<tr><td align="right">IoT_Host:</td><td><input type="text" id="IoT_Host" name="IoT_Host" value="" disabled ></td></tr>
<tr><td align="right">IoT_Port:</td><td><input type="text" id="IoT_Port" name="IoT_Port" value="" disabled ></td></tr>
<tr><td align="right">IoT_Login:</td><td><input type="text" id="IoT_Login" name="IoT_Login" value=""></td></tr>
<tr><td align="right">IoT_Key:</td><td><input type="password" id="IoT_Key" name="IoT_Key" value=""></td></tr>


<tr><td align="left">IoT feeds</td><td></td></tr>
<tr><td align="right">feed_1:</td><td><input type="text" id="feed_1" name="feed_1" value=""  ></td></tr>
<tr><td align="right">feed_2:</td><td><input type="text" id="feed_2" name="feed_2" value=""  ></td></tr>
<tr><td align="right">feed_3:</td><td><input type="text" id="feed_3" name="feed_3" value=""  ></td></tr>
<tr><td align="right">feed_4:</td><td><input type="text" id="feed_4" name="feed_4" value=""  ></td></tr>
<tr><td align="right">feed_5:</td><td><input type="text" id="feed_5" name="feed_5" value=""  ></td></tr>
<tr><td align="right">feed_6:</td><td><input type="text" id="feed_6" name="feed_6" value=""  ></td></tr>

<tr><td colspan="2" align="center"><input type="submit" style="width:150px" class="btn btn--m btn--blue" value="Save"></td></tr> 
<!-- <tr><td colspan="2" align="center"><a href="iot.html" class="btn btn--m btn--blue">Refresh</a></td></tr> -->
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
					setValues("/admin/IoTvalues");
		});
	});
}
function load(e,t,n){if("js"==t){var a=document.createElement("script");a.src=e,a.type="text/javascript",a.async=!1,a.onload=function(){n()},document.getElementsByTagName("head")[0].appendChild(a)}else if("css"==t){var a=document.createElement("link");a.href=e,a.rel="stylesheet",a.type="text/css",a.async=!1,a.onload=function(){n()},document.getElementsByTagName("head")[0].appendChild(a)}}




</script>


)=====";

const char PAGE_IoT_WaitAndReload[] PROGMEM = R"=====(
<meta http-equiv="refresh" content="5; URL=iot.html">
Please Wait....Configuring and Restarting.
)=====";


//
//  SEND HTML PAGE OR IF A FORM SUMBITTED VALUES, PROCESS THESE VALUES
// 

void send_iot_configuration_html() ;
//
//   FILL THE PAGE WITH VALUES
//
void send_iot_configuration_values_html() ;

#endif

