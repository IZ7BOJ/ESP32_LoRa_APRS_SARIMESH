// LoRa_Device_Handler.h
#ifndef LORA_DEVICE_HANDLER_H
#define LORA_DEVICE_HANDLER_H

#include "rdbg.h"
#include "BCN_Structs.h"
#include <LoraEncoder.h>
#include "LoraMessage.h"
#include <freertos/semphr.h>
#include <freertos/queue.h>
#include "ax25_payload.h"
#include <RadioLib.h>
#include "loraprs_config.h"
#include "loraprs_service.h"
#include "ezTime_loc.h"
#include <Syslog.h>
#include "PortExpander8574_Support_Functions.h"
#include "BCN_Structs.h"

extern LoraPrs::Config cfg;              // this global structure will keep all the relevant data related to the LORA_APRS application
extern int counter6 ;  
extern volatile uint16_t state ;
extern volatile uint8_t MsgStdSeqNbr ;
extern BeaconRunTimeConfig BCN_APRS_cfg  ;
extern String LastFunction ;
extern volatile uint8_t LoraDeviceType ;  // 2= second generation  1= first generation
extern String LoRaDevice ;
extern Timezone Italy;
extern Timezone UTC ;
extern time_t now;
extern BeaconRunTimeConfig BCN_LDH_cfg  ;  // present (latest used) LoRa configuration vector used any service
extern volatile bool LDH_RX_Lock ;          // signal that LoRa RTX is in RX_Locked state to allow native beacon reception only time slots
extern uint8_t LDH_RX_Mode  ;          // 0= APRS mode   1 = Beacon mode
extern char slat[9];
extern char slon[10];
extern SemaphoreHandle_t LoRa_Device_Handler_mutex_v;
extern QueueHandle_t xQueue_in;
extern QueueHandle_t xQueue_APRS;
extern QueueHandle_t xQueue_native;
extern QueueHandle_t msg_queue;
extern QueueHandle_t xQueue_park;
extern QueueHandle_t xQueue_native_park;
extern bool BCN_VectorCmp( struct BeaconRunTimeConfig &BCN_vector_a , struct BeaconRunTimeConfig &BCN_vector_b ) ;
extern Syslog syslog ;
extern bool syslog_initialized ;
extern volatile bool syslog_available ;
extern volatile uint32_t last_wifi_up ;
extern int last_wifi_tag ;
extern int wifi_unavailable ;
extern volatile bool AdminEnabled;
extern bool standalone ;
extern bool wifi_enable ;
extern volatile int op_status;
extern volatile uint8_t reboot_pin ;
extern volatile uint8_t RebootTag ;
extern volatile uint8_t display_event ;  
extern String SW_Revision ; 
extern String NodeName ;
extern bool LoRa_debug ;           // to debug LoRa operation
extern bool APRS_debug ;
extern volatile uint8_t EncapType ;
extern volatile float LoRa_DutyCycle ;
extern volatile float cpu_temp ;
extern volatile bool interuptFlag  ;
extern volatile bool enableInterrupt ;               // disable interrupt when it's not needed
extern volatile int rtxFlag ;   // 0 = idle 1=rx 2=tx   // flag to indicate lora RTX mode
extern volatile bool transmittedFlag ;              // flag to indicate that a packet was sent
extern volatile bool receivedFlag  ;                // flag to indicate that a packet was received
extern int transmissionState ;                   // save transmission state between loops
extern float lora_rssi, lora_snr, lora_frequencyError, frequencyError; 
extern volatile float LoRa_ENL  ;  // LoRa_ENL = estimated noise level
extern volatile uint16_t LoRaChipVersion ;
extern volatile float LoRaFreqNow ;
extern volatile uint32_t IPC_lost_msgs ;
extern volatile uint32_t LoRa_lost_packets ;
extern volatile uint32_t LoRa_rx_packets ;
extern volatile uint32_t LoRa_tx_packets ;
extern volatile uint32_t LoRa_rx_AX25_packets ;
extern volatile uint32_t LoRa_tx_AX25_packets ;
extern volatile uint32_t LoRa_rx_OEStyle_packets ;
extern volatile uint32_t LoRa_tx_OEStyle_packets ;
extern volatile uint32_t LoRa_rx_native_packets ;
extern volatile uint32_t LoRa_tx_native_packets ;
extern volatile uint32_t LoRa_ReSched_packets ;
extern volatile uint32_t LoRa_CRC_errored_packets ;
extern volatile uint32_t LoRa_CRC_errored_packets_BCN ;
extern volatile uint32_t LoRa_UMN_errored_packets ;
extern volatile uint32_t LoRa_CAD_errors ;
extern volatile uint32_t LoRa_CAD_errorsunk ;
extern volatile bool LoRa_initialized  ;
extern SX1268 lora_2g ;
extern SX1278 lora_1g ;


