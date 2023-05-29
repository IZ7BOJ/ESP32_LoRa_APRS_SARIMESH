// PAGE_PayloadEncapConfiguration.h
#ifndef P_ENCAPC_H
#define P_ENCAPC_H

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
extern char has_SD_Card[10];
extern char SD_Card_Size[10];
extern volatile byte EncapType ;
extern volatile byte PayloadStyle ;
extern volatile byte RepeaterOperation ;
extern volatile byte LoraSync ;
extern volatile byte LocationCompression ;
extern volatile byte BlackList ;

extern volatile uint8_t AgileBeaconing ;

void WriteConfig(void) ;



//
//  HTML PAGE
//
const char PAGE_PayloadEncapConfiguration[] PROGMEM = R"=====(
<meta name="viewport" content="width=device-width, initial-scale=1" />
<meta http-equiv="Content-Type" content="text/html; charset=utf-8" />
<a href="admin.html"  class="btn btn--s"><</a>&nbsp;&nbsp;<strong>APRS Compatibility</strong>
<hr>
Payload Encapsulation Configuration:<br>

<form action="" method="get">
<table border="0"  cellspacing="0" cellpadding="3" style="width:310px" >
<tr><td align="left"> </td><td></td></tr>

<tr><td align="right">Payload Encapsulation:</td><td>
<select  id="EncapType" name="EncapType">
  <option value="0">Native LoRa</option> 
  <option value="1">APRS AX25</option>
  <option value="2">APRS OE Style</option>
  
</select>
</td></tr>
</table>

<hr>
Payload Style Configuration:<br>

<table border="0"  cellspacing="0" cellpadding="3" style="width:310px" >
<tr><td align="right">Payload Style:</td><td>
<select  id="PayloadStyle" name="PayloadStyle">
  <option value="0">Sarimesh LoRa</option> 
  <option value="1">Pure OE_Style</option>
  <option value="2">Modified OE_Style</option>
</select>
</td></tr>
</table>

<hr>
Repeater operation Configuration:<br>

<table border="0"  cellspacing="0" cellpadding="3" style="width:310px" >
<tr><td align="right">Repeater operation:</td><td>
<select  id="RepeaterOperation" name="RepeaterOperation">
  <option value="0">Sarimesh(enabled)</option> 
  <option value="1">OE_Style(disabled)</option>
</select>
</td></tr>
</table>


<hr>
TX Location Compression Configuration:<br>

<table border="0"  cellspacing="0" cellpadding="3" style="width:310px" >
<tr><td align="right">LocationCompression:</td><td> 
<select  id="LocationCompression" name="LocationCompression">
  <option value="0">Disable</option> 
  <option value="1">Enable</option>
</select>
</td></tr>
</table>

<hr>
TX Agile Beaconing Configuration:<br>

<table border="0"  cellspacing="0" cellpadding="3" style="width:310px" >
<tr><td align="right">AgileBeconing:</td><td> 
<select  id="AgileBeaconing" name="AgileBeaconing">
  <option value="0">Disable</option> 
  <option value="1">EventMode</option>
  <option value="2">MappingModeUltraFine</option>
  <option value="3">MappingModeDetailed</option>
  <option value="4">MappingModeCourse</option>
  <option value="5">MappingModeUltraCourse</option>
</select>
</td></tr>
</table>

<hr>
Beacon Blacklist Configuration:<br>

<table border="0"  cellspacing="0" cellpadding="3" style="width:310px" >
<tr><td align="right">Black list :</td><td> 
<select  id="BlackList" name="BlackList">
  <option value="0">Disable</option> 
  <option value="1">Enable</option>
</select>
</td></tr>
</table>



<hr>
LoraSync word Configuration:<br>

<table border="0"  cellspacing="0" cellpadding="3" style="width:310px" >
<tr><td align="right">LoraSync_word:</td><td>
<!-- <input type="text" id="LoraSync" name="LoraSync" value=""> -->
<select  id="LoraSync" name="LoraSync">
  <option value="52">0x34</option>
  <option value="18">0x12</option>
  
