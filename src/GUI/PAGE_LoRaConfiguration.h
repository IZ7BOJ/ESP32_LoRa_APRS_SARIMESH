// PAGE_LoRaConfiguration.h
#ifndef PLORA_H
#define PLORA_H

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
extern String LoRaDevice;
extern volatile uint8_t LoraDeviceType ;
extern char has_SD_Card[10];
extern char SD_Card_Size[10];
extern volatile byte LoraSync ;
extern volatile float ppm_runtime;

void WriteConfig(void) ;


//
//  HTML PAGE
//
const char PAGE_LoRaConfiguration_2g[] PROGMEM = R"=====(
<meta name="viewport" content="width=device-width, initial-scale=1" />
<meta http-equiv="Content-Type" content="text/html; charset=utf-8" />
<a href="admin.html"  class="btn btn--s"><</a>&nbsp;&nbsp;<strong>LoRa APRS Configuration</strong>
<hr>
LoRa APRS Main Configuration:<br>
<form action="" method="get">
<table border="0"  cellspacing="0" cellpadding="3" style="width:310px" >
<tr><td align="right">LoraFreq:</td><td><input type="text" id="LoraFreq" name="LoraFreq" value=""></td></tr>
<tr><td align="right">LoraBw:</td><td>
<!-- <input type="text" id="LoraBw" name="LoraBw" value=""> -->
<select  id="LoraBw" name="LoraBw">
  <option value="7.8">7.8KHz</option>
  <option value="10.4">10.4KHz</option>
  <option value="15.6">15.6KHz</option>
  <option value="20.8">20.8KHz</option>
  <option value="31.25">31.25KHz</option>
  <option value="41.7">41.7KHz</option>
  <option value="62.5">62.5KHz</option>
  <option value="125">125KHz</option>
  <option value="250">250KHz</option>
  <option value="500">500KHz</option>  
</select>
</td></tr>
<tr><td align="right">LoraSf:</td><td>
<!-- <input type="text" id="LoraSf" name="LoraSf" value=""> -->
<select  id="LoraSf" name="LoraSf">
  <option value="5" disabled>5</option> 
  <option value="6">6</option>
  <option value="7">7</option>
  <option value="8">8</option>
  <option value="9">9</option>
  <option value="10">10</option>
  <option value="11">11</option>
  <option value="12">12</option>
</select>
</td></tr>
<tr><td align="right">LoraCodingRate:</td><td>
<!-- <input type="text" id="LoraCodingRate" name="LoraCodingRate" value=""> -->
<select  id="LoraCodingRate" name="LoraCodingRate">
  <option value="5">4:5</option>
  <option value="6">4:6</option>
  <option value="7">4:7</option>
  <option value="8">4:8</option>
</select>
</td></tr>

<tr><td align="right">LoraPreambleLen:</td><td>
<!-- <input type="text" id="LoraPreambleLen" name="LoraPreambleLen" value=""> -->
<select  id="LoraPreambleLen" name="LoraPreambleLen">
  <option value="4">4</option>
  <option value="6">6</option>
  <option value="8">8</option>
  <option value="10">10</option>  
  <option value="12">12</option>
  <option value="14">14</option>
  <option value="16">16</option>
  <option value="18">18</option>  
  <option value="20">20</option>
  <option value="22">22</option>
  <option value="24">24</option>  
</select>
</td></tr>


<tr><td align="right">LoraSync:</td><td>
<!-- <input type="text" id="LoraSync" name="LoraSync" value=""> -->
<select  id="LoraSync" name="LoraSync" disabled>
  <option value="52">0x34</option>
  <option value="18">0x12</option>
  
</select>
</td></tr>
<tr><td align="right">LoraPower:</td><td>
<!-- <input type="text" id="LoraPower" name="LoraPower" value=""> -->
<select  id="LoraPower" name="LoraPower">
  <option value="-9">-9 dbm</option> 
  <option value="-8">-8 dbm</option> 
  <option value="-7">-7 dbm</option> 
  <option value="-6">-6 dbm</option> 
  <option value="-5">-5 dbm</option> 
  <option value="-4">-4 dbm</option> 
  <option value="-3">-3 dbm</option> 
  <option value="-2">-2 dbm</option> 
  <option value="-1">-1 dbm</option> 
  <option value="0">0 dbm</option>
  <option value="1">+1 dbm</option>
  <option value="2">+2 dbm</option>
  <option value="3">+3 dbm</option>
  <option value="4">+4 dbm</option>
  <option value="5">+5 dbm</option>
  <option value="6">+6 dbm</option>
  <option value="7">+7 dbm</option>
  <option value="8">+8 dbm</option>
  <option value="9">+9 dbm</option>
  <option value="10">+10 dbm</option>
  <option value="11">+11 dbm</option>
  <option value="12">+12 dbm</option>
  <option value="13">+13 dbm</option>
  <option value="14">+14 dbm</option>   
  <option value="15">+15 dbm</option>
  <option value="16">+16 dbm</option>
  <option value="17">+17 dbm</option>
  <option value="18">+18 dbm</option>
  <option value="19">+19 dbm</option>
  <option value="20">+20 dbm</option>
  <option value="21">+21 dbm</option>
  <option value="22">+22 dbm</option>  
