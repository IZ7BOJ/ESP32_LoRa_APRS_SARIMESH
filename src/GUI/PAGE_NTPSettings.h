// PAGE_NTPSettings.h
#ifndef P_NTP_H
#define P_NTP_H

#include "rdbg.h"
#include <Arduino.h>
#include "ezTime_loc.h"                   // local fork for the ezTime library to support RTC and GPS in addition to NTP synch
#include "helpers.h"
#include <WebServer.h>  
#include "../configuration_data_definition.h"

extern strConfig ESP_Config;
extern WebServer web_server;
extern volatile bool WebConfig_debug ;  
extern strDateTime DateTime;
extern Timezone Italy;
extern Timezone UTC;
extern volatile uint16_t AdminTimeOutCounter ;
extern String SW_Revision ;
extern bool firstStart ;       

void WriteConfig(void) ;


const char PAGE_NTPConfiguration[] PROGMEM = R"=====(
<meta name="viewport" content="width=device-width, initial-scale=1" />
<meta http-equiv="Content-Type" content="text/html; charset=utf-8" />
<a href="admin.html"  class="btn btn--s"><</a>&nbsp;&nbsp;<strong>NTP Settings</strong>
<hr>
<form action="" method="get">
<table border="0"  cellspacing="0" cellpadding="3" >
<tr><td align="right">NTP Server:</td><td><input type="text" id="ntpServerName" name="ntpServerName" maxlength="172" value=""></td></tr>
<!--
<tr><td align="right">Update:</td><td><input type="text" id="update" name="update" size="3"maxlength="6" value=""> minutes (0=disable)</td></tr>
-->
<tr><td>Timezone</td><td>
<select  id="tz" name="tz">
	<option value="-120">(GMT-12:00)</option>
	<option value="-110">(GMT-11:00)</option>
	<option value="-100">(GMT-10:00)</option>
	<option value="-90">(GMT-09:00)</option>
	<option value="-80">(GMT-08:00)</option>
	<option value="-70">(GMT-07:00)</option>
	<option value="-60">(GMT-06:00)</option>
	<option value="-50">(GMT-05:00)</option>
	<option value="-40">(GMT-04:00)</option>
	<option value="-35">(GMT-03:30)</option>
	<option value="-30">(GMT-03:00)</option>
	<option value="-20">(GMT-02:00)</option>
	<option value="-10">(GMT-01:00)</option>
	<option value="0">(GMT+00:00)</option>
	<option value="10">(GMT+01:00)</option>
	<option value="20">(GMT+02:00)</option>
	<option value="30">(GMT+03:00)</option>
	<option value="35">(GMT+03:30)</option>
	<option value="40">(GMT+04:00)</option>
	<option value="45">(GMT+04:30)</option>
	<option value="50">(GMT+05:00)</option>
	<option value="55">(GMT+05:30)</option>
	<option value="57">(GMT+05:45)</option>
	<option value="60">(GMT+06:00)</option>
	<option value="65">(GMT+06:30)</option>
	<option value="70">(GMT+07:00)</option>
	<option value="80">(GMT+08:00)</option>
	<option value="90">(GMT+09:00)</option>
	<option value="95">(GMT+09:30)</option>
	<option value="100">(GMT+10:00)</option>
	<option value="110">(GMT+11:00)</option>
	<option value="120">(GMT+12:00)</option>
	<option value="120">(GMT+12:00)</option>
	<option value="130">(GMT+13:00)</option>
</select>
</td></tr>
<tr><td align="right">Daylight saving:</td><td><input type="checkbox" id="dst" name="dst"></td></tr>
<tr><td colspan="2" align="center"><input type="submit" style="width:150px" class="btn btn--m btn--blue" value="Save"></td></tr>
</table>
</form>
<script>
	

window.onload = function ()
{
	load("style.css","css", function() 
	{
		load("microajax.js","js", function() 
		{
				setValues("/admin/ntpvalues");
		});
	});
}
function load(e,t,n){if("js"==t){var a=document.createElement("script");a.src=e,a.type="text/javascript",a.async=!1,a.onload=function(){n()},document.getElementsByTagName("head")[0].appendChild(a)}else if("css"==t){var a=document.createElement("link");a.href=e,a.rel="stylesheet",a.type="text/css",a.async=!1,a.onload=function(){n()},document.getElementsByTagName("head")[0].appendChild(a)}}


</script>
)=====";


void send_NTP_configuration_html() ;
void send_NTP_configuration_values_html() ;


#endif

