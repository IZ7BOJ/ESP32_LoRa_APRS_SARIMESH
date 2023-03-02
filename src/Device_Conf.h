// Device_Conf.h
#ifndef HW_DEVICE_CONFIG_H
#define HW_DEVICE_CONFIG_H

#include "rdbg.h"
#include <Arduino.h>

//******************************* generic global data  ***************************
#define DEVICE_HOSTNAME "tracker"  // This device info
#define APP_NAME "LoRaBeacon"
#define HOST_NAME "remotedebug"
// define SW revision tag
String SW_Revision = SW_REVISION_ID ;
String VersionBanner = "Sarimesh Core Vr." + SW_Revision  ;
String DeviceId = "";
String NodeName = "";                     // This is the node official name
long randNumber;
volatile uint8_t RebootTag ;           // RebootTag 
volatile float cpu_temp ;
uint32_t activity_start = millis();
uint32_t activity_time = 0;
volatile int32_t time_jitter = 0;
String LastFunction ;
String webpage ;
volatile uint8_t  MsgStdSeqNbr ;  // global variable to keep IPC Message Sequence number 
unsigned long UnixTimestamp = 0;  // GLOBALTIME  ( Will be set by NTP)
bool Refresh = false;             // For Main Loop, to refresh things like GPIO / WS2812
int cNTP_Update = 0;              // Counter for Updating the time via NTP
byte Minute_Old = 100;            // Helpvariable for checking, when a new Minute comes up (for Auto Turn On / Off)

// HW related data
volatile uint8_t hw_arch;   // 0= undefined ( get values from GUI) 1=SARIMESH , 2= TTGO , 3 = Heltec
volatile uint8_t i2c_sda;
volatile uint8_t i2c_scl;
volatile uint8_t spi_sck;
volatile uint8_t spi_miso;
volatile uint8_t spi_mosi;
volatile uint8_t red_led;
volatile uint8_t green_led;
volatile uint8_t oled_addr;
volatile uint8_t oled_rst;
volatile uint8_t lora_cs;
volatile uint8_t lora_rst;
volatile uint8_t lora_dio;
volatile uint8_t lora_busy;
volatile uint8_t gps_rx;
volatile uint8_t gps_tx;
volatile uint8_t reboot_pin ;  // 0 to disable  esp32 hw reset output pin to force processor reset
volatile uint8_t buzzer_pin ;  // 0 to disable 
volatile uint8_t fp_button ;   // 0 to disable  front panel button
volatile bool powerchip;


// Debug flags
bool APRS_debug ;
bool LoRa_debug ;
volatile bool gps_debug = false ;         
volatile bool RTC_debug = false ;         
volatile bool pps_debug = false ; 
volatile bool ezTime_debug = 0 ;          // to debug ezTime operation
volatile bool PE_debug = 0;
volatile bool ps_debug= false ;
volatile bool mqtt_enable = false;       // to debug miscellanea sel_test features

// Operation modes and status flags
volatile bool BT_KISS_Mode = false ;         
volatile bool Serial_KISS_Mode = false ;         
volatile bool TCP_KISS_Mode = false ;         
volatile bool Tracker_Mode = false ;         
volatile bool IoT_enable = false ;         
volatile bool iGate_Mode = false ;  
volatile bool Beacon_Mode = false ;     // to debug  Task_1 
volatile bool Admin_Mode = true ;       // to enter a special admin mode for SW loading
volatile bool EnableRfToIs = false ;         
volatile bool EnableIsToRf = false ;         
volatile bool EnableRepeater = false ;
uint8_t LDH_RX_Mode = 0 ;               // 0= APRS mode   1 = Beacon mode
volatile int op_status;                 // system operation status
volatile bool AdminEnabled=false;
bool standalone = false;                // 1--> allows to start without WiFi/NTP immediately; will try to use GPS or RTC disciplined operation
bool syslog_enable = false;             // disables syslog upstream functionality
volatile bool has_Si5351 = 0;           // autodiscovery
volatile bool has_BME280 = 0;           // autodiscovery
volatile bool has_Si7021 = 0;           // autodiscovery
volatile bool has_PCF8574 = 0;          // autodiscovery 
volatile bool has_DS3231  ;             // autodiscovery
volatile bool has_DS3231_eeprom ;       // autodiscovery
volatile bool has_FM24W256;             // autodiscovery
volatile bool has_SSD1306 ;             // autodiscovery
bool has_ST77XX = true;
volatile bool has_GPS ;                 // autodiscovery
volatile int rtxFlag = 0;               // 0 = idle 1=rx 2=tx flag to indicate lora RTX mode
volatile bool interuptFlag = false ;                // an interrupt has been raised by lora HW
volatile bool transmittedFlag = false;              // flag to indicate that a packet was sent
volatile bool receivedFlag = false ;                // flag to indicate that a packet was received
bool Native_Service_flag = true ;
bool LoraPrs_Service_flag = true ;
volatile bool LoRa_initialized ;        // this is an atomic variable
bool Native_Service_initialized = false;
volatile bool APRS_Service_initialized ;
int volatile gps_initialized = 0 ;   // 1--> GPS discovery and initialization passed
volatile bool i2c_initialized = false;
int wifi_unavailable = 1 ;
int mqtt_unavailable = 1 ;
volatile bool NTP_status = false;
volatile bool LDH_RX_Lock = false ;          // signal that LoRa RTX is in RX_Locked state to allow native beacon reception only time slots
volatile byte LoraSync ;
volatile bool enableInterrupt = true;               // disable interrupt when it's not needed
int volatile gps_fix_available = 0 ;   // 1--> a GPS fix is available from gps 
int volatile gps_fix_static = 0 ;   // 1--> a GPS fix is statically assigned by configuration 
volatile bool aprsis_available ; 
void AprsMonitor(void) ;



