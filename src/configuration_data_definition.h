// configuration_data_definition.h
#ifndef LOCAL_DATA_TYPES_DEF_H
#define LOCAL_DATA_TYPES_DEF_H

#include "Master_Config.h"
#include <Arduino.h>

#ifndef byte
#define byte uint8_t
#endif

class strConfig {
 public:
  String Structname ;
  String DeviceName ;
  String DeviceId ;
  String cpu_type ;
  String Loc_Lat ;
  String Loc_Lon ;

// wifi parameters  

  String ssid ;  //  7
  String password ;
  byte  IP[4] ;
  byte  Netmask[4] ;
  byte  Gateway[4] ;
  byte  Dns[4] ;
  bool  dhcp ;
  String ntpServerName ;

// time related parameters

  long Update_Time_Via_NTP_Every ;  // 15
  long timezone ;
  bool daylight ;

// debug and operation mode related parameters

  bool gps_debug ;
  bool LoRa_debug ;
  bool APRS_debug ;
  
  bool RTC_debug ;
  bool ezTime_debug ;
  bool pps_debug ;
  bool PE_debug ;
  bool mqtt_enable ;
  bool WebConfig_debug ;  
  bool BT_KISS_Mode ;
  bool Serial_KISS_Mode ; 
  bool Tracker_Mode ; 
  bool IoT_enable ; 
  bool standalone ;
  bool no_gps ; 
  bool iGate_Mode ;
  bool Beacon_Mode ;
  bool Admin_Mode ;
  bool Reboot_Now ;
  bool EnableRfToIs ;
  bool EnableIsToRf ;
  bool EnableRepeater ;

// APRS related parameters

  String AprsHost ;  // 40
  uint16_t AprsPort ;
  String AprsLogin ;
  String AprsPass ;
  String AprsFilter ;
  String AprsRawBeaconIGATE ;
  String AprsRawBeaconTRACK ;
  uint16_t AprsRawBeaconPeriodSecondsIGATE ;
  uint16_t AprsRawBeaconPeriodSecondsTRACK ;
  String AprsLoggerAddr ;
  uint16_t AprsLoggerPort ; 
  byte EncapType ;

// LoRa related parameters

  String LoraFreq ;
  String LoraBw ;
  byte LoraSf ;
  byte LoraCodingRate ;
  byte LoraSync ;
  int16_t LoraPower ;
  byte LoraPreambleLen ;
  int16_t LoraFreqCorr ;

// Beacon app parameters

  byte BeaconId ;  //60
  bool BeaconSeqNbr ;
  byte BeaconLocation ;
  bool BeaconUnixTime ; 
  bool BeaconFreq ;   
  bool BeaconPower ;   
  bool BeaconWorkConditions ; 
  byte BeaconEngineType ;
  byte BeaconRun ;
  byte BCN_TimeSlotOffset ;
  byte BCN_TimeSlotSync ;
  byte  BeaconStatsCollector_IP[4] ;
  uint16_t BeaconStatsCollector_Port ;
  String BeaconQRALocator ;
  String BCN_LoRa_Vector ; 

// IoT app parameters

  String IoT_Host ; // 75
  uint16_t IoT_Port ;
  String IoT_Login ;
  String IoT_Key ;
  String feed_1 ;
  String feed_2 ;
  String feed_3 ;
  String feed_4 ;
  String feed_5 ;
  String feed_6 ;

// void parameters
  String SW_Revision ; // 85 
  bool AutoTurnOff ;
  bool AutoTurnOn ;
  byte TurnOffHour ;
  byte TurnOffMinute ;
  byte TurnOnHour ;
  byte TurnOnMinute ;
  byte LED_R ;
  byte LED_G ;
  byte LED_B ; 

// HW Setup adj
  uint8_t i2c_sda; // 95
  uint8_t i2c_scl;
   
  uint8_t spi_sck;
  uint8_t spi_miso;
  uint8_t spi_mosi;
   
  uint8_t oled_addr;
  uint8_t oled_rst;
  uint8_t oled_orient;
   
  uint8_t lora_cs;
  uint8_t lora_rst;
  uint8_t lora_dio;
   
  uint8_t gps_rx;
  uint8_t gps_tx;
  bool syslog_enable ;
  uint8_t RebootTag ;
  byte PayloadStyle ;
  byte RepeaterOperation ;

  bool V5_ena; // 112
  bool V24_1_ena;
  bool V24_2_ena;
  bool V24_3_ena;
  bool Bat_discarge ;
  uint8_t PS_Out_Reset ;
  bool ps_debug ;
  bool wifi_enable ; 
  bool TCP_KISS_Mode ; // 120   
}  ;

#endif
