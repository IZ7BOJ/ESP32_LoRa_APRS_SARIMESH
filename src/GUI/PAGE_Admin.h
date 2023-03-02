// PAGE_Admin.h
#ifndef PAGE_ADMIN_H
#define PAGE_ADMIN_H

#include "rdbg.h"
#include <Arduino.h>
#include <WebServer.h>  
#include "helpers.h"
#include "../configuration_data_definition.h"

extern WebServer web_server;
extern strConfig ESP_Config;
extern volatile uint16_t AdminTimeOutCounter ;
extern volatile bool WebConfig_debug ; 

void WriteConfig(void) ;

//
//  HTML PAGE
//

const char PAGE_AdminMainPage_aprs[] PROGMEM = R"=====(
<meta name="viewport" content="width=device-width, initial-scale=1" />
<center>
<strong>LoRa Tracker Admin</strong>
<hr>
<a href="general.html" style="width:250px" class="btn btn--m btn--blue" >General Configuration</a><br>
<a href="dashboard.html" style="width:250px" class="btn btn--m btn--blue" >DashBoard</a><br>
<a href="config.html" style="width:250px" class="btn btn--m btn--blue" >Network Configuration</a><br>
<a href="info.html"   style="width:250px"  class="btn btn--m btn--blue" >Network Information</a><br>
<a href="ntp.html"   style="width:250px"  class="btn btn--m btn--blue" >NTP Settings</a><br>
<a href="debug.html"   style="width:250px"  class="btn btn--m btn--blue" >Operation Mode Settings</a><br>
<a href="inventory.html"   style="width:250px"  class="btn btn--m btn--blue" >Board Inventory</a><br>
<a href="aprs.html"   style="width:250px"  class="btn btn--m btn--blue" >APRS Configuration</a><br>
<a href="lora.html"   style="width:250px"  class="btn btn--m btn--blue" >LoRa Configuration</a><br>
<a href="beacon.html"   style="width:250px"  class="btn btn--m btn--blue" >Beacon Configuration</a><br>
<a href="encap.html"   style="width:250px"  class="btn btn--m btn--blue" >APRS Compatibility</a><br>
<a href="iot.html"   style="width:250px"  class="btn btn--m btn--blue" >IoT Configuration</a><br>
<a href="hwadj.html"   style="width:250px"  class="btn btn--m btn--blue" >HW Setup Adjustment</a><br>
<a href="files.html"   style="width:250px"  class="btn btn--m btn--blue" >Save/Restore Configuration</a><br>
<a href="stats.html"   style="width:250px"  class="btn btn--m btn--blue" >Statistics</a><br>

</center>

<script>
window.onload = function ()
{
	load("style.css","css", function() 
	{
		load("microajax.js","js", function() 
		{
				// Do something after load...
		});
	});
}
function load(e,t,n){if("js"==t){var a=document.createElement("script");a.src=e,a.type="text/javascript",a.async=!1,a.onload=function(){n()},document.getElementsByTagName("head")[0].appendChild(a)}else if("css"==t){var a=document.createElement("link");a.href=e,a.rel="stylesheet",a.type="text/css",a.async=!1,a.onload=function(){n()},document.getElementsByTagName("head")[0].appendChild(a)}}

</script>

)=====";