bool wifi_enable = true ;
volatile bool activity_flag = false ;
volatile bool Reboot_Now = true ;           // to reboot immediately ESP32
bool firstStart = true;                     // On firststart = true, NTP will try to get a valid time
volatile bool I2C_Busy ;


// define syslog required data
#define SYSLOG_SERVER "75.119.145.3"
#define SYSLOG_PORT 514
bool syslog_initialized = false ;
bool volatile syslog_available = false ;


bool HW_Set_SARIMESH(void);
void show_HW_Device_Config(void);
void show_HW_Device_Config_Serial(void);


// Device generic data
volatile uint16_t AdminTimeOutCounter = 0 ;
volatile uint32_t IPC_lost_msgs ;
uint32_t  button_press ;                // millisecs the pushbutt has been down
uint32_t last_button_millis ;           // to implement millisecond event for pushbutton
uint32_t last_green_time, last_red_time ;
uint32_t last_millis;                   // to implement milli second events
uint32_t loop_time=0;


// Time subsystem related data
#include "ezTime_loc.h"                   // local fork for the ezTime library to support RTC and GPS in addition to NTP synch
#include "helpers.h"
strDateTime DateTime;
volatile int resync_counter=0;            // used for RTC resync with remote NTP server
volatile int resync_counter_trsh = 2;     // used for RTC resync with remote NTP server
volatile time_t NTP_sync_time ;           // patched ezTime to support NTP sync 
volatile time_t NTP_last_sync_time ;
volatile time_t GPS_sync_time ;           // patched ezTime to support GPS sync 
volatile time_t GPS_last_sync_time ;
timeStatus_t GPS_time_status ;
timeStatus_t NTP_time_status ;
Timezone Italy;
Timezone UTC;
Timezone *defaultTZ = &UTC;
String ntpServer = "ntp1.inrim.it";
uint32_t last_aux_time;                   // to implement one second events
char RTCdatestring[20];
long gmtOffset_sec = 3600;
int daylightOffset_sec = 3600;
volatile float ppm_runtime = 0.0 ;    // global data for average ppm deviation
volatile unsigned long  GPS_rising_pps_millis = 9876;

//LoRa related data
volatile uint32_t LoRa_OnAirTime = 0 ;
volatile float LoRa_DutyCycle = 0.0 ;     // our own transmission duty cycle
volatile float ChanCong = 0.0 ;           // channel congestion as average of reported peers dutycycles
volatile float currChanCong = 0.0 ;       // current channel congestion while scanning network
String nodeList = " ";                    // current node list
String currNodeList = " ";                // current node list
volatile uint32_t LoRa_OnAirTime_Tot = 0 ;
String RX_Last_Spot_level ;
String RX_Last_Spot_payload ;
String RX_Last_Path_Report ;

bool SetLoraConfig(float loraFreq, float bw, byte sf, byte cr, int pwr, byte sync, byte prlen , String lora_mod_type);
void onLoraDataAvailable_BCN(const byte *rxBuf_BCN, const int packetSize);

#include <RadioLib.h>
int transmissionState = RADIOLIB_ERR_NONE;                   // save transmission state between loops
float lora_rssi, lora_snr, lora_frequencyError, frequencyError; 
volatile float LoRa_ENL = 0.0 ;  // LoRa_ENL = estimated noise level
volatile uint16_t LoRaChipVersion ;
volatile float LoRaFreqNow = 0.0;
volatile uint32_t LoRa_lost_packets = 0;
volatile uint32_t LoRa_rx_packets = 0;
volatile uint32_t LoRa_tx_packets = 0;
volatile uint32_t LoRa_rx_AX25_packets = 0;
volatile uint32_t LoRa_tx_AX25_packets = 0;
volatile uint32_t LoRa_rx_OEStyle_packets = 0;
volatile uint32_t LoRa_tx_OEStyle_packets = 0;
volatile uint32_t LoRa_rx_native_packets = 0;
volatile uint32_t LoRa_tx_native_packets = 0;
volatile uint32_t LoRa_ReSched_packets = 0;
volatile uint32_t LoRa_CRC_errored_packets = 0;
volatile uint32_t LoRa_CRC_errored_packets_BCN = 0;
volatile uint32_t LoRa_UMN_errored_packets = 0;
volatile uint32_t LoRa_CAD_errors = 0;
volatile uint32_t LoRa_CAD_errorsunk = 0;