// Each send_request or receive_result will be a LoRa message with following format:
//   - Message_type_identifier
//   - LoRa_config_vector
//   - Buffer_length
//   - Buffer_to_send / buffer_received
// this message will be represented by a LoRa_MSG structure
// Following Message_type_identifiers are defined:
//


typedef struct Message {
  char SigRep[300];
  uint8_t ack;
} Message;



typedef struct LDH_Message {
  char SigRep[300];
  uint8_t ack;
  uint8_t MsgType ;
  uint8_t MsgSqNbr ;
  uint8_t bufLen ;    // bufData effective length
  uint32_t dueTime ;  // millis
  uint32_t ReschedTime ;  // millis request to reschedule message with such additional delta on original dueTime
  uint8_t bufData[255];
  float LoraFreq ;        // ex 433.725
  float LoraBw ;          // ex 32.25 Khz
  byte LoraSf ;            // ex. 12
  byte LoraCodingRate ;    // 8
  byte LoraSync ;          // 0x34
  signed int LoraPower ;   // 22 dbm
  int LoraFreqCorr ;       // 0
  byte LoraPreambleLen ;   // 6
} LDH_Message ;


typedef struct LDH_Message_v2 {
  uint8_t MsgType ;            // 1=lora init, 2= setup via LoRa vector (RX), 3=tranmit pkt with CAD autoselect, 4=tranmit pkt no CAD, 5=RX packet, 6= status query,  etc.
  uint8_t MsgSqNbr ;           // message sequence number ( optional)
  uint8_t ack;                 // 0=request/don't care  1= ack for this message ( see other fields...) 2= Nack for this message ( see other fields...)
  bool SkipIfNeeded ;          // skip silently this message if needed for overload control
  uint32_t dueTime ;           // millis for scheduled execution time
  uint32_t ReschedTime ;       // millis request to reschedule message with such additional delta on original dueTime
  BeaconRunTimeConfig Lv ;     // LoRa device setup vector
  uint8_t bufLen ;             // bufData effective length
  uint8_t bufData[260];        // packet native data
  char SigRep[300];            // Signal Report/aux info
} LDH_Message_v2 ;


void sendMagicPacket();
void write_spot_to_log(String my_record);
void LoRa_Device_Handler_setup( void );      
void LoRa_Device_Handler_loop( void );      
bool LDH_sendNativeToLora(byte *rxBuf, int rxBufIndex, uint8_t Encap , bool NoShed , bool SkipIfNeeded );
bool LDH_sendNativeToLora_noCAD(byte *rxBuf, int rxBufIndex, uint8_t Encap , bool NoShed , bool SkipIfNeeded );
bool LDH_Ran_sendNativeToLora(byte *rxBuf, int rxBufIndex, uint8_t Encap, bool NoShed , bool SkipIfNeeded );
bool LDH_Ran_sendNativeToLora_noCAD(byte *rxBuf, int rxBufIndex, uint8_t Encap , bool NoShed, bool SkipIfNeeded);
bool LDH_SetLoraConfig(float loraFreq, float bw, byte sf, byte cr, int pwr, byte sync, byte prlen , String lora_mod_type);
bool Setup_LDH_IPP(void);
uint32_t GetNextSlice( uint32_t Interval, uint32_t Offset);

#endif
