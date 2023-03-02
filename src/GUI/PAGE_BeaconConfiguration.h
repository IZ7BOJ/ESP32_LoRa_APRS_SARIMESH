// PAGE_BeaconConfiguration.h
#ifndef PBC1_H
#define PBC1_H

#include "rdbg.h"
#include <Arduino.h>
#include "ezTime_loc.h"                   // local fork for the ezTime library to support RTC and GPS in addition to NTP synch
#include <WebServer.h>  
#include "../configuration_data_definition.h"
#include "helpers.h"

extern volatile bool WebConfig_debug ;  
extern strDateTime DateTime;
extern Timezone Italy;
extern Timezone UTC;
extern WebServer web_server;
extern strConfig ESP_Config;
extern volatile uint16_t AdminTimeOutCounter ;
extern String SW_Revision ;
extern bool firstStart ;      
extern volatile bool save_config_enable ;
extern char has_SD_Card[10];
extern char SD_Card_Size[10];

// Beacon app parameters
extern  byte BeaconId ;
extern  bool BeaconSeqNbr;
extern  byte BeaconLocation ;
extern  byte BeaconEngineType ;
extern  byte BeaconRun ;
extern  byte BCN_TimeSlotOffset ;
extern  byte BCN_TimeSlotSync ;
extern  byte BeaconStatsCollector_IP[4];
extern  int BeaconStatsCollector_Port ;
extern  String BeaconQRALocator;
extern  String BCN_LoRa_Vector ;  
extern  bool BeaconUnixTime ;  //
extern  bool BeaconFreq ;   //
extern  bool BeaconPower ;   //
extern  bool BeaconWorkConditions ; //
extern volatile uint8_t LoraDeviceType ;  

void WriteConfig(void) ;


//
//  HTML PAGE
//
const char PAGE_BeaconConfiguration_2g[] PROGMEM = R"=====(
<meta name="viewport" content="width=device-width, initial-scale=1" />
<meta http-equiv="Content-Type" content="text/html; charset=utf-8" />
<a href="admin.html"  class="btn btn--s"><</a>&nbsp;&nbsp;<strong>Beacon Configuration</strong>
<hr>
Beacon Configuration:<br>
<form action="" method="get">
<table border="0"  cellspacing="0" cellpadding="3" style="width:310px" >

<tr><td colspan="2"><hr></span></td></tr>
<tr><td align="right">Payload content:</td><td></td></tr>
<tr><td colspan="2"><hr></span></td></tr>

<tr><td align="right">BeaconId:</td><td>$<input  type="text" id="BeaconId" name="BeaconId" value="" size="3"> 2 bytes</td></tr>

<tr><td align="right">BeaconSeqNbr:</td><td>
<select  id="BeaconSeqNbr" name="BeaconSeqNbr">
  <option value="0">disable</option>
  <option value="1">enable (+1 byte)</option>
</select>
</td></tr>

<tr><td align="right">BeaconUnixTime:</td><td>
<select  id="BeaconUnixTime" name="BeaconUnixTime">
  <option value="0">disable</option>
  <option value="1">enable (+4 byte)</option>
</select>
</td></tr>

<tr><td align="right">BeaconLocation:</td><td>
<select  id="BeaconLocation" name="BeaconLocation">
  <option value="0">disable</option>
  <option value="1">GPS_location (+8 byte)</option>
  <option value="2">QRA_Locator (+8 byte)</option>  
</select>
</td></tr>

<tr><td align="right">BeaconFreq:</td><td>
<select  id="BeaconFreq" name="BeaconFreq">
  <option value="0">disable</option>
  <option value="1">enable (+4 byte)</option>
</select>
</td></tr>


<tr><td align="right">BeaconPower:</td><td>
<select  id="BeaconPower" name="BeaconPower">
  <option value="0">disable</option>
  <option value="1">enable (+1 byte)</option>
</select>
</td></tr>

<tr><td align="right">BeaconWorkConditions:</td><td>
<select  id="BeaconWorkConditions" name="BeaconWorkConditions">
  <option value="0">disable</option>
  <option value="1">enable (+2 byte)</option>
