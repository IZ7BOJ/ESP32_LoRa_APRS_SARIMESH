// PAGE_Dashboard.h
#ifndef PAGE_DASHBOARD_H
#define PAGE_DASHBOARD_H

#include "rdbg.h"
#include <Arduino.h>
#include "ezTime_loc.h"                   // local fork for the ezTime library to support RTC and GPS in addition to NTP synch
#include "helpers.h"
#include <WebServer.h>  
#include "../configuration_data_definition.h"
#include <TinyGPS++.h>

extern TinyGPSPlus gps;
extern volatile bool WebConfig_debug ;  
extern strDateTime DateTime;
extern Timezone Italy;
extern Timezone UTC;
extern WebServer web_server;
extern strConfig ESP_Config;
extern volatile float ppm_runtime;
extern volatile bool BT_KISS_Mode  ;         
extern volatile bool Serial_KISS_Mode  ;         
extern volatile bool TCP_KISS_Mode  ; 
extern volatile uint32_t IPC_lost_msgs ;
extern volatile uint32_t LoRa_lost_packets ;
extern volatile uint32_t LoRa_rx_packets ;
extern volatile uint32_t LoRa_tx_packets ;
extern volatile uint32_t AprsIS_rx_packets ;
extern volatile uint32_t AprsIS_tx_packets ;
extern volatile uint32_t Kiss_rx_packets ;
extern volatile uint32_t Kiss_tx_packets ;
extern volatile uint32_t AprsIS_relayed_packets ;
extern volatile uint32_t LoRa_rx_AX25_packets ;
extern volatile uint32_t LoRa_tx_AX25_packets ;
extern volatile uint32_t LoRa_rx_OEStyle_packets ;
extern volatile uint32_t LoRa_tx_OEStyle_packets ;
extern volatile uint32_t LoRa_rx_native_packets ;
extern volatile uint32_t LoRa_tx_native_packets ;
extern volatile uint32_t LoRa_ReSched_packets;
extern volatile uint32_t LoRa_CAD_errors;
extern volatile float ChanCong , LoRa_ENL;
extern volatile uint32_t AprsIS_dropped_packets ;
extern volatile uint32_t LoRa_CRC_errored_packets;
extern volatile uint32_t LoRa_CRC_errored_packets_BCN;
extern volatile uint32_t LoRa_UMN_errored_packets;
extern volatile uint32_t LoRa_OnAirTime ;
extern volatile float LoRa_DutyCycle  ;
extern volatile uint32_t LoRa_OnAirTime_Tot ;
extern volatile float cpu_temp ;
extern String lat_now,lon_now;
extern String RX_Last_Spot_level ;
extern String RX_Last_Spot_payload ;
extern String RX_Last_Path_Report ;
extern volatile int op_status;
extern volatile int op_status;
extern volatile int32_t time_jitter ;
extern int wifi_unavailable ;
extern int mqtt_unavailable ;
extern volatile bool aprsis_available ;
extern bool volatile syslog_available  ;

void WriteConfig(void) ;

      
//
//   The HTML PAGE
//
const char PAGE_Dashboard[] PROGMEM = R"=====(
<meta name="viewport" content="width=device-width, initial-scale=1" />
<meta http-equiv="Content-Type" content="text/html; charset=utf-8" />
<link rel="stylesheet" href="style.css" type="text/css" />
<script src="microajax.js"></script> 
<a href="admin.html"  class="btn btn--s"><</a>&nbsp;&nbsp;<strong>Dashboard Data (<span id="x_device_type"></span>)</strong>

<hr>
<table border="0"  cellspacing="0" cellpadding="3" style="width:310px" >
<tr><td align="right">Device Date/Time:</td><td><span id="x_device_date"></span></td></tr>
<tr><td align="right">Operation Status:</td><td><span id="x_op_status"></span></td></tr>
<tr><td align="right">Clock Jitter(msec):</td><td><span id="x_time_jitter"></span></td></tr>

<tr><td align="right">WiFi Upstream:</td><td><span id="x_wifi_upstream"></span></td></tr>
<tr><td align="right">APRS-IS Upstream:</td><td><span id="x_aprs_upstream"></span></td></tr>
<tr><td align="right">MQTT Upstream:</td><td><span id="x_mqtt_upstream"></span></td></tr>
<tr><td align="right">Syslog Upstream:</td><td><span id="x_syslog_upstream"></span></td></tr>


<tr><td colspan="2"><hr></span></td></tr>

