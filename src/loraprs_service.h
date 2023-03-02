// loraprs_service.h
// Master configuration directives: need to be customized depending on HW/SW configuratio of the device
#include "Master_Config.h"

#ifndef LORAPRS_SERVICE_H
#define LORAPRS_SERVICE_H

#include "loraprs_service.h"
#include <HardwareSerial.h>

#include <LoraEncoder.h>
#include "LoraMessage.h"
#include "ezTime_loc.h"
#include <WiFi.h>
#include <WiFiClient.h>
#include <RadioLib.h>
#include "LoRa_Device_Handler.h"
#include <freertos/semphr.h>
#include <freertos/queue.h>
#include "LoRa_Device_Handler.h"
#include "PortExpander8574_Support_Functions.h"

void NetworkUpstreamMonitor(); 
extern WiFiServer Kiss_server ;             // KISS_Over_TCP server
extern WiFiClient Kiss_client ;             // client definition for listening on  kiss_server incoming clients
extern volatile bool Beacon_Mode ;

extern String LastFunction ;
extern volatile uint8_t MsgStdSeqNbr ;  // Standard InterProcessComm sequence nmber global variable
extern bool syslog_initialized ;
extern volatile bool syslog_available ;
extern volatile uint8_t display_event ;  // triggers real display update

extern int volatile gps_fix_available  ;   // 1--> a GPS fix is available from gps
extern int volatile gps_fix_static  ;   // 1--> a GPS fix is statically assigned by configuration

extern volatile bool gps_debug  ;            // to debug gps operation
extern bool LoRa_debug ;

extern volatile bool requirePeriodicBeacon ; 
extern BeaconRunTimeConfig BCN_APRS_cfg  ;  // LoRa configuration vector used by APRS service
extern BeaconRunTimeConfig BCN_LDH_cfg  ;  // LDH present  configuration vector 
extern uint8_t LDH_RX_Mode ;          // 0= APRS mode   1 = Beacon mode
extern volatile bool aprsis_available ; 


extern bool standalone ;
extern volatile bool AdminEnabled;

extern int wifi_unavailable ;
extern volatile uint32_t IPC_lost_msgs ;

extern  bool LoRa_debug ;           // to debug LoRa operation
extern  bool APRS_debug;           // to debug APRS operation
extern volatile bool iGate_Mode ;     // to debug  Task_1 
extern volatile bool TCP_KISS_Mode ;            // to activate the TCP KISS modeover wifi
extern volatile bool BT_KISS_Mode ;             // to activate the Bluetooth KISS mode and disable WiFi connectivity
extern volatile bool Serial_KISS_Mode ;         // to activate the Serial KISS mode and disable WiFi connectivity
extern volatile bool Tracker_Mode;
extern volatile bool IoT_enable ;
extern volatile bool EnableRfToIs;
extern volatile bool EnableIsToRf;
extern volatile bool EnableRepeater;
extern volatile bool APRS_Service_initialized ;

extern String aprsBeacon_proto ;
// extern LoraPrs::Config cfg;             // this global structure will keep all the relevant data related to the LORA_APRS application extern LoraPrs::Service loraPrsService; // this is the manager ol the LORA_APRS application

void handle_nativeLoRaPacket( byte *rxBuf, int rxBufIndex, float rssi, float lora_snr, float frequencyError);
void sendPeriodicShortBeacon();
bool LDH_sendNativeToLora(byte *rxBuf, int rxBufIndex, uint8_t Encap, bool noSched , bool SkipIfNeeded) ;
bool LDH_Ran_sendNativeToLora(byte *rxBuf, int rxBufIndex, uint8_t Encap , bool noSched, bool SkipIfNeeded);
bool LDH_Ran_sendNativeToLora_noCAD(byte *rxBuf, int rxBufIndex, uint8_t Encap, bool noSched , bool SkipIfNeeded);

void PrintLoraPrsConfig();
void PrintLoraPrsConfig_private();
bool send_log_msg(String textPayload , String signalReport, String my_spot);
void setupBeaconMode();

// LoRa support global parameters
extern bool LoraPrs_Service_flag  ;
//WiFiClient aprs_client;  // used for iGate operation

#ifdef WIFI_UPSTREAM
  extern WiFiClient aprs_client;  // used for iGate operation
#endif

extern volatile bool LoRa_initialized ;   // this is an atomic variable
extern String Loc_Lat;
extern String Loc_Lon;

extern volatile uint32_t AprsIS_rx_packets ;
extern volatile uint32_t AprsIS_tx_packets ;
extern volatile uint32_t Kiss_rx_packets  ;
extern volatile uint32_t Kiss_tx_packets  ;
extern volatile uint32_t AprsIS_dropped_packets ;
extern volatile uint32_t AprsIS_relayed_packets ;
extern volatile uint32_t LoRa_ReSched_packets;
extern volatile float LoRa_DutyCycle;