</select>
</td></tr>

<tr><td colspan="2"><hr></span></td></tr>
<tr><td align="left">Beacon Operation:</td><td></td></tr>
<tr><td colspan="2"><hr></span></td></tr>

<tr><td align="right">BeaconEngineType:</td><td>
<select  id="BeaconEngineType" name="BeaconEngineType">
  <option value="0">txEnable</option>
  <option value="1">rxOnly</option>
<!--  
  <option value="2">alternating</option>  
  <option value="3">idBased</option>    
-->  
</select>
</td></tr>

<tr><td align="right">BeaconRun:</td><td>
<select  id="BeaconRun" name="BeaconRun">
  <option value="0">singlePhase_10</option>
  <option value="1">singlePhase_18</option>
  <option value="2">singlePhase_23</option>
  <option value="3">singlePhase_100</option>  
<!--    
  <option value="4">shortCalendar</option>
  <option value="5">longCalendar</option> 
-->
  <option value="6">tuningMode</option>    
</select>
</td></tr>


<tr><td colspan="2"><hr></span></td></tr>
<tr><td align="left">singlePhase_Config</td><td></td></tr>
<tr><td colspan="2"><hr></span></td></tr>

<tr><td align="right">BCN_LoRa_Vector:</td><td><input  type="text" id="BCN_LoRa_Vector" name="BCN_LoRa_Vector" value="" size="40" ></td></tr>
<tr><td align="right">(format)</td><td><font size=2>sqnbr, loraFreq,  bw,  sf,  cr,  sync, pwr, ppm, prlen</td></tr>
<tr><td align="right">(example)</td><td><font size=3>0,     433.725, 7.8,  11,   8,  0x34,  10,   0,  15</td></tr>

<tr><td align="right">BCN_TimeSlotOffset(secs):</td><td><input  type="text" id="BCN_TimeSlotOffset" name="BCN_TimeSlotOffset" value=""></td></tr>
<!--
<tr><td align="right">BCN_TimeSlotSync:</td><td><input  type="text" id="BCN_TimeSlotSync" name="BCN_TimeSlotSync" value=""></td></tr>
-->
<tr><td align="right">BCN_TimeSlotSync:</td><td>
<select  id="BCN_TimeSlotSync" name="BCN_TimeSlotSync">
  <option value="0">halfMinute</option>
  <option value="1">OnMinute</option>
  <option value="2">OnMinuteModulo_2</option>
  <option value="3">OnMinuteModulo_3</option> 
  <option value="4">OnMinuteModulo_4</option> 
  <option value="5">OnMinuteModulo_5</option> 
  <option value="6">OnMinuteModulo_6</option> 
  <option value="7">OnMinuteModulo_10</option> 
  <option value="8">OnMinuteModulo_15</option> 
  <option value="9">OnMinuteModulo_30</option> 
  <option value="10">OnMinuteModulo_60</option> 
  <option value="11">OnMinuteModulo_120</option>   
</select>
</td></tr>

<tr><td colspan="2"><hr></span></td></tr>
<tr><td align="left">BeaconStatsCollector</td><td></td></tr>
<tr><td colspan="2"><hr></span></td></tr>

<tr><td align="right">StatsCollector_IP:     </td><td><input type="text" id="ip_0" name="ip_0" size="3">.<input type="text" id="ip_1" name="ip_1" size="3">.<input type="text" id="ip_2" name="ip_2" size="3">.<input type="text" id="ip_3" name="ip_3" value="" size="3"></td></tr>
<!--
<tr><td align="right">BeaconStatsCollector_IP:</td><td><input  type="text" id="BeaconStatsCollector_IP" name="BeaconStatsCollector_IP" value=""></td></tr>
-->
<tr><td align="right">StatsCollector_Port:</td><td><input  type="text" id="BeaconStatsCollector_Port" name="BeaconStatsCollector_Port" value=""></td></tr>

<tr><td colspan="2"><hr></span></td></tr>

<tr><td colspan="2" align="center"><input type="submit" style="width:150px" class="btn btn--m btn--blue" value="Save"></td></tr>

