// PAGE_Statistics.h
#ifndef P_STATS_H
#define P_STATS_H

#include "rdbg.h"
#include <Arduino.h>
#include "global.h"
#include <WebServer.h>  

extern volatile bool WebConfig_debug;      // to debug webConfig 
extern WebServer web_server;             // The Webserver
extern volatile uint32_t LoRa_lost_packets ;
extern volatile uint32_t LoRa_rx_packets ;
extern volatile uint32_t LoRa_tx_packets ;
extern volatile uint32_t LoRa_rx_AX25_packets ;
extern volatile uint32_t LoRa_tx_AX25_packets ;
extern volatile uint32_t LoRa_rx_OEStyle_packets ;
extern volatile uint32_t LoRa_tx_OEStyle_packets ;
extern volatile uint32_t LoRa_rx_native_packets ;
extern volatile uint32_t LoRa_tx_native_packets ;
extern volatile uint32_t AprsIS_rx_packets ;
extern volatile uint32_t AprsIS_tx_packets ;
extern volatile uint32_t AprsIS_dropped_packets ;
extern volatile uint32_t LoRa_CRC_errored_packets;
extern volatile uint32_t LoRa_OnAirTime ;
extern volatile float cpu_temp ;


//
//   The HTML PAGE
//
const char PAGE_Statistics[] PROGMEM = R"=====(
<meta name="viewport" content="width=device-width, initial-scale=1" />
<meta http-equiv="Content-Type" content="text/html; charset=utf-8" />
<link rel="stylesheet" href="style.css" type="text/css" />
<script src="microajax.js"></script> 
<a href="admin.html"  class="btn btn--s"><</a>&nbsp;&nbsp;<strong>Statistics Data</strong>

<hr>
<table border="0"  cellspacing="0" cellpadding="3" style="width:310px" >
<tr><td align="right">Device Date/Time:</td><td><span id="x_device_date"></span></td></tr>
<tr><td colspan="2"><hr></span></td></tr>
<tr><td align="right">LoRa_rx_packets:</td><td><span id="x_LoRa_rx_packets"></span></td></tr>
<tr><td align="right">LoRa_tx_packets:</td><td><span id="x_LoRa_tx_packets"></span></td></tr>
<tr><td align="right">LoRa_rx_AX25_packets:</td><td><span id="x_LoRa_rx_AX25_packets"></span></td></tr>
<tr><td align="right">LoRa_tx_AX25_packets:</td><td><span id="x_LoRa_tx_AX25_packets"></span></td></tr>

<tr><td align="right">LoRa_rx_OEStyle_packets:</td><td><span id="x_LoRa_rx_OEStyle_packets"></span></td></tr>
<tr><td align="right">LoRa_tx_OEStyle_packets:</td><td><span id="x_LoRa_tx_OEStyle_packets"></span></td></tr>

<tr><td align="right">LoRa_rx_native_packets:</td><td><span id="x_LoRa_rx_native_packets"></span></td></tr>
<tr><td align="right">LoRa_tx_native_packets:</td><td><span id="x_LoRa_tx_native_packets"></span></td></tr>

<tr><td align="right">LoRa_lost_packets:</td><td><span id="x_LoRa_lost_packets"></span></td></tr>
<tr><td colspan="2"><hr></span></td></tr>
<tr><td align="right">AprsIS_rx_packets:</td><td><span id="x_AprsIS_rx_packets"></span></td></tr>
<tr><td align="right">AprsIS_tx_packets:</td><td><span id="x_AprsIS_tx_packets"></span></td></tr>
<tr><td align="right">AprsIS_dropped_packets:</td><td><span id="x_AprsIS_dropped_packets"></span></td></tr>
<tr><td align="right">LoRa_CRC_errored_packets:</td><td><span id="x_LoRa_CRC_errored_packets"></span></td></tr>

<tr><td colspan="2"><hr></span></td></tr>
<tr><td align="right">LoRa_OnAirTime(msec):</td><td><span id="x_LoRa_OnAirTime"></span></td></tr>
<tr><td align="right">CPU Temperature(C°):</td><td><span id="x_cpu_temp"></span></td></tr>
<tr><td align="right">CPU_UpTime(secs):</td><td><span id="x_Processor_UpTime"></span></td></tr>

<tr><td colspan="2" align="center"><a href="javascript:GetState()" class="btn btn--m btn--blue">Refresh</a></td></tr>
</table>
<script>

function GetState()
{
	setValues("/admin/statsvalues");
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


//
// FILL WITH INFOMATION
// 
void send_stats_values_html () ;

#endif // TAGS