</select>

</td></tr>
<tr><td align="right">LoraFreqCorr(ppm * 10):</td><td><input type="text" id="LoraFreqCorr" name="LoraFreqCorr" value=""> </td></tr>
<tr><td align="right">LoRa_FreqJitter(ppm)now:</td><td><span id="x_LoRa_FreqJitter"></span></td></tr>

<!--
<tr><td align="right">LoRa_FreqJitter(ppm)now:</td><td><input type="text" id="LoRa_FreqJitter" name="LoRa_FreqJitter" disabled value=""> </td></tr>
-->


<tr><td colspan="2"><hr></span></td></tr>
<tr><td align="left">LoRa APRS Alter Configuration</td><td></td></tr>
<tr><td colspan="2"><hr></span></td></tr>

<tr><td align="right">ALT_LoRa_Vector:</td><td><input  type="text" id="ALT_LoRa_Vector" name="ALT_LoRa_Vector" value="" size="40" ></td></tr>
<tr><td align="right">(format)</td><td><font size=2>sqnbr, loraFreq,  bw,  sf,  cr,  sync, pwr, ppm, prlen</td></tr>
<tr><td align="right">(example)</td><td><font size=3>0,     433.725, 7.8,  11,   8,  0x34,  10,   0,  15</td></tr>

<tr><td align="right">ALT_TimeSlotSize(secs):</td><td><input  type="text" id="ALT_TimeSlotSize" name="ALT_TimeSlotSize" value=""></td></tr>

<tr><td align="right">ALT_TimeSlotMode:</td><td>
<select  id="ALT_TimeSlotMode" name="ALT_TimeSlotMode">
  <option value="0">tracker</option>
  <option value="1">iGate</option>
</select>
</td></tr>




<tr><td align="right">ALT_TimeSlotSync:</td><td>
<select  id="ALT_TimeSlotSync" name="ALT_TimeSlotSync">
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
  <option value="12">disable_alt_time_slot</option>    
</select>
</td></tr>



<tr><td colspan="2" align="center"><input type="submit" style="width:150px" class="btn btn--m btn--blue" value="Save"></td></tr>
<!-- <tr><td colspan="2" align="center"><a href="lora.html" class="btn btn--m btn--blue">Refresh</a></td></tr> -->
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
          setValues("/admin/LoRavalues");
    });
  });
}
function load(e,t,n){if("js"==t){var a=document.createElement("script");a.src=e,a.type="text/javascript",a.async=!1,a.onload=function(){n()},document.getElementsByTagName("head")[0].appendChild(a)}else if("css"==t){var a=document.createElement("link");a.href=e,a.rel="stylesheet",a.type="text/css",a.async=!1,a.onload=function(){n()},document.getElementsByTagName("head")[0].appendChild(a)}}




</script>


)=====";


//
//  HTML PAGE
//
const char PAGE_LoRaConfiguration_1g[] PROGMEM = R"=====(
<meta name="viewport" content="width=device-width, initial-scale=1" />
<meta http-equiv="Content-Type" content="text/html; charset=utf-8" />
<a href="admin.html"  class="btn btn--s"><</a>&nbsp;&nbsp;<strong>LoRa APRS Configuration</strong>
<hr>
LoRa APRS Main Configuration:<br>
<form action="" method="get">
<table border="0"  cellspacing="0" cellpadding="3" style="width:310px" >
<tr><td align="right">LoraFreq:</td><td><input type="text" id="LoraFreq" name="LoraFreq" value=""></td></tr>
<tr><td align="right">LoraBw:</td><td>
<!-- <input type="text" id="LoraBw" name="LoraBw" value=""> -->
<select  id="LoraBw" name="LoraBw">
  <option value="7.8">7.8KHz</option>
  <option value="10.4">10.4KHz</option>
  <option value="15.6">15.6KHz</option>
  <option value="20.8">20.8KHz</option>
  <option value="31.25">31.25KHz</option>
  <option value="41.7">41.7KHz</option>
  <option value="62.5">62.5KHz</option>
  <option value="125">125KHz</option>
  <option value="250">250KHz</option>
  <option value="500">500KHz</option>  
</select>
</td></tr>
<tr><td align="right">LoraSf:</td><td>
<!-- <input type="text" id="LoraSf" name="LoraSf" value=""> -->
<select  id="LoraSf" name="LoraSf">
  <option value="6" disabled>6</option>
  <option value="7">7</option>
  <option value="8">8</option>
  <option value="9">9</option>
  <option value="10">10</option>
  <option value="11">11</option>
  <option value="12">12</option>
</select>
</td></tr>
<tr><td align="right">LoraCodingRate:</td><td>
<!-- <input type="text" id="LoraCodingRate" name="LoraCodingRate" value=""> -->
<select  id="LoraCodingRate" name="LoraCodingRate">
  <option value="5">4:5</option>
  <option value="6">4:6</option>
  <option value="7">4:7</option>
  <option value="8">4:8</option>