<!-- <tr><td colspan="2" align="center"><a href="beacon.html" class="btn btn--m btn--blue">Refresh</a></td></tr> -->
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
					setValues("/admin/Beaconvalues");
		});
	});
}
function load(e,t,n){if("js"==t){var a=document.createElement("script");a.src=e,a.type="text/javascript",a.async=!1,a.onload=function(){n()},document.getElementsByTagName("head")[0].appendChild(a)}else if("css"==t){var a=document.createElement("link");a.href=e,a.rel="stylesheet",a.type="text/css",a.async=!1,a.onload=function(){n()},document.getElementsByTagName("head")[0].appendChild(a)}}




</script>


)=====";


//
//  HTML PAGE
//
const char PAGE_BeaconConfiguration_1g[] PROGMEM = R"=====(
<meta name="viewport" content="width=device-width, initial-scale=1" />
<meta http-equiv="Content-Type" content="text/html; charset=utf-8" />
<a href="admin.html"  class="btn btn--s"><</a>&nbsp;&nbsp;<strong>Beacon Configuration</strong>
<hr>
Beacon Configuration:<br>
<form action="" method="get">
<table border="0"  cellspacing="0" cellpadding="3" style="width:310px" >


<tr><td colspan="2"><hr></span></td></tr>
<tr><td align="right">Payload content:</td><td></td></tr>
<tr><td align="right">BeaconId:</td><td>$<input  type="text" id="BeaconId" name="BeaconId" value="" size="3"> 2 bytes</td></tr>

<tr><td align="right">BeaconSeqNbr:</td><td>
<select  id="BeaconSeqNbr" name="BeaconSeqNbr">
  <option value="0">disable</option>
  <option value="1">enable (+1 byte)</option>
</select>
</td></tr>

<tr><td align="right">BeaconUnixTime:</td><td>
<select  id="BeaconUnixTime" name="BeaconUnixTime">
  <option value="0">disable</option>
  <option value="1">enable (+4 byte)</option>
</select>
</td></tr>

<tr><td align="right">BeaconLocation:</td><td>
<select  id="BeaconLocation" name="BeaconLocation">
  <option value="0">disable</option>
  <option value="1">GPS_location (+8 byte)</option>
  <option value="2">QRA_Locator (+8 byte)</option>  
</select>
</td></tr>

<tr><td align="right">BeaconFreq:</td><td>
<select  id="BeaconFreq" name="BeaconFreq">
  <option value="0">disable</option>
  <option value="1">enable (+4 byte)</option>
</select>
</td></tr>


<tr><td align="right">BeaconPower:</td><td>
<select  id="BeaconPower" name="BeaconPower">
  <option value="0">disable</option>
  <option value="1">enable (+1 byte)</option>
</select>
</td></tr>

<tr><td align="right">BeaconWorkConditions:</td><td>
<select  id="BeaconWorkConditions" name="BeaconWorkConditions">
  <option value="0">disable</option>
  <option value="1">enable (+2 byte)</option>
</select>
</td></tr>


<tr><td colspan="2"><hr></span></td></tr>
<tr><td align="left">Beacon Operation:</td><td></td></tr>
<tr><td colspan="2"><hr></span></td></tr>

<tr><td align="right">BeaconEngineType:</td><td>
<select  id="BeaconEngineType" name="BeaconEngineType">
  <option value="0">txEnable</option>
  <option value="1">rxOnly</option>
<!--  
  <option value="2">alternating</option>  
  <option value="3">idBased</option>    
-->  
</select>
</td></tr>

<tr><td align="right">BeaconRun:</td><td>
<select  id="BeaconRun" name="BeaconRun">
  <option value="0">singlePhase_10</option>
  <option value="1">singlePhase_18</option>
  <option value="2">singlePhase_23</option>
  <option value="3">singlePhase_100</option>  
<!--    
  <option value="4">shortCalendar</option>
  <option value="5">longCalendar</option> 
--> 
  <option value="6">tuningMode</option>         
</select>
</td></tr>


<tr><td colspan="2"><hr></span></td></tr>
<tr><td align="left">singlePhase_Config</td><td></td></tr>
<tr><td colspan="2"><hr></span></td></tr>