<tr><td align="right">GPS Fix:</td><td><span id="x_gps_pos"></span></td></tr>
<tr><td align="right">GPS Sats:</td><td><span id="x_gps_sats"></span></td></tr>

<tr><td colspan="2"><hr></span></td></tr>
<tr><td align="right">Last_RX_Pkt_Report:</td><td><span id="x_RX_Last_Spot_level"></span></td></tr>
<tr><td colspan="2"><hr></span></td></tr>

<tr><td colspan="2"><hr></span></td></tr>
<tr><td align="right">Last_RX_Pkt_payload:</td><td><span id="x_RX_Last_Spot_payload"></span></td></tr>
<tr><td colspan="2"><hr></span></td></tr>

<tr><td align="right">Last_RX_Path:</td><td><span id="x_RX_Last_Path_Report"></span></td></tr>
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
<tr><td align="right">LoRa_CRC_errored_packets:</td><td><span id="x_LoRa_CRC_errored_packets"></span></td></tr>

<tr><td align="right">LoRa_UMN_errored_packets:</td><td><span id="x_LoRa_UMN_errored_packets"></span></td></tr>
<tr><td align="right">LoRa_CAD_errors:</td><td><span id="x_LoRa_CAD_errors"></span></td></tr>
<tr><td align="right">LoRa_ReSched_packets:</td><td><span id="x_LoRa_ReSched_packets"></span></td></tr>

<tr><td colspan="2"><hr></span></td></tr>
<tr><td align="right">AprsIS_rx_packets:</td><td><span id="x_AprsIS_rx_packets"></span></td></tr>
<tr><td align="right">AprsIS_tx_packets:</td><td><span id="x_AprsIS_tx_packets"></span></td></tr>
<tr><td align="right">AprsIS_dropped_packets:</td><td><span id="x_AprsIS_dropped_packets"></span></td></tr>
<tr><td align="right">AprsIS_relayed_packets:</td><td><span id="x_AprsIS_relayed_packets"></span></td></tr>

<!--
<tr><td colspan="2"><hr></span></td></tr>
<tr><td align="right">Kiss_rx_packets:</td><td><span id="x_Kiss_rx_packets"></span></td></tr>
<tr><td align="right">Kiss_tx_packets:</td><td><span id="x_Kiss_tx_packets"></span></td></tr>
-->

<tr><td colspan="2"><hr></span></td></tr>
<tr><td align="right">IPC_lost_msgs:</td><td><span id="x_IPC_lost_msgs"></span></td></tr>

<tr><td colspan="2"><hr></span></td></tr>
<tr><td align="right">LoRa_OnAirTime(msec):</td><td><span id="x_LoRa_OnAirTime"></span></td></tr>
<tr><td align="right">LoRa_DutyCycle(%):</td><td><span id="x_LoRa_DutyCycle"></span></td></tr>
<tr><td align="right">LoRa_ChanCong(%):</td><td><span id="x_ChanCong"></span></td></tr>
<tr><td align="right">LoRa_ENL(dbm):</td><td><span id="x_LoRa_ENL"></span></td></tr>
<tr><td align="right">LoRa_FreqJitter(ppm):</td><td><span id="x_LoRa_FreqJitter"></span></td></tr>
<tr><td align="right">CPU Temperature(C°):</td><td><span id="x_cpu_temp"></span></td></tr>
<tr><td align="right">CPU_UpTime(secs):</td><td><span id="x_Processor_UpTime"></span></td></tr>

<tr><td colspan="2" align="center"><a href="javascript:GetState()" class="btn btn--m btn--blue">Refresh</a></td></tr>
</table>
<script>