</select>
</td></tr>

<tr><td align="right">LoraPreambleLen:</td><td>
<!-- <input type="text" id="LoraPreambleLen" name="LoraPreambleLen" value=""> -->
<select  id="LoraPreambleLen" name="LoraPreambleLen">
  <option value="6">6</option>
  <option value="8">8</option>
  <option value="10">10</option>  
  <option value="12">12</option>
  <option value="14">14</option>
  <option value="16">16</option>
  <option value="18">18</option>  
  <option value="20">20</option>
  <option value="22">22</option>
  <option value="24">24</option>  
</select>
</td></tr>


<tr><td align="right">LoraSync:</td><td>
<!-- <input type="text" id="LoraSync" name="LoraSync" value=""> -->
<select  id="LoraSync" name="LoraSync" disabled>
  <option value="52">0x34</option>
  <option value="18">0x12</option>  
</select>
</td></tr>

<tr><td align="right">LoraPower:</td><td>
<!-- <input type="text" id="LoraPower" name="LoraPower" value=""> -->
<select  id="LoraPower" name="LoraPower">
  <option value="2">+2 dbm</option>
  <option value="3">+3 dbm</option>
  <option value="4">+4 dbm</option>
  <option value="5">+5 dbm</option>
  <option value="6">+6 dbm</option>
  <option value="7">+7 dbm</option>
  <option value="8">+8 dbm</option>
  <option value="9">+9 dbm</option>
  <option value="10">+10 dbm</option>
  <option value="11">+11 dbm</option>
  <option value="12">+12 dbm</option>
  <option value="13">+13 dbm</option>
  <option value="14">+14 dbm</option>   
  <option value="15">+15 dbm</option>
  <option value="16">+16 dbm</option>
  <option value="17">+17 dbm</option>
</select>

</td></tr>

<tr><td align="right">LoraFreqCorr(ppm * 10):</td><td><input type="text" id="LoraFreqCorr" name="LoraFreqCorr" value=""> </td></tr>
<tr><td align="right">LoRa_FreqJitter(ppm)now:</td><td><span id="x_LoRa_FreqJitter"></span></td></tr>

<!--
<tr><td align="right">LoRa_FreqJitter(ppm)now:</td><td><input type="text" id="LoRa_FreqJitter" name="LoRa_FreqJitter" disabled value=""> </td></tr>
-->


<tr><td colspan="2"><hr></span></td></tr>
<tr><td align="left">LoRa APRS Alter Configuration</td><td></td></tr>
<tr><td colspan="2"><hr></span></td></tr>

<tr><td align="right">ALT_LoRa_Vector:</td><td><input  type="text" id="ALT_LoRa_Vector" name="ALT_LoRa_Vector" value="" size="40" ></td></tr>
<tr><td align="right">(format)</td><td><font size=2>sqnbr, loraFreq,  bw,  sf,  cr,  sync, pwr, ppm, prlen</td></tr>
<tr><td align="right">(example)</td><td><font size=3>0,     433.725, 7.8,  11,   8,  0x34,  10,   0,  15</td></tr>

<tr><td align="right">ALT_TimeSlotSize(secs):</td><td><input  type="text" id="ALT_TimeSlotSize" name="ALT_TimeSlotSize" value=""></td></tr>

<tr><td align="right">ALT_TimeSlotMode:</td><td>
<select  id="ALT_TimeSlotMode" name="ALT_TimeSlotMode">
  <option value="0">tracker</option>
  <option value="1">iGate</option>
</select>
</td></tr>


<tr><td align="right">ALT_TimeSlotSync:</td><td>
<select  id="ALT_TimeSlotSync" name="ALT_TimeSlotSync">
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
  <option value="12">disable_alt_time_slot</option>   
</select>
</td></tr>


<tr><td colspan="2" align="center"><input type="submit" style="width:150px" class="btn btn--m btn--blue" value="Save"></td></tr>
<!-- <tr><td colspan="2" align="center"><a href="lora.html" class="btn btn--m btn--blue">Refresh</a></td></tr> -->
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
					setValues("/admin/LoRavalues");
		});
	});
}
function load(e,t,n){if("js"==t){var a=document.createElement("script");a.src=e,a.type="text/javascript",a.async=!1,a.onload=function(){n()},document.getElementsByTagName("head")[0].appendChild(a)}else if("css"==t){var a=document.createElement("link");a.href=e,a.rel="stylesheet",a.type="text/css",a.async=!1,a.onload=function(){n()},document.getElementsByTagName("head")[0].appendChild(a)}}


</script>


)=====";


const char PAGE_LoRaWaitAndReload[] PROGMEM = R"=====(
<meta http-equiv="refresh" content="5; URL=lora.html">
Please Wait....Configuring and Restarting.
)=====";


//
//  SEND HTML PAGE OR IF A FORM SUMBITTED VALUES, PROCESS THESE VALUES
// 
void send_lora_configuration_html() ;
//
//   FILL THE PAGE WITH VALUES
//
void send_lora_configuration_values_html() ;

#endif 
