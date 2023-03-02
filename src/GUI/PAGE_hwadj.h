//PAGE_hwadj.h
#ifndef HWADJ_H
#define HWADJ_H

#include "rdbg.h"
#include <Arduino.h>
#include "ezTime_loc.h"                   // local fork for the ezTime library to support RTC and GPS in addition to NTP synch
#include "helpers.h"
#include <WebServer.h>  
#include "../configuration_data_definition.h"
#include <TinyGPS++.h>
#include <WebServer.h>

extern TinyGPSPlus gps;
extern strConfig ESP_Config;
extern WebServer web_server;
extern volatile bool WebConfig_debug ;  
extern strDateTime DateTime;
extern Timezone Italy;
extern Timezone UTC;
extern volatile uint16_t AdminTimeOutCounter ;
extern strConfig ESP_Config ;
extern String SW_Revision ;
extern WebServer web_server;
extern volatile uint8_t  hw_arch;   // 0= undefined ( get values from GUI) 1=SARIMESH , 2= TTGO , 3 = Heltec
extern volatile uint8_t  i2c_sda;
extern volatile uint8_t  i2c_scl;
extern volatile uint8_t  spi_sck;
extern volatile uint8_t  spi_miso;
extern volatile uint8_t  spi_mosi;
extern volatile uint8_t  red_led;
extern volatile uint8_t  green_led;
extern volatile uint8_t  oled_addr;
extern volatile uint8_t  oled_rst;
extern volatile uint8_t  oled_orient;
extern volatile uint8_t   lora_cs;
extern volatile uint8_t   lora_rst;
extern volatile uint8_t   lora_dio;
extern volatile uint8_t   lora_busy;
extern volatile uint8_t   gps_rx;
extern volatile uint8_t   gps_tx;
extern volatile uint8_t   reboot_pin ;  // 0 to disable  esp32 hw reset output pin to force processor reset
extern volatile uint8_t   buzzer_pin ;  // 0 to disable
extern volatile uint8_t   fp_button ;   // 0 to disable  front panel button
extern volatile bool powerchip;
extern volatile uint8_t LoraDeviceType ;  // 1=lora_E22_400M30S  0= first generation
extern volatile bool  i2c_initialized ;
extern String LoRaDevice ;

void UpdateLocalConfigData(void ) ;
void WriteConfig(void) ;

// HW_Setup_Adjustment
//
//  HTML PAGE
//

#ifndef HW_TYPE_SARIMESH

const char PAGE_HW_SetupAdjustment[] PROGMEM = R"=====(
<meta name="viewport" content="width=device-width, initial-scale=1" />
<meta http-equiv="Content-Type" content="text/html; charset=utf-8" />
<a href="admin.html"  class="btn btn--s"><</a>&nbsp;&nbsp;<strong>HW Setup Configuration</strong>
<hr>
HW Setup Configuration:<br>
<form action="" method="get">
<table border="0"  cellspacing="0" cellpadding="3" style="width:310px" >
<tr><td colspan="2"><hr></span></td></tr>
<tr><td align="right">I2C Bus Pins:</td><td></td></tr>
<tr><td colspan="2"><hr></span></td></tr>
<tr><td align="right">i2c_sda:</td><td><input  type="text" id="i2c_sda" name="i2c_sda" value="" size="3"> </td></tr>
<tr><td align="right">i2c_scl:</td><td><input  type="text" id="i2c_scl" name="i2c_scl" value="" size="3"> </td></tr>

<tr><td colspan="2"><hr></span></td></tr>
<tr><td align="right">SPI Bus Pins:</td><td></td></tr>
<tr><td colspan="2"><hr></span></td></tr>
<tr><td align="right">spi_sck:</td><td><input  type="text" id="spi_sck" name="spi_sck" value="" size="3"> </td></tr>
<tr><td align="right">spi_miso:</td><td><input  type="text" id="spi_miso" name="spi_miso" value="" size="3"> </td></tr>
<tr><td align="right">spi_mosi:</td><td><input  type="text" id="spi_mosi" name="spi_mosi" value="" size="3"> </td></tr>