extern volatile float cpu_temp ;
extern String NodeName ;

extern float lora_snr ;
extern float lora_rssi;
extern float lora_frequencyError ;
extern uint32_t aprsShortBeaconPeriodSeconds ;
extern bool requirePeriodicShortBeacon ;
extern HardwareSerial Esp32Serial1; // GPS
extern Stream * aSerial;
extern volatile byte EncapType ;     // 0=LoRa   1=APRS 
extern volatile byte PayloadStyle ;
extern volatile byte RepeaterOperation ;


extern char slat[9]; 
extern char slon[10];
extern SemaphoreHandle_t LocationManager_mutex_v;
extern QueueHandle_t xQueue_in;
extern QueueHandle_t xQueue_APRS;
extern QueueHandle_t xQueue_park;
extern QueueHandle_t xQueue_native_park;


#ifndef BLUETOOTH_DISABLED
  Stream * aSerial;    //  stream to be used to switch Bluetooth and HW_Serial
#endif



namespace LoraPrs {

class Service
{
public:
  Service();
  bool setup(const Config &conf);
  void aprs_loop();
  bool SetLoraConfig(float loraFreq, float bw, byte sf, byte cr, int pwr, byte sync, byte prlen , String lora_mod_type);
  bool setupLora(float loraFreq, float bw, byte sf, byte cr, int pwr, byte sync, byte prlen);
  void sendPeriodicBeacon();
  bool sendNativeToLora(byte *txBuf, int txBufIndex, uint8_t Encap );
  bool SchedNativeToLora(byte *txBuf, int txBufIndex, uint8_t Encap , uint32_t schedTime , bool SkipIfNeeded  ) ;
  // bool SchedNativeToLora_noCAD(byte *txBuf, int txBufIndex, uint8_t Encap , uint32_t schedTime , uint32_t repTime ) ;
  bool SchedNativeToLora_noCAD(byte *txBuf, int txBufIndex, uint8_t Encap , uint32_t schedTime , uint32_t repTime , struct BeaconRunTimeConfig &BCN_vector, bool SkipIfNeeded  );
  bool reconnectAprsis();
  inline bool needsAprsis() const { return !isClient_ && (enableRfToIs_ || enableIsToRf_); }


private:
  void setupBt(const String &btName);
  void reconnectWifi();
  void kissPDUcheckRX(uint8_t *rxBuffer , int packetSize) ;
  void kissPDUcheckTX(uint8_t *rxBuffer , int packetSize) ;
  void  kissForwardPacketFromLoRa(uint8_t *rxBuffer , int packetSize) ;

  void onLoraDataAvailable(uint8_t *rxBuffer , int packetSize, String signalReport, String my_spot);

  void onBtDataAvailable();
  void onAprsisDataAvailable();
  void sendToAprsis(String aprsMessage);
////  bool sendToLora(const AX25::Payload &payload, bool rndDelay);
//  bool sendToLora(const AX25::Payload &payload);
  bool sendToLoraRaw(uint8_t *buffer, int bytesToSend , bool SkipIfNeeded , uint8_t Encap);
  bool sendToLora(const AX25::Payload &payload, bool rndDelay, bool SkipIfNeeded ) ;

  void PrintLoraPrsConfig_private(const Config &conf);
  void kissResetState();

 // inline bool needsAprsis() const { return !isClient_ && (enableRfToIs_ || enableIsToRf_); }
  inline bool needsWifi() const { return needsAprsis(); }
  inline bool needsBt() const { return isClient_; }
  inline bool needsBeacon() const { return !isClient_ && enableBeacon_; }

private:
  enum KissMarker {
    Fend = 0xc0,
    Fesc = 0xdb,
    Tfend = 0xdc,
    Tfesc = 0xdd
  };

  enum KissState {
    Void = 0,
    GetCmd,
    GetData,
    Escape
  };

  enum KissCmd {
    Data = 0x00,
    NoCmd = 0x80
  };

  const String CfgLoraprsVersion = "LoRAPRS 0.1";

  // tune depending on TOA, higher value for higher time on air
  const int CfgPollDelayMs = 500;

private:
  // config
  bool isClient_;
  float loraFreq_;

  AX25::Callsign ownCallsign_;

  String aprsHost_;
  int aprsPort_;
  String aprsLogin_;
  String aprsBeacon_;
  int aprsBeaconPeriodSeconds_;

  bool autoCorrectFreq_;
  bool addSignalReport_;
  bool persistentConn_;
  bool enableRfToIs_;
  bool enableIsToRf_;
  bool enableRepeater_;
  bool enableBeacon_;

  // state
  KissCmd kissCmd_;
  KissState kissState_;
  long previousBeaconMs_;
  
  // peripherals
#ifndef BLUETOOTH_DISABLED  
  BluetoothSerial serialBt_;
#else
  Stream * aSerial;  
#endif

//  WiFiClient aprsisConn_;
};

} // LoraPrs

#endif

