// Lora_Radiolib_Support_Functions.h
#ifndef LRSF_H
#define LRSF_H

#include "rdbg.h"
#include "loraprs_service.h"
#include "PortExpander8574_Support_Functions.h"
#include <TinyGPS++.h>
#include <Syslog.h>
#include "ezTime_loc.h"

extern String lat_now,lon_now;
extern String RX_Last_Spot_level ;
extern String RX_Last_Spot_payload ;
extern String RX_Last_Path_Report ;
extern String LastFunction ;
extern uint32_t activity_time ;
extern String lora_payload ;
extern int udpPort ;
extern WiFiUDP udp;
extern String udpAddress ;
extern volatile float ChanCong  ;           // channel congestion as average of reported peers dutycycles
extern volatile float currChanCong ;       // current channel congestion while scanning network
extern uint32_t activity_start ;
extern String nodeList ;           // current node list
extern String currNodeList ;           // current node list
extern LoraPrs::Config cfg;
extern LoraPrs::Service loraPrsService;
extern strConfig ESP_Config;
extern String ui3_payload ;
extern String ui4_payload ;
extern String ui5_payload ;
extern float old_Lat , old_Lon ;

void write_spot_to_log(String my_record);
void initializeLoraPrsConfig() ;
bool setup_LoraPrs_Service(void) ;
bool loop_Lora_aprs();
void PrintLoraPrsConfig();
void printWifiStatus() ;
void sendUdpMsg(char * Msg) ;
bool send_log_msg(String textPayload , String signalReport, String my_spot) ;
void DutyCycle_Monitor(void) ;
void show_dashboard(void) ;
void show_stats(void) ;
void write_event_to_log(String my_record);
String textToBeaconProto( String BeaconText ) ;

#endif
