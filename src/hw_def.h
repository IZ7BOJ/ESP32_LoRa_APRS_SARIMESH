// hw_def.h
#ifndef HW_DEF_H
#define HW_DEF_H

#include "rdbg.h"
#include <Arduino.h>


enum DeviceType
{
  eHELTEC_WIFI_LORA_32_V1,
  eHELTEC_WIFI_LORA_32_V2,
  eTTGO_LORA32_V1,
  eTTGO_LORA32_V2,
  eTTGO_T_Beam_V0_7,
  eTTGO_T_Beam_V1_0,
  eETH_BOARD,
  eTRACKERD
};

extern volatile uint8_t hw_arch;   // 0= undefined ( get values from GUI) 1=SARIMESH , 2= TTGO , 3 = Heltec
extern volatile uint8_t i2c_sda;
extern volatile uint8_t i2c_scl;
extern volatile uint8_t spi_sck;
extern volatile uint8_t spi_miso;
extern volatile uint8_t spi_mosi;
extern volatile uint8_t red_led;
extern volatile uint8_t green_led;
extern volatile uint8_t oled_addr;
extern volatile uint8_t oled_rst;
extern volatile uint8_t oled_orient;
extern volatile uint8_t LoraDeviceType ; 
extern volatile uint8_t lora_cs;
extern volatile uint8_t lora_rst;
extern volatile uint8_t lora_dio;
extern volatile uint8_t lora_busy;
extern volatile uint8_t gps_rx;
extern volatile uint8_t gps_tx;
extern volatile uint8_t reboot_pin ;  // 0 to disable  esp32 hw reset output pin to force processor reset
extern volatile uint8_t buzzer_pin ;  // 0 to disable 
extern volatile uint8_t fp_button ;   // 0 to disable  front panel button
extern volatile bool powerchip;
extern volatile bool has_GPS ;                // autodiscovery
extern volatile bool has_FM24W256;
extern volatile bool no_gps ;
extern volatile bool has_SSD1306 ;
extern volatile uint8_t display_event  ;  // triggers real display update
extern bool LoRa_debug ;
extern bool APRS_debug ;
extern volatile uint32_t last_wifi_up;
extern volatile bool mqtt_enable ;       // to debug miscellanea sel_test features
extern volatile bool Admin_Mode  ;      // to enter a special admin mode for SW loading
extern volatile bool AdminEnabled;
extern  bool standalone ;  // 1--> allows to start without WiFi/NTP immediately; will try to use GPS or RTC disciplined operation
extern  bool syslog_enable ;  
extern volatile bool ps_debug;
extern bool wifi_enable ;
extern int wifi_unavailable  ;
extern int last_wifi_tag ;
extern int mqtt_unavailable  ;
extern int counter6 ;  
extern bool has_ST77XX ;
extern volatile bool has_W5500 ;
extern volatile bool has_LittleFS ;           // autodiscovery
extern String Loc_Lat ; 
extern String Loc_Lon ; 
extern String ntpServer ;
extern volatile bool has_PCF8574  ;
extern volatile bool PE_debug;
extern uint32_t  button_press ;  // millisecs the pushbutt has been down
extern uint32_t last_button_millis ;  // to implement millisecond event for pushbutton
extern volatile bool I2C_Busy ;
extern uint32_t last_green_time, last_red_time ;
extern uint32_t last_aux_time;            // to implement one second events
extern volatile bool NTP_status ;
extern char RTCdatestring[20];
extern volatile int resync_counter;            // used for RTC resync with remote NTP server
extern volatile int resync_counter_trsh ;     // used for RTC resync with remote NTP server

bool HW_Set_SARIMESH(void);
void show_HW_Device_Config(void);
void show_HW_Device_Config_Serial(void);

#endif
