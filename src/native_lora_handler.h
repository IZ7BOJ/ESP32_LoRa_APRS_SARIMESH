// native_lora_handler.h
#ifndef NLH_H
#define NLH_H

#include "rdbg.h"
#include <LoraEncoder.h>
#include "LoraMessage.h"
#include <freertos/semphr.h>
#include <freertos/queue.h>
#include "LoRa_Device_Handler.h"
#include "loraprs_config.h"
#include "loraprs_service.h"
#include "beacon_aux_functions.h"
#include "BCN_Structs.h"

extern LoraPrs::Service loraPrsService;
extern BeaconRunTimeConfig BCN_APRS_cfg  ;
extern QueueHandle_t xQueue_native_park;
extern uint32_t previousShortBeaconMs ;
extern uint16_t BCN_TimeSlot[] ;
extern byte BCN_TimeSlotSync ;
extern  byte BCN_TimeSlotOffset ;

bool LDH_sendNativeToLora(byte *rxBuf, int rxBufIndex, uint8_t Encap, bool noSched , bool SkipIfNeeded) ;
bool send_native_log_msg(String textPayload , String signalReport, String my_spot) ;
void native_loop() ;
void onLoraNativeDataAvailable( byte *rxBuf_BCN, const int packetSize, String signalReport , String my_spot) ;
void sendMagicPacket();
void sendNativePeriodicShortBeacon(void);
void hard_reboot(void) ;
void write_spot_to_log(String my_record);
void onLoraNativeDataAvailable( byte *rxBuf_BCN, const int packetSize, String sigreport , String my_spot);
void sendNativePeriodicBeacon();
void sendNativePeriodicShortBeacon();
void sendNativePeriodicShortBeacon_noCAD();
uint32_t GetNextSlice( uint32_t Interval, uint32_t Offset);

#endif
