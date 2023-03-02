 // PAGE_AprsConfiguration.h
#ifndef PAGE_APRSC_H
#define PAGE_APRSC_H

#include "rdbg.h"
#include <Arduino.h>
#include <WebServer.h> 
#include "../configuration_data_definition.h"
#include "helpers.h"

extern strConfig ESP_Config;
extern WebServer web_server;
extern volatile uint16_t AdminTimeOutCounter ;
extern volatile bool WebConfig_debug ;  
extern String NodeName ;    
extern char has_SD_Card[10];
extern char SD_Card_Size[10];

void WriteConfig(void) ;
void send_aprs_configuration_html();
void send_aprs_configuration_values_html() ;


//
//  HTML PAGE
//
const char PAGE_AprsConfiguration[] PROGMEM = R"=====(
<meta name="viewport" content="width=device-width, initial-scale=1" />
<meta http-equiv="Content-Type" content="text/html; charset=utf-8" />
<a href="admin.html"  class="btn btn--s"><</a>&nbsp;&nbsp;<strong>Aprs Configuration</strong>
<hr>
<!-- APRS Configuration:<br> -->
<form action="" method="get">
<table border="0"  cellspacing="0" cellpadding="3" style="width:310px" >
<tr><td align="left"> </td><td></td></tr>
<tr><td align="left">Location Coordinates</td><td></td></tr>
<tr><td align="right">static Latitude:</td><td><input type="text" id="Loc_Lat" name="Loc_Lat" value=""  ></td></tr>
<tr><td align="right">static Longitude:</td><td><input type="text" id="Loc_Lon" name="Loc_Lon" value=""  ></td></tr>

<tr><td align="left">APRS/IS iGate</td><td></td></tr>
<tr><td align="right">APRS_Host:</td><td><input type="text" id="AprsHost" name="AprsHost" value="" disabled ></td></tr>
<tr><td align="right">APRS_Port:</td><td><input type="text" id="AprsPort" name="AprsPort" value="" disabled ></td></tr>
<tr><td align="right">APRS_Login:</td><td><input type="text" id="AprsLogin" name="AprsLogin" value=""></td></tr>
<tr><td align="right">APRS_Pass:</td><td><input type="password" id="AprsPass" name="AprsPass" value=""></td></tr>
<tr><td align="right">APRS_Filter:</td><td>
<!-- <input type="text" id="AprsFilter" name="AprsFilter" value=""> -->
<select  id="AprsFilter" name="AprsFilter">
  <option value="m/1">max 2 km</option> 
  <option value="m/2">max 4 km</option>   
  <option value="m/5">max 10 km</option>
  <option value="m/10">max 20 km</option>
  <option value="m/20">max 40 km</option>
  <option value="m/30">max 60 km</option>
  <option value="m/40">max 80 km</option>
  <option value="m/80">max 160 km</option>
  <option value="m/160">max 320 km</option>
</select>




</td></tr>
<tr><td align="right">iGate_Beacon:</td><td><input type="text" id="AprsRawBeaconIGATE" name="AprsRawBeaconIGATE" value="" ></td></tr>
<tr><td align="right"><font size=1>(format)</td><td><font size=1>callsign,path,message,symtab,symcode</td></tr>
<tr><td align="right"><font size=1>(example)</td><td><font size=1>I1XYZ-10,WIDE1-1,LoRa iGate,&,L</td></tr>

<tr><td align="right">iGate_Beacon_Int:</td><td>
<!-- <input type="text" id="AprsRawBeaconPeriodSecondsIGATE" name="AprsRawBeaconPeriodSecondsIGATE" value=""> -->

<select  id="AprsRawBeaconPeriodSecondsIGATE" name="AprsRawBeaconPeriodSecondsIGATE">
  <option value="30">30 secs.</option> 
  <option value="60">1 min.</option> 
  <option value="120">2 min.</option> 
  <option value="180">3 min.</option>
  <option value="240">4 min.</option>
  <option value="300">5 min.</option>
  <option value="420">7 min.</option>
  <option value="600">10 min.</option>
  <option value="1200">20 min.</option>
  <option value="1800">30 min.</option>
  <option value="3600">60 min.</option>
  <option value="7200">120 min.</option>
</select>
</td></tr>
<tr><td align="left"> </td><td></td></tr>
<tr><td align="left">APRS Tracker</td><td></td></tr>
<tr><td align="right">Tracker_Beacon:</td><td><input type="text" id="AprsRawBeaconTRACK" name="AprsRawBeaconTRACK" value="" </td></tr>
<tr><td align="right"><font size=1>(format)</td><td><font size=1>callsign,path,message,symtab,symcode</td></tr>
<tr><td align="right"><font size=1>(example)</td><td><font size=1>I1XYZ-10,WIDE1-1,LoRa iGate,&,L</td></tr>

<tr><td align="right">Tracker_Beacon_Int:</td><td>
<!-- <input type="text" id="AprsRawBeaconPeriodSecondsTRACK" name="AprsRawBeaconPeriodSecondsTRACK" value="" > -->
<select  id="AprsRawBeaconPeriodSecondsTRACK" name="AprsRawBeaconPeriodSecondsTRACK">
  <option value="4">4 secs.</option> 
  <option value="9">9 secs.</option> 
  <option value="14">14 secs.</option> 
  <option value="19">19 secs.</option> 
  <option value="24">24 secs.</option>   
  <option value="30">30 secs.</option> 
  <option value="60">60 secs.</option>
  <option value="120">2 min.</option>
  <option value="180">3 min.</option>
  <option value="240">4 min.</option>
  <option value="300">5 min.</option>
  <option value="420">7 min.</option>
  <option value="600">10 min.</option>
  <option value="1200">20 min.</option>
  <option value="1800">30 min..</option>
  <option value="3600">60 min..</option>
</select>
</td></tr>

<tr><td align="left">APRS Logger</td><td></td></tr>
<tr><td align="right">APRS Logger Host:</td><td><input type="text" id="AprsLoggerAddr" name="AprsLoggerAddr" value=""  ></td></tr>
<tr><td align="right">APRS Logger Port:</td><td><input type="text" id="AprsLoggerPort" name="AprsLoggerPort" value=""  ></td></tr>

<tr><td colspan="2" align="center"><input type="submit" style="width:150px" class="btn btn--m btn--blue" value="Save"></td></tr> 
<!-- <tr><td colspan="2" align="center"><a href="aprs.html" class="btn btn--m btn--blue">Refresh</a></td></tr> -->
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
					setValues("/admin/Aprsvalues");
		});
	});
}
function load(e,t,n){if("js"==t){var a=document.createElement("script");a.src=e,a.type="text/javascript",a.async=!1,a.onload=function(){n()},document.getElementsByTagName("head")[0].appendChild(a)}else if("css"==t){var a=document.createElement("link");a.href=e,a.rel="stylesheet",a.type="text/css",a.async=!1,a.onload=function(){n()},document.getElementsByTagName("head")[0].appendChild(a)}}




</script>


)=====";

const char PAGE_AprsWaitAndReload[] PROGMEM = R"=====(
<meta http-equiv="refresh" content="5; URL=aprs.html">
Please Wait....Configuring and Restarting.
)=====";


//
//  SEND HTML PAGE OR IF A FORM SUMBITTED VALUES, PROCESS THESE VALUES
// 
void send_aprs_configuration_html() ;
//
//   FILL THE PAGE WITH VALUES
//
void send_aprs_configuration_values_html() ;

#endif 