<tr><td align="right">BCN_LoRa_Vector:</td><td><input  type="text" id="BCN_LoRa_Vector" name="BCN_LoRa_Vector" value="" size="40" ></td></tr>
<tr><td align="right">(format)</td><td><font size=2>sqnbr, loraFreq,  bw,  sf,  cr,  sync, pwr, ppm, prlen</td></tr>
<tr><td align="right">(example)</td><td><font size=2>0,     433.725, 7.8,  11,   8,  0x34,  10,   0,  15</td></tr>
<tr><td align="right">BCN_TimeSlotOffset(secs):</td><td><input  type="text" id="BCN_TimeSlotOffset" name="BCN_TimeSlotOffset" value=""></td></tr>
<!--
<tr><td align="right">BCN_TimeSlotSync:</td><td><input  type="text" id="BCN_TimeSlotSync" name="BCN_TimeSlotSync" value=""></td></tr>
-->
<tr><td align="right">BCN_TimeSlotSync:</td><td>
<select  id="BCN_TimeSlotSync" name="BCN_TimeSlotSync">
  <option value="0">halfMinute</option>
  <option value="1">OnMinute</option>
  <option value="2">OnMinuteModulo_2</option>
  <option value="3">OnMinuteModulo_3</option> 
  <option value="4">OnMinuteModulo_4</option> 
  <option value="5">OnMinuteModulo_5</option> 
  <option value="6">OnMinuteModulo_6</option> 
  <option value="7">OnMinuteModulo_10</option> 
  <option value="8">OnMinuteModulo_15</option> 
  <option value="9">OnMinuteModulo_30</option> 
  <option value="10">OnMinuteModulo_60</option> 
  <option value="11">OnMinuteModulo_120</option>   
</select>
</td></tr>


<tr><td colspan="2"><hr></span></td></tr>
<tr><td align="left">BeaconStatsCollector</td><td></td></tr>
<tr><td colspan="2"><hr></span></td></tr>

<tr><td align="right">StatsCollector_IP:     </td><td><input type="text" id="ip_0" name="ip_0" size="3">.<input type="text" id="ip_1" name="ip_1" size="3">.<input type="text" id="ip_2" name="ip_2" size="3">.<input type="text" id="ip_3" name="ip_3" value="" size="3"></td></tr>
<!--
<tr><td align="right">BeaconStatsCollector_IP:</td><td><input  type="text" id="BeaconStatsCollector_IP" name="BeaconStatsCollector_IP" value=""></td></tr>
-->
<tr><td align="right">StatsCollector_Port:</td><td><input  type="text" id="BeaconStatsCollector_Port" name="BeaconStatsCollector_Port" value=""></td></tr>


<tr><td colspan="2"><hr></span></td></tr>

<tr><td colspan="2" align="center"><input type="submit" style="width:150px" class="btn btn--m btn--blue" value="Save"></td></tr>

<!-- <tr><td colspan="2" align="center"><a href="beacon.html" class="btn btn--m btn--blue">Refresh</a></td></tr> -->
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
          setValues("/admin/Beaconvalues");
    });
  });
}
function load(e,t,n){if("js"==t){var a=document.createElement("script");a.src=e,a.type="text/javascript",a.async=!1,a.onload=function(){n()},document.getElementsByTagName("head")[0].appendChild(a)}else if("css"==t){var a=document.createElement("link");a.href=e,a.rel="stylesheet",a.type="text/css",a.async=!1,a.onload=function(){n()},document.getElementsByTagName("head")[0].appendChild(a)}}




</script>


)=====";


const char PAGE_BeaconWaitAndReload[] PROGMEM = R"=====(
<meta http-equiv="refresh" content="5; URL=beacon.html">
Please Wait....Configuring and Restarting.
)=====";


//
//  SEND HTML PAGE OR IF A FORM SUMBITTED VALUES, PROCESS THESE VALUES
// 
void send_beacon_configuration_html() ;
//
//   FILL THE PAGE WITH VALUES
//
void send_beacon_configuration_values_html() ;

#endif 
