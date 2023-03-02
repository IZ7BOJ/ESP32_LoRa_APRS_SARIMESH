// Lora_ShortBeacon_Support_Functions.h
#ifndef LSBS_H
#define LSBS_H 

#include "rdbg.h"
#include "loraprs_service.h"
#include <Syslog.h>
#include "ezTime_loc.h"
#include "FRAM.h"

float old_Lat , old_Lon ;
extern Syslog syslog ;
extern bool syslog_initialized ;
extern volatile bool syslog_available ;
extern String udpAddress ;
extern int udpPort ;
extern String udpAddress ;
extern int udpPort ;
extern float BWindx[];
extern uint8_t SFindx[];
extern uint8_t CRindx[] ;
extern uint8_t PrLenindx[] ;
extern float old_Lat , old_Lon ;
extern String ui3_payload ;
extern String ui4_payload ;
extern String ui5_payload ;
extern WiFiUDP udp;
extern uint32_t aprsShortBeaconPeriodSeconds ;
uint32_t previousShortBeaconMs ;
extern uint32_t aprsBeaconPeriodSeconds ;
extern bool requirePeriodicShortBeacon ;
extern uint32_t aprsBeaconPeriodSeconds ;
extern uint32_t previousBeaconMs ;
extern volatile bool requirePeriodicBeacon ;


bool send_native_log_msg(String textPayload , String signalReport, String my_spot) ;
void hard_reboot(void) ;
void write_spot_to_log(String my_record);
void soundBuzzer();
bool LDH_sendNativeToLora(byte *rxBuf, int rxBufIndex, uint8_t EncapType , bool noSched , bool SkipIfNeeded ) ;
bool LDH_sendNativeToLora_noCAD(byte *rxBuf, int rxBufIndex, uint8_t EncapType, bool noSched , bool SkipIfNeeded) ;
void handle_nativeLoRaPacket( byte *rxBuf, int rxBufIndex, float rssi, float lora_snr, float frequencyError) ;
void sendPeriodicShortBeacon_noCAD() ;
void sendPeriodicShortBeacon() ;
void sendNativeUdpMsg(char * Msg) ;
bool send_native_log_msg(String textPayload , String signalReport, String my_spot) ;

#endif