<tr><td colspan="2"><hr></span></td></tr>
<tr><td align="right">OLED Pins/Orient:</td><td></td></tr>
<tr><td colspan="2"><hr></span></td></tr>
<tr><td align="right">oled_addr:</td><td>
<select  id="oled_addr" name="oled_addr">
  <option value="60">0x3C</option>
  <option value="61">0x3D</option>
</select>
</td></tr>

<tr><td align="right">oled_rst:</td><td><input  type="text" id="oled_rst" name="oled_rst" value="" size="3"> </td></tr>
<tr><td align="right">oled_orient:</td><td>
<select  id="oled_orient" name="oled_orient">
  <option value="0">up</option>
  <option value="1">down</option>
</select>
</td></tr>

<tr><td colspan="2"><hr></span></td></tr>
<tr><td align="right">LoRa Pins:</td><td></td></tr>
<tr><td colspan="2"><hr></span></td></tr>
<tr><td align="right">lora_cs:</td><td><input  type="text" id="lora_cs" name="lora_cs" value="" size="3"> </td></tr>
<tr><td align="right">lora_rst:</td><td><input  type="text" id="lora_rst" name="lora_rst" value="" size="3"> </td></tr>
<tr><td align="right">lora_dio:</td><td><input  type="text" id="lora_dio" name="lora_dio" value="" size="3"> </td></tr>
<!-- <tr><td align="right">lora_busy:</td><td><input  type="text" id="lora_busy" name="lora_busy" value="" size="3"> </td></tr> -->

<tr><td colspan="2"><hr></span></td></tr>
<tr><td align="right">GPS Pins:</td><td></td></tr>
<tr><td colspan="2"><hr></span></td></tr>
<tr><td align="right">gps_rx:</td><td><input  type="text" id="gps_rx" name="gps_rx" value="" size="3"> </td></tr>
<tr><td align="right">gps_tx:</td><td><input  type="text" id="gps_tx" name="gps_tx" value="" size="3"> </td></tr>

<tr><td colspan="2"><hr></span></td></tr>
<tr><td colspan="2" align="center"><input type="submit" style="width:150px" class="btn btn--m btn--blue" value="Save"></td></tr>
<!-- <tr><td colspan="2" align="center"><a href="hwadj.html" class="btn btn--m btn--blue">Refresh</a></td></tr> -->
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
          setValues("/admin/hwadjvalues");
    });
  });
}
function load(e,t,n){if("js"==t){var a=document.createElement("script");a.src=e,a.type="text/javascript",a.async=!1,a.onload=function(){n()},document.getElementsByTagName("head")[0].appendChild(a)}else if("css"==t){var a=document.createElement("link");a.href=e,a.rel="stylesheet",a.type="text/css",a.async=!1,a.onload=function(){n()},document.getElementsByTagName("head")[0].appendChild(a)}}


</script>


)=====";


#else

const char PAGE_HW_SetupAdjustment[] PROGMEM = R"=====(
<meta name="viewport" content="width=device-width, initial-scale=1" />
<meta http-equiv="Content-Type" content="text/html; charset=utf-8" />
<a href="admin.html"  class="btn btn--s"><</a>&nbsp;&nbsp;<strong>HW Setup Configuration</strong>
<hr>
HW Setup Configuration:<br>
<form action="" method="get">
<table border="0"  cellspacing="0" cellpadding="3" style="width:310px" >
<tr><td colspan="2"><hr></span></td></tr>
<tr><td align="right">I2C Bus Pins:</td><td></td></tr>
<tr><td colspan="2"><hr></span></td></tr>
<tr><td align="right">i2c_sda:</td><td><input  type="text" id="i2c_sda" name="i2c_sda" value="" disabled  size="3"> </td></tr>
<tr><td align="right">i2c_scl:</td><td><input  type="text" id="i2c_scl" name="i2c_scl" value="" disabled  size="3"> </td></tr>

