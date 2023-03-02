// beacon_aux_functions.h
#ifndef BEACON_FUNCTIONS_H
#define BEACON_FUNCTIONS_H

#include "rdbg.h"
#include <Arduino.h>
#include <LoraEncoder.h>
#include "LoraMessage.h"
#include "LoRa_Device_Handler.h"
#include "BCN_Structs.h"
#include "ezTime_loc.h"

extern Timezone Italy;
extern Timezone UTC ;
extern time_t now;
// task sync and IPP support
extern SemaphoreHandle_t i2c_mutex_v; 
extern SemaphoreHandle_t SPI_mutex_v;
extern SemaphoreHandle_t LocationManager_mutex_v;
extern SemaphoreHandle_t ClockManager_mutex_v;
extern SemaphoreHandle_t LoRa_Device_Handler_mutex_v;
extern QueueHandle_t xQueue_in;
extern QueueHandle_t xQueue_APRS;
extern QueueHandle_t xQueue_native;
extern QueueHandle_t msg_queue;
extern QueueHandle_t xQueue_park;
extern QueueHandle_t xQueue_native_park;

// Beacon app parameters
extern volatile bool Beacon_Mode  ;     // to debug  Task_1 
extern byte BeaconId ;
extern byte BeaconFmt ;   // beacon format
extern bool BeaconSeqNbr;
extern byte BeaconLocation ;
extern byte BeaconEngineType ;
extern byte BeaconRun ;
extern byte BCN_TimeSlotOffset ;
extern byte BCN_TimeSlotSync ;
extern byte  BeaconStatsCollector_IP[4];
extern int BeaconStatsCollector_Port ;
extern String BeaconQRALocator;
extern String BCN_LoRa_Vector ;  
extern bool BeaconUnixTime ;  //
extern bool  BeaconFreq ;   //
extern bool BeaconPower ;   //
extern bool BeaconWorkConditions ; //
extern float lora_frequencyError ;
extern bool Native_Service_flag  ;
extern bool Native_Service_initialized ;
extern volatile int op_status;
extern bool LoRa_debug ;
extern volatile uint32_t IPC_lost_msgs ;
extern volatile uint32_t LoRa_ReSched_packets;
extern uint16_t BCN_SeqNbr ;
extern uint8_t BCN_MsgSN ;
extern uint8_t BCN_status ;  // 0=idle  1=TX  2=RX
extern volatile bool requirePeriodicBeacon; 
extern volatile bool APRS_Service_initialized ;
extern volatile uint8_t MsgStdSeqNbr ;  // Standard InterProcessComm sequence nmber global variable


bool SetLoraConfig(float loraFreq, float bw, byte sf, byte cr, int pwr, byte sync, byte prlen , String lora_mod_type);
void sendPeriodicShortBeacon_noCAD();
void onLoraDataAvailable_BCN(const byte *rxBuf_BCN, const int packetSize);
void sendNativePeriodicShortBeacon(void);
bool LDH_sendNativeToLora_noCAD(byte *rxBuf, int rxBufIndex, uint8_t Encap , bool NoShed , bool SkipIfNeeded) ;
bool SchedEnterRX_BeaconMode(byte *txBuf, int txBufIndex, uint8_t Encap , uint32_t schedTime , uint32_t repTime , struct BeaconRunTimeConfig &BCN_vector) ;
bool SchedExitRX_BeaconMode(byte *txBuf, int txBufIndex, uint8_t Encap , uint32_t schedTime , uint32_t repTime , struct BeaconRunTimeConfig &BCN_vector) ;
void onLoraDataAvailable_BCN( byte *rxBuf_BCN, const int packetSize);
void BCN_Controller( void * pvParameters );      //BCN_Controller: Beacon sender/receiver controller task
int  BeaconEncode(  struct BeaconRunTimeConfig &BCN_vector,  byte *outBuf_BCN );   // (float loraFreq, float bw, byte sf, byte cr, int pwr, byte sync, byte prlen){
bool LoRa_Set_BCN(  BeaconRunTimeConfig &BCN_vector);
void textToVector( String &LoraText ,  BeaconRunTimeConfig &BCN_vector );
bool BCN_VectorCmp(  struct BeaconRunTimeConfig &BCN_vector_a , struct BeaconRunTimeConfig &BCN_vector_b ) ;
String  BeaconDecode( byte *rxBuf_BCN, const int packetSize);  
uint32_t GetNextSlice( uint32_t Interval, uint32_t Offset);
bool SchedSendPeriodicBeacon(byte *txBuf, int txBufIndex, uint8_t Encap , uint32_t schedTime , uint32_t repTime , struct BeaconRunTimeConfig &BCN_vector) ;
time_t nextRuntime_short() ;
time_t nextRuntime_long() ;
void BeaconServiceLoop(void);  // this is the loop function in Beacon_mode
void BeaconSequencer_long(void);
void BeaconSequencer_short(void);
time_t nextRuntime_single(void) ;
void BeaconSequencer_single(void);
void setupBeaconMode(void);
void LoRaSer_demo(void) ;
void LoRaBeacon_msg(void) ;
void test_beacon(void);
bool loop_Lora_native(void);
void native_loop(void) ;

#endif
