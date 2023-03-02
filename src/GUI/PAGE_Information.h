// PAGE_Information.h
#ifndef P_INFOMATION_H
#define P_INFOMATION_H

#include "rdbg.h"
#include <Arduino.h>
#include <WebServer.h>  
#include "../configuration_data_definition.h"
#include "helpers.h"

extern strConfig ESP_Config;
extern WebServer web_server;
extern volatile uint16_t AdminTimeOutCounter ;
extern volatile bool WebConfig_debug ;
extern strDateTime DateTime ;

void send_information_values_html ();
void WriteConfig(void) ;


//
//   The HTML PAGE
//
const char PAGE_Information[] PROGMEM = R"=====(
<meta name="viewport" content="width=device-width, initial-scale=1" />
<meta http-equiv="Content-Type" content="text/html; charset=utf-8" />
<link rel="stylesheet" href="style.css" type="text/css" />
<script src="microajax.js"></script> 
<a href="admin.html"  class="btn btn--s"><</a>&nbsp;&nbsp;<strong>WiFi Upstream Network Status</strong>
<hr>
<table border="0"  cellspacing="0" cellpadding="3" style="width:310px" >
<tr><td align="right">SSID :</td><td><span id="x_ssid"></span></td></tr>
<tr><td align="right">IP :</td><td><span id="x_ip"></span></td></tr>
<tr><td align="right">Netmask :</td><td><span id="x_netmask"></span></td></tr>
<tr><td align="right">Gateway :</td><td><span id="x_gateway"></span></td></tr>
<tr><td align="right">Dns :</td><td><span id="x_dns"></span></td></tr>
<tr><td align="right">Mac :</td><td><span id="x_mac"></span></td></tr>
<tr><td colspan="2"><hr></span></td></tr>
<!-- <tr><td align="right">NTP Time:</td><td><span id="x_ntp"></span></td></tr> -->


<tr><td colspan="2" align="center"><a href="javascript:GetState()" class="btn btn--m btn--blue">Refresh</a></td></tr>
</table>
<script>

function GetState()
{
	setValues("/admin/infovalues");
}

window.onload = function ()
{
	load("style.css","css", function() 
	{
		load("microajax.js","js", function() 
		{
				GetState();
		});
	});
}
function load(e,t,n){if("js"==t){var a=document.createElement("script");a.src=e,a.type="text/javascript",a.async=!1,a.onload=function(){n()},document.getElementsByTagName("head")[0].appendChild(a)}else if("css"==t){var a=document.createElement("link");a.href=e,a.rel="stylesheet",a.type="text/css",a.async=!1,a.onload=function(){n()},document.getElementsByTagName("head")[0].appendChild(a)}}

</script>
)=====" ;


#endif
