// PAGE_Root.cpp
#ifndef PROOT1_H
#define PROOT1_H

#include "rdbg.h"
#include <Arduino.h>
#include <WebServer.h>  
#include "helpers.h"
#include "../configuration_data_definition.h"

extern strConfig ESP_Config;
extern WebServer web_server;
extern volatile uint16_t AdminTimeOutCounter ;
extern volatile bool WebConfig_debug ;  
extern String SW_Revision ;

void sendRootPage();
void WriteConfig(void) ;


const char PAGE_Root[] PROGMEM = R"=====(
<!doctype html>
<meta name="viewport" content="width=device-width, initial-scale=1" />
<script src="microajax.js"></script> 
<html>
	<head>
	<title></title>
	<meta charset="utf-8" />
	</head>
	<body>
	<link rel="stylesheet" href="style.css" type="text/css" />
	It work's!
	</body>
</html>

)=====";

void sendRootPage() ;


#endif
