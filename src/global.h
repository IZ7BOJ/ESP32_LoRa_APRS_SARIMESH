// global.h
#ifndef GLOBAL_H
#define GLOBAL_H

#include "rdbg.h"
#include <Arduino.h>

extern SemaphoreHandle_t i2c_mutex_v;
#include "FRAM.h"
extern FRAM fram;
#include "ezTime_loc.h"
#include "JsonConfigFile.h"
#include "PortExpander8574_Support_Functions.h"
#include "File_Download_Upload.h"

#include "GUI/PAGES_Includes.h"

extern byte packetBuffer[ NTP_PACKET_SIZE]; 

#endif
