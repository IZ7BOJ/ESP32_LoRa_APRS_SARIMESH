// PAGE_General.h 
#ifndef PGEN1_H
#define PGEN1_H

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
extern volatile bool save_config_enable ;

void WriteConfig(void) ;

//
//  HTML PAGE
//

const char PAGE_AdminGeneralSettings[] PROGMEM =  R"=====(
<meta name="viewport" content="width=device-width, initial-scale=1" />
<meta http-equiv="Content-Type" content="text/html; charset=utf-8" />
<a href="admin.html"  class="btn btn--s"><</a>&nbsp;&nbsp;<strong>General Settings</strong>
<hr>
<form action="" method="post">
<table border="0"  cellspacing="0" cellpadding="3" >
<tr>
	<td align="right">Name of Device</td>
	<td><input type="text" id="devicename" name="devicename" value="" ></td>
</tr>

<tr>
  <td align="right">Device Id</td>
  <td><input type="text" id="DeviceId" name="DeviceId"  value="" disabled ></td>
</tr>

<tr>
  <td align="right">CPU_Type</td>
  <td><input type="text" id="cpu_type" name="cpu_type"  value="" disabled ></td>
</tr>

<tr>
  <td align="right">SW_Revision</td>
  <td><input type="text" id="SW_Revision" name="SW_Revision"  value="" disabled ></td>
</tr>

<tr>
	<td align="left" colspan="2"><hr></td>
</tr>

<!--
<tr>
	<td align="left" colspan="2">Turn on at</td>
</tr>
<tr>
	<td align="right"> Enabled:</td>
	<td><input type="checkbox" id="tonenabled" name="tonenabled"></td>
</tr>

<tr>
	<td align="right"> Time:</td>
	<td><input type="text" id="tonhour" name="tonhour" size="2" value="00">:<input type="text" id="tonminute" name="tonminute" size="2" value="00"></td>
</tr>

<tr>
	<td align="left" colspan="2">Turn off at:</td>
<tr>
	<td align="right"> Enabled:</td>
	<td><input type="checkbox" id="toffenabled" name="toffenabled"></td>
</tr>
<tr>
	<td align="right"> Time:</td>
	<td><input type="text" id="toffhour" name="toffhour" size="2" value="00">:<input type="text" id="toffminute" name="toffminute" size="2" value="00"></td>
</tr>
-->

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
				setValues("/admin/generalvalues");
		});
	});
}
function load(e,t,n){if("js"==t){var a=document.createElement("script");a.src=e,a.type="text/javascript",a.async=!1,a.onload=function(){n()},document.getElementsByTagName("head")[0].appendChild(a)}else if("css"==t){var a=document.createElement("link");a.href=e,a.rel="stylesheet",a.type="text/css",a.async=!1,a.onload=function(){n()},document.getElementsByTagName("head")[0].appendChild(a)}}



</script>
)=====";


// Functions for this Page

String get_device_id(void) ;
void send_devicename_value_html() ;
void send_general_html() ;
void send_general_configuration_values_html() ;

#endif
