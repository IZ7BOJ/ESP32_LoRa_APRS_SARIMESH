// webhome.h
#ifndef PAGE_HOME_H
#define PAGE_HOME_H

#include "rdbg.h"
#include "helpers.h"
#include "ezTime_loc.h"                   // local fork for the ezTime library to support RTC and GPS in addition to NTP synch
#include <WebServer.h>  
#include "../configuration_data_definition.h"
#include "helpers.h"

extern strConfig ESP_Config;
extern WebServer web_server;
extern strDateTime DateTime;
extern Timezone Italy;
extern Timezone UTC;
extern volatile bool WebConfig_debug ;  
extern strDateTime DateTime;
extern Timezone Italy;
extern Timezone UTC;
extern  String DeviceId ;
extern bool firstStart ;
extern String SW_Revision ;

void WriteConfig(void);

//
//   The HOME PAGE
//
const char PAGE_HOME[] PROGMEM = R"=====(
  <meta name="viewport" content="width=device-width, initial-scale=1" />
  <meta http-equiv="Content-Type" content="text/html; charset=utf-8" />
  <h1>Welcome to LoRa Tracker</h1>     

<hr>
<form action="/admin.html" method="post">
<table border="0"  cellspacing="0" cellpadding="3" >
<tr>
  <td align="right">Name of Device</td>
  <td><input type="text" id="devicename" name="devicename" value="" disabled ></td>
</tr>
<tr>
  <td align="right">Device Id</td>
  <td><input type="text" id="DeviceId" name="DeviceId"  value="" disabled ></td>
</tr>

<tr>
  <td align="right">SW_Revision</td>
  <td><input type="text" id="SW_Revision" name="SW_Revision"  value="" disabled ></td>
</tr>

<tr><td colspan="2" align="center"><input type="submit" style="width:150px" class="btn btn--m btn--blue" value="Continue"></td></tr>
</table>
</form>


    
 <!--  <div id="mydynamicdata">Here comes the Dynamic Data in </div> -->  <!-- added a DIV, where the dynamic data goes to -->
 
   <script>                


		window.onload = function ()
		{
			load("style.css","css", function() 
			{
				load("microajax.js","js", function() 
				{
						setValues("/admin/examplevalues");  //-- this function calls the function on the ESP      
				});
			});
		}
   
		function load(e,t,n){
		  if("js"==t){
		     var a=document.createElement("script");a.src=e,a.type="text/javascript",a.async=!1,a.onload=function(){n()},document.getElementsByTagName("head")[0].appendChild(a)
		     }
		  else if("css"==t){
		     var a=document.createElement("link");a.href=e,a.rel="stylesheet",a.type="text/css",a.async=!1,a.onload=function(){n()},document.getElementsByTagName("head")[0].appendChild(a)
		     }
		  }



   </script>

)=====";


void filldynamicdata() ;
void processHome();
void send_home_html() ;
void send_example_configuration_values_html() ;

#endif