<tr><td colspan="2"><hr></span></td></tr>
<tr><td align="right">SPI Bus Pins:</td><td></td></tr>
<tr><td colspan="2"><hr></span></td></tr>
<tr><td align="right">spi_sck:</td><td><input  type="text" id="spi_sck" name="spi_sck" value="" disabled  size="3"> </td></tr>
<tr><td align="right">spi_miso:</td><td><input  type="text" id="spi_miso" name="spi_miso" value="" disabled  size="3"> </td></tr>
<tr><td align="right">spi_mosi:</td><td><input  type="text" id="spi_mosi" name="spi_mosi" value="" disabled  size="3"> </td></tr>

<tr><td colspan="2"><hr></span></td></tr>
<tr><td align="right">OLED Pins/Orient:</td><td></td></tr>
<tr><td colspan="2"><hr></span></td></tr>
<tr><td align="right">oled_addr:</td><td>
<select  id="oled_addr" name="oled_addr" disabled>
  <option value="60">0x3C</option>
  <option value="61">0x3D</option>
</select>
</td></tr>

<tr><td align="right">oled_rst:</td><td><input  type="text" id="oled_rst" name="oled_rst" value="" disabled  size="3"> </td></tr>
<tr><td align="right">oled_orient:</td><td>
<select  id="oled_orient" name="oled_orient" disabled>
  <option value="0">up</option>
  <option value="1">down</option>
</select>
</td></tr>

<tr><td colspan="2"><hr></span></td></tr>
<tr><td align="right">LoRa Pins:</td><td></td></tr>
<tr><td colspan="2"><hr></span></td></tr>
<tr><td align="right">lora_cs:</td><td><input  type="text" id="lora_cs" name="lora_cs" value="" disabled  size="3"> </td></tr>
<tr><td align="right">lora_rst:</td><td><input  type="text" id="lora_rst" name="lora_rst" value="" disabled  size="3"> </td></tr>
<tr><td align="right">lora_dio:</td><td><input  type="text" id="lora_dio" name="lora_dio" value="" disabled  size="3"> </td></tr>
<!-- <tr><td align="right">lora_busy:</td><td><input  type="text" id="lora_busy" name="lora_busy" value="" disabled  size="3"> </td></tr> -->

<tr><td colspan="2"><hr></span></td></tr>
<tr><td align="right">GPS Pins:</td><td></td></tr>
<tr><td colspan="2"><hr></span></td></tr>
<tr><td align="right">gps_rx:</td><td><input  type="text" id="gps_rx" name="gps_rx" value="" disabled  size="3"> </td></tr>
<tr><td align="right">gps_tx:</td><td><input  type="text" id="gps_tx" name="gps_tx" value="" disabled  size="3"> </td></tr>

<tr><td colspan="2"><hr></span></td></tr>
<!-- <tr><td colspan="2" align="center"><input type="submit" style="width:150px" class="btn btn--m btn--blue" value="Save"></td></tr> -->
<!-- <tr><td colspan="2" align="center"><a href="hwadj.html" class="btn btn--m btn--blue">Refresh</a></td></tr> -->
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
          setValues("/admin/hwadjvalues");
    });
  });
}
function load(e,t,n){if("js"==t){var a=document.createElement("script");a.src=e,a.type="text/javascript",a.async=!1,a.onload=function(){n()},document.getElementsByTagName("head")[0].appendChild(a)}else if("css"==t){var a=document.createElement("link");a.href=e,a.rel="stylesheet",a.type="text/css",a.async=!1,a.onload=function(){n()},document.getElementsByTagName("head")[0].appendChild(a)}}


</script>


)=====";

#endif

const char PAGE_HWadjWaitAndReload[] PROGMEM = R"=====(
<meta http-equiv="refresh" content="5; URL=hwadj.html">
Please Wait....Configuring and Restarting.
)=====";


//
//  SEND HTML PAGE OR IF A FORM SUMBITTED VALUES, PROCESS THESE VALUES
// 

void send_hwadj_configuration_html() ;
//
//   FILL THE PAGE WITH VALUES
//
void send_hwadj_configuration_values_html() ;

#endif