// Beacon app parameters
String aprsBeacon_proto ;
byte BeaconId ;
byte BeaconFmt ;                         // beacon format
bool BeaconSeqNbr;
byte BeaconLocation ;
byte BeaconEngineType ;
byte BeaconRun ;
byte BCN_TimeSlotOffset ;
byte BCN_TimeSlotSync ;
byte BeaconStatsCollector_IP[4];
int BeaconStatsCollector_Port ;
String BeaconQRALocator;
String BCN_LoRa_Vector ;  
bool BeaconUnixTime ;  
bool BeaconFreq ;   
bool BeaconPower ;   
bool BeaconWorkConditions ; 
uint16_t BCN_SeqNbr =0;
uint8_t BCN_MsgSN =0;
uint8_t BCN_status =0;  // 0=idle  1=TX  2=RX
#include "BCN_Structs.h"
BeaconRunTimeConfig BCN_APRS_cfg  ;  // LoRa configuration vector used by APRS service
BeaconRunTimeConfig BCN_LDH_cfg  ;  // present (latest used) LoRa configuration vector used any service
bool BCN_VectorCmp(  struct BeaconRunTimeConfig &BCN_vector_a , struct BeaconRunTimeConfig &BCN_vector_b ) ;


// APRS / LoRa related data
String lora_payload ;
volatile bool requirePeriodicBeacon = false; 
bool requirePeriodicShortBeacon ;
volatile byte EncapType ;     // 0=LoRa   1=APRS 
volatile byte PayloadStyle ;
volatile byte RepeaterOperation ;
#include "loraprs_config.h"
#include "loraprs_service.h"
LoraPrs::Config cfg;              // this global structure will keep all the relevant data related to the LORA_APRS application
LoraPrs::Service loraPrsService;  // this is the manager ol the LORA_APRS application
WiFiClient aprs_client;           // used for iGate operation
void sendPeriodicShortBeacon_noCAD();
void sendNativePeriodicShortBeacon(void);
volatile uint32_t AprsIS_rx_packets = 0;
volatile uint32_t AprsIS_tx_packets = 0;
volatile uint32_t Kiss_rx_packets = 0 ;
volatile uint32_t Kiss_tx_packets = 0 ;
volatile uint32_t AprsIS_dropped_packets = 0;
volatile uint32_t AprsIS_relayed_packets = 0;
uint32_t aprsShortBeaconPeriodSeconds = 30 ;

// GPS Related data
uint32_t GPSBaud = 9600;
int MAX_SATELLITES = 40;
int totalMessages = 0;
int currentMessage = 0;
// these are the latest GPS position lat/lon in string and char array format , updated by any call to getGPSdata()
// these data are protected via the LocationManager_mutex_v
String Loc_Lat = "NAJ"; // these are the static location lat/lon as setup by GUI interface in String format
String Loc_Lon = "NAJ"; 
String lat_now = "NA"; // present GPS location in String format
String lon_now = "NA";  
char slat[9];    // present GPS location in APRS formats char arrays
char slon[10];   
float homelat , homelon ;   // aux data 


// Display related data
volatile uint8_t oled_orient;
volatile uint8_t display_event = 1 ;  // triggers real display update
uint32_t last_display_update ;

// WiFi related data
volatile uint32_t last_wifi_up = 0;
int last_wifi_tag = 1 ;
bool wifi_disable ; 


// IoT related data
int counter6 = 0; 


// PS subsystem related data
#ifdef INA219_MOD
volatile bool has_INA219_5 = 1;               // autodiscovery A0 --> 0x41
volatile bool has_INA219_24 = 1 ;              // autodiscovery A1 --> 0x44
#else
volatile bool has_INA219_5 = 0;               // autodiscovery A0 --> 0x41
volatile bool has_INA219_24 = 0 ;              // autodiscovery A1 --> 0x44
#endif

bool LoRa_Device(void) ;
void setup_tft_adafruit() ;
void OTA_setup(void) ;
void LocalDisplayUpdate(void);   // run from main loop to update the local displays
void HMQ_setupMQTT() ;
void HMQ_loop_MQTT() ;


#endif