</select>
</td></tr>

<tr><td colspan="2" align="center"><input type="submit" style="width:150px" class="btn btn--m btn--blue" value="Save"></td></tr> 

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
					setValues("/admin/Encapvalues");
		});
	});
}
function load(e,t,n){if("js"==t){var a=document.createElement("script");a.src=e,a.type="text/javascript",a.async=!1,a.onload=function(){n()},document.getElementsByTagName("head")[0].appendChild(a)}else if("css"==t){var a=document.createElement("link");a.href=e,a.rel="stylesheet",a.type="text/css",a.async=!1,a.onload=function(){n()},document.getElementsByTagName("head")[0].appendChild(a)}}




</script>


)=====";


const char PAGE_PayloadEncapConfiguration_kiss[] PROGMEM = R"=====(
<meta name="viewport" content="width=device-width, initial-scale=1" />
<meta http-equiv="Content-Type" content="text/html; charset=utf-8" />
<a href="admin.html"  class="btn btn--s"><</a>&nbsp;&nbsp;<strong>APRS Compatibility</strong>
<hr>
Payload Encapsulation Configuration:<br>

<form action="" method="get">
<table border="0"  cellspacing="0" cellpadding="3" style="width:310px" >
<tr><td align="left"> </td><td></td></tr>

<tr><td align="right">Payload Encapsulation:</td><td>
<select  id="EncapType" name="EncapType">
  <option value="0">Native LoRa</option> 
  <option value="1">APRS AX25</option>
  <option value="2">APRS OE Style</option>
  
</select>
</td></tr>
</table>


<hr>
Payload Style Configuration:<br>

<table border="0"  cellspacing="0" cellpadding="3" style="width:310px" >
<tr><td align="right">Payload Style:</td><td>
<select  id="PayloadStyle" name="PayloadStyle" disabled>
  <option value="0">Sarimesh LoRa</option> 
  <option value="1">Pure OE_Style</option>
  <option value="2">Modified OE_Style</option>
</select>
</td></tr>
</table>

<hr>
Repeater operation Configuration:<br>

<table border="0"  cellspacing="0" cellpadding="3" style="width:310px" >
<tr><td align="right">Repeater operation:</td><td>
<select  id="RepeaterOperation" name="RepeaterOperation" disabled>
  <option value="0">Sarimesh(enabled)</option> 
  <option value="1">OE_Style(disabled)</option>
</select>
</td></tr>
</table>

<hr>
LoraSync word Configuration:<br>

<table border="0"  cellspacing="0" cellpadding="3" style="width:310px" >
<tr><td align="right">LoraSync_word:</td><td>
<!-- <input type="text" id="LoraSync" name="LoraSync" value=""> -->
<select  id="LoraSync" name="LoraSync">
  <option value="52">0x34</option>
  <option value="18">0x12</option>
  
</select>
</td></tr>

<tr><td colspan="2" align="center"><input type="submit" style="width:150px" class="btn btn--m btn--blue" value="Save"></td></tr> 

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
          setValues("/admin/Encapvalues");
    });
  });
}
function load(e,t,n){if("js"==t){var a=document.createElement("script");a.src=e,a.type="text/javascript",a.async=!1,a.onload=function(){n()},document.getElementsByTagName("head")[0].appendChild(a)}else if("css"==t){var a=document.createElement("link");a.href=e,a.rel="stylesheet",a.type="text/css",a.async=!1,a.onload=function(){n()},document.getElementsByTagName("head")[0].appendChild(a)}}




</script>


)=====";

const char PAGE_EncapWaitAndReload[] PROGMEM = R"=====(
<meta http-equiv="refresh" content="5; URL=encap.html">
Please Wait....Configuring and Restarting.
)=====";


//
//  SEND HTML PAGE OR IF A FORM SUMBITTED VALUES, PROCESS THESE VALUES
// 
void send_encap_configuration_html()  ;
void send_encap_configuration_html_kiss() ;
//
//   FILL THE PAGE WITH VALUES
//
void send_encap_configuration_values_html() ;


#endif  