function GetState()
{
  setValues("/admin/dashboardvalues");
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



const char PAGE_Dashboard_kiss[] PROGMEM = R"=====(
<meta name="viewport" content="width=device-width, initial-scale=1" />
<meta http-equiv="Content-Type" content="text/html; charset=utf-8" />
<link rel="stylesheet" href="style.css" type="text/css" />
<script src="microajax.js"></script> 
<a href="admin.html"  class="btn btn--s"><</a>&nbsp;&nbsp;<strong>Dashboard Data (<span id="x_device_type"></span>)</strong>

<hr>
<table border="0"  cellspacing="0" cellpadding="3" style="width:310px" >
<tr><td align="right">Device Date/Time:</td><td><span id="x_device_date"></span></td></tr>
<tr><td align="right">Operation Status:</td><td><span id="x_op_status"></span></td></tr>
<tr><td align="right">Clock Jitter(msec):</td><td><span id="x_time_jitter"></span></td></tr>

<tr><td align="right">WiFi Upstream:</td><td><span id="x_wifi_upstream"></span></td></tr>
<tr><td align="right">APRS-IS Upstream:</td><td><span id="x_aprs_upstream"></span></td></tr>
<tr><td align="right">MQTT Upstream:</td><td><span id="x_mqtt_upstream"></span></td></tr>
<tr><td align="right">Syslog Upstream:</td><td><span id="x_syslog_upstream"></span></td></tr>


<tr><td colspan="2"><hr></span></td></tr>

<tr><td align="right">GPS Fix:</td><td><span id="x_gps_pos"></span></td></tr>
<tr><td align="right">GPS Sats:</td><td><span id="x_gps_sats"></span></td></tr>

<tr><td colspan="2"><hr></span></td></tr>
<tr><td align="right">Last_RX_Pkt_Report:</td><td><span id="x_RX_Last_Spot_level"></span></td></tr>
<tr><td colspan="2"><hr></span></td></tr>

<tr><td colspan="2"><hr></span></td></tr>
<tr><td align="right">Last_RX_Pkt_payload:</td><td><span id="x_RX_Last_Spot_payload"></span></td></tr>
<tr><td colspan="2"><hr></span></td></tr>

<tr><td align="right">Last_RX_Path:</td><td><span id="x_RX_Last_Path_Report"></span></td></tr>
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
<tr><td align="right">LoRa_CRC_errored_packets:</td><td><span id="x_LoRa_CRC_errored_packets"></span></td></tr>

<tr><td align="right">LoRa_UMN_errored_packets:</td><td><span id="x_LoRa_UMN_errored_packets"></span></td></tr>
<tr><td align="right">LoRa_CAD_errors:</td><td><span id="x_LoRa_CAD_errors"></span></td></tr>
<tr><td align="right">LoRa_ReSched_packets:</td><td><span id="x_LoRa_ReSched_packets"></span></td></tr>

<!--
<tr><td colspan="2"><hr></span></td></tr>
<tr><td align="right">AprsIS_rx_packets:</td><td><span id="x_AprsIS_rx_packets"></span></td></tr>
<tr><td align="right">AprsIS_tx_packets:</td><td><span id="x_AprsIS_tx_packets"></span></td></tr>
<tr><td align="right">AprsIS_dropped_packets:</td><td><span id="x_AprsIS_dropped_packets"></span></td></tr>
<tr><td align="right">AprsIS_relayed_packets:</td><td><span id="x_AprsIS_relayed_packets"></span></td></tr>
-->

<tr><td colspan="2"><hr></span></td></tr>
<tr><td align="right">Kiss_rx_packets:</td><td><span id="x_Kiss_rx_packets"></span></td></tr>
<tr><td align="right">Kiss_tx_packets:</td><td><span id="x_Kiss_tx_packets"></span></td></tr>


<tr><td colspan="2"><hr></span></td></tr>
<tr><td align="right">IPC_lost_msgs:</td><td><span id="x_IPC_lost_msgs"></span></td></tr>

<tr><td colspan="2"><hr></span></td></tr>
<tr><td align="right">LoRa_OnAirTime(msec):</td><td><span id="x_LoRa_OnAirTime"></span></td></tr>
<tr><td align="right">LoRa_DutyCycle(%):</td><td><span id="x_LoRa_DutyCycle"></span></td></tr>
<tr><td align="right">LoRa_ChanCong(%):</td><td><span id="x_ChanCong"></span></td></tr>
<tr><td align="right">LoRa_ENL(dbm):</td><td><span id="x_LoRa_ENL"></span></td></tr>
<tr><td align="right">LoRa_FreqJitter(ppm):</td><td><span id="x_LoRa_FreqJitter"></span></td></tr>
<tr><td align="right">CPU Temperature(C°):</td><td><span id="x_cpu_temp"></span></td></tr>
<tr><td align="right">CPU_UpTime(secs):</td><td><span id="x_Processor_UpTime"></span></td></tr>

<tr><td colspan="2" align="center"><a href="javascript:GetState()" class="btn btn--m btn--blue">Refresh</a></td></tr>
</table>
<script>

function GetState()
{
  setValues("/admin/dashboardvalues");
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
void send_dashboard_values_html () ;

#endif