const char PAGE_AdminMainPage_kiss[] PROGMEM = R"=====(
<meta name="viewport" content="width=device-width, initial-scale=1" />
<center>
<strong>LoRa Tracker Admin</strong>
<hr>
<a href="general.html" style="width:250px" class="btn btn--m btn--blue" >General Configuration</a><br>
<a href="dashboard.html" style="width:250px" class="btn btn--m btn--blue" >DashBoard</a><br>
<a href="config.html" style="width:250px" class="btn btn--m btn--blue" >Network Configuration</a><br>
<a href="info.html"   style="width:250px"  class="btn btn--m btn--blue" >Network Information</a><br>
<a href="ntp.html"   style="width:250px"  class="btn btn--m btn--blue" >NTP Settings</a><br>
<a href="debug.html"   style="width:250px"  class="btn btn--m btn--blue" >Operation Mode Settings</a><br>
<a href="inventory.html"   style="width:250px"  class="btn btn--m btn--blue" >Board Inventory</a><br>
<!-- 
<a href="aprs.html"   style="width:250px"  class="btn btn--m btn--blue" >APRS Configuration</a><br>
-->
<a href="lora.html"   style="width:250px"  class="btn btn--m btn--blue" >LoRa Configuration</a><br>
<a href="beacon.html"   style="width:250px"  class="btn btn--m btn--blue" >Beacon Configuration</a><br>
<a href="encap.html"   style="width:250px"  class="btn btn--m btn--blue" >APRS Compatibility</a><br>
<a href="iot.html"   style="width:250px"  class="btn btn--m btn--blue" >IoT Configuration</a><br>
<a href="hwadj.html"   style="width:250px"  class="btn btn--m btn--blue" >HW Setup Adjustment</a><br>
<a href="files.html"   style="width:250px"  class="btn btn--m btn--blue" >Save/Restore Configuration</a><br>
<a href="stats.html"   style="width:250px"  class="btn btn--m btn--blue" >Statistics</a><br>

</center>

<script>
window.onload = function ()
{
  load("style.css","css", function() 
  {
    load("microajax.js","js", function() 
    {
        // Do something after load...
    });
  });
}
function load(e,t,n){if("js"==t){var a=document.createElement("script");a.src=e,a.type="text/javascript",a.async=!1,a.onload=function(){n()},document.getElementsByTagName("head")[0].appendChild(a)}else if("css"==t){var a=document.createElement("link");a.href=e,a.rel="stylesheet",a.type="text/css",a.async=!1,a.onload=function(){n()},document.getElementsByTagName("head")[0].appendChild(a)}}

</script>

)=====";


const char PAGE_AdminMainPage_core[] PROGMEM = R"=====(
<meta name="viewport" content="width=device-width, initial-scale=1" />
<center>
<strong>LoRa Tracker Admin</strong>
<hr>
<a href="general.html" style="width:250px" class="btn btn--m btn--blue" >General Configuration</a><br>
<a href="dashboard.html" style="width:250px" class="btn btn--m btn--blue" >DashBoard</a><br>
<a href="config.html" style="width:250px" class="btn btn--m btn--blue" >Network Configuration</a><br>
<a href="info.html"   style="width:250px"  class="btn btn--m btn--blue" >Network Information</a><br>
<a href="ntp.html"   style="width:250px"  class="btn btn--m btn--blue" >NTP Settings</a><br>
<a href="debug.html"   style="width:250px"  class="btn btn--m btn--blue" >Operation Mode Settings</a><br>
<a href="inventory.html"   style="width:250px"  class="btn btn--m btn--blue" >Board Inventory</a><br>
<a href="aprs.html"   style="width:250px"  class="btn btn--m btn--blue" >APRS Configuration</a><br>
<a href="lora.html"   style="width:250px"  class="btn btn--m btn--blue" >LoRa Configuration</a><br>
<a href="beacon.html"   style="width:250px"  class="btn btn--m btn--blue" >Beacon Configuration</a><br>
<a href="encap.html"   style="width:250px"  class="btn btn--m btn--blue" >APRS Compatibility</a><br>
<a href="iot.html"   style="width:250px"  class="btn btn--m btn--blue" >IoT Configuration</a><br>
<a href="hwadj.html"   style="width:250px"  class="btn btn--m btn--blue" >HW Setup Adjustment</a><br>
<a href="files.html"   style="width:250px"  class="btn btn--m btn--blue" >Save/Restore Configuration</a><br>
<a href="stats.html"   style="width:250px"  class="btn btn--m btn--blue" >Statistics</a><br>

</center>

<script>
window.onload = function ()
{
	load("style.css","css", function() 
	{
		load("microajax.js","js", function() 
		{
				// Do something after load...
		});
	});
}
function load(e,t,n){if("js"==t){var a=document.createElement("script");a.src=e,a.type="text/javascript",a.async=!1,a.onload=function(){n()},document.getElementsByTagName("head")[0].appendChild(a)}else if("css"==t){var a=document.createElement("link");a.href=e,a.rel="stylesheet",a.type="text/css",a.async=!1,a.onload=function(){n()},document.getElementsByTagName("head")[0].appendChild(a)}}

</script>

)=====";

#endif   
