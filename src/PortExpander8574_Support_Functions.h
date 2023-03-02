//PortExpander8574_Support_Functions.h
#ifndef PCF8574_H
#define PCF8574_H

#include "rdbg.h"
#include <Arduino.h>
#include "configuration_data_definition.h"
#include "PCF8574.h"
#include "FRAM.h"
#include <WebServer.h>  


extern WebServer web_server;
extern strConfig ESP_Config;
extern FRAM fram;
extern volatile bool requirePeriodicBeacon ; 
extern volatile bool LoRa_initialized ;
extern volatile uint32_t LoRa_OnAirTime ;
extern volatile uint32_t LoRa_OnAirTime_Tot ;
extern bool LoRa_debug ;           // to debug LoRa 
extern volatile bool has_PCF8574 ; 
extern volatile bool has_FM24W256 ;
extern volatile bool PE_debug;
extern volatile bool I2C_Busy ;
extern volatile uint8_t red_led;
extern volatile uint8_t green_led;
extern uint32_t last_green_time, last_red_time ;
extern volatile uint8_t buzzer_pin ;  // 0 to disable 
extern volatile uint8_t fp_button ;   // 0 to disable  front panel button
extern uint32_t  button_press ;  // millisecs the pushbutt has been down
extern uint32_t last_button_millis ;  // to implement millisecond event for pushbutton
extern volatile uint8_t display_event  ;  // triggers real display update
extern volatile uint8_t reboot_pin ;  // 0 to disable  esp32 hw reset output pin to force processor reset
extern volatile uint8_t RebootTag ;           // RebootTag 
extern PCF8574 pcf8574;
extern SemaphoreHandle_t i2c_mutex_v;
extern bool  wait_default_config ; // wait for defaults loading by pushing button at startup
extern bool force_config_init ;

const char DebugWaitAndReload[] PROGMEM = R"=====(
<meta http-equiv="refresh" content="5; URL=debug.html">
Please Wait....Configuring and Restarting.
)=====";


void WriteConfig(void);
void hard_reboot(void);
void check_button();
void buzzer_disable();
void buzzer_enable();
void soundBuzzer();
void soundBuzzer_long();
void RED_LED_enable();
void RED_LED_disable();
void GREEN_LED_enable();
void GREEN_LED_disable();
void red_flash();
void red_flash_short();
void green_flash();
void green_flash_short();
void green_red_flash();
void WriteConfig();
bool LoadConfig( bool force_defaults ) ;
void write_event_to_log(String my_record);

#endif