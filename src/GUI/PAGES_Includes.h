//PAGES_Includes.h
#ifndef PGSI_H
#define PGSI_H

// Include the HTML, STYLE and Script "Pages"
#include "GUI/PAGE_Style.css.h"
#include "GUI/PAGE_Information.h"
#include "GUI/PAGE_Admin.h"
#include "GUI/PAGE_Statistics.h"
#include "GUI/PAGE_Dashboard.h"
#include "GUI/webhome.h"
#include "GUI/PAGE_NetworkConfiguration.h"
#include "GUI/PAGE_DebugConfiguration.h"
#include "GUI/PAGE_InventoryConfiguration.h"
#include "GUI/PAGE_IoT_Configuration.h"
#include "GUI/PAGE_hwadj.h"
#include "GUI/PAGE_NTPSettings.h"
#include "GUI/PAGE_General.h"
#include "GUI/PAGE_PayloadEncapConfiguration.h"
#include "GUI/PAGE_LoRaConfiguration.h"
#include "GUI/PAGE_AprsConfiguration.h"
#include "GUI/PAGE_BeaconConfiguration.h"
#include "GUI/PAGE_Root.h"
#include "GUI/PAGE_Script.js.h"
#include "helpers.h"

extern volatile bool has_PCF8574 ;              
extern volatile bool has_DS3231 ;
extern volatile bool has_DS3231_eeprom ;
extern volatile bool has_Si5351 ;
extern volatile bool has_SSD1306 ;
extern volatile bool has_SSD1306 ;
extern volatile bool has_BME280 ;
extern volatile bool has_Si7021 ;
extern volatile bool has_W5500 ;
extern volatile bool has_GPS ;
extern volatile bool has_LittleFS ;
extern bool has_ST77XX ;
extern char has_SD_Card[10];
extern char SD_Card_Size[10];
extern volatile bool gps_debug  ; 
extern volatile bool RTC_debug  ;         
extern volatile bool pps_debug  ;          
extern volatile bool BT_KISS_Mode  ;          
extern volatile bool Serial_KISS_Mode  ;          
extern volatile bool TCP_KISS_Mode  ;          
extern volatile bool Tracker_Mode  ;          
extern volatile bool IoT_enable  ;          
extern volatile bool iGate_Mode  ;          
extern volatile bool Beacon_Mode  ;          
extern volatile bool EnableRfToIs  ;          
extern volatile bool EnableIsToRf  ;          
extern volatile bool EnableRepeater  ;  
extern bool syslog_enable ;
extern volatile bool LoRa_initialized  ;
extern volatile bool save_config_enable ;
extern bool standalone ;
extern volatile bool no_gps ;
extern volatile bool ezTime_debug  ;       // to debug RTC operation
extern bool LoRa_debug ;           // to debug LoRa 
extern bool APRS_debug ;           // to debug APRS 
extern volatile uint8_t RebootTag ;           // RebootTag 
extern volatile bool ps_debug ;   //
extern bool wifi_enable ; 
extern volatile bool Admin_Mode  ;         //   
extern volatile uint8_t display_event ;
extern volatile bool WebConfig_debug ;      // to debug webConfig task
extern volatile bool mqtt_enable;      // web_console debug
extern volatile bool PE_debug ;
extern volatile bool V5_ena;            // to debug gps operation
extern volatile bool V24_1_ena ;
extern volatile bool V24_2_ena  ;          // to debug PulsePerSecond functions
extern volatile bool V24_3_ena ;
extern volatile uint8_t PS_Out_Reset;     
extern volatile bool Bat_discarge ;
extern bool firstStart ;           // On firststart = true, NTP will try to get a valid time
extern strDateTime DateTime;                // Global DateTime structure, will be refreshed every Second
extern unsigned long UnixTimestamp ;     // GLOBALTIME  ( Will be set by NTP)
extern bool Refresh ;             // For Main Loop, to refresh things like GPIO / WS2812
extern int cNTP_Update ;                 // Counter for Updating the time via NTP
extern byte Minute_Old ;               // Helpvariable for checking, when a new Minute comes up (for Auto Turn On / Off)
extern volatile uint16_t AdminTimeOutCounter ;
extern String NodeName ;    
extern  byte BeaconId ;
extern  bool BeaconSeqNbr;
extern  byte BeaconLocation ;
extern  byte BeaconEngineType ;
extern  byte BeaconRun ;
extern  byte BCN_TimeSlotOffset ;
extern  byte BCN_TimeSlotSync ;
extern  byte BeaconStatsCollector_IP[4];
extern  int BeaconStatsCollector_Port ;
extern  String BeaconQRALocator;
extern  String BCN_LoRa_Vector ;  
extern  bool BeaconUnixTime ;  //
extern  bool  BeaconFreq ;   //
extern  bool BeaconWorkConditions ; //
extern SemaphoreHandle_t i2c_mutex_v;
extern  WiFiUDP UDPNTPClient;
extern volatile bool Reboot_Now ;      // to reboot immediately ESP32

void WriteRecord(String *text);
bool DisplayConfig(void ) ;
void send_home_html();
void send_inventory_configuration_html();
void send_debug_configuration_html(void) ;
void send_debug_configuration_values_html(void) ;
void send_aprs_configuration_html() ;
void send_aprs_configuration_values_html() ;
void send_beacon_configuration_html() ;
void send_beacon_configuration_values_html() ;
void send_debug_configuration_html() ;
void send_debug_configuration_values_html() ;
void send_iot_configuration_html() ;
void send_iot_configuration_values_html() ;
void send_lora_configuration_html() ;
void send_lora_configuration_values_html() ;
void send_hwadj_configuration_html() ;
void send_hwadj_configuration_values_html() ;
void send_network_configuration_values_html() ;
void send_connection_state_values_html() ;
void send_NTP_configuration_html() ;
void send_NTP_configuration_values_html() ;
void send_encap_configuration_html() ;
void send_encap_configuration_values_html() ;
void send_devicename_value_html() ;
void send_example_configuration_values_html() ;
void send_general_configuration_values_html() ;
void send_inventory_configuration_values_html() ;
void send_dashboard_values_html() ;
void send_information_values_html() ;
void send_network_configuration_html() ;
void send_encap_configuration_html_kiss();
void send_general_html();
void processHome();      
void filldynamicdata();
void WriteConfig(void) ;
bool ReadConfig(void ) ;
void UpdateLocalConfigData(void ) ;
bool DisplayConfig_serial(void ) ;
void NTPRefresh() ;
void loop_global() ;
bool LoadConfig( bool force_defaults ) ;
bool WebServer_Init( void );
void write_spot_to_log(String my_record) ;
void write_event_to_log(String my_record);
void Second_Tick(void) ;
void send_stats_values_html (void) ;
void send_beacon_configuration_html(void) ;


#endif
