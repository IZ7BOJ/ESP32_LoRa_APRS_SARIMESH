// PPS_Support_Functions.h
#ifndef PPSSF_H
#define PPSSF_H

#include <stdio.h>
#include "freertos/FreeRTOS.h"
#include "freertos/portmacro.h"
#include "freertos/task.h"
#include "freertos/queue.h"
#include "driver/periph_ctrl.h"
#include "driver/ledc.h"
#include "driver/gpio.h"
#include "driver/pcnt.h"
#include "esp_attr.h"
#include "esp_log.h"
#include "soc/gpio_sig_map.h"



#define PCNT_TEST_UNIT PCNT_UNIT_0
#define PCNT_TEST_UNIT_1 PCNT_UNIT_1
#define PCNT_TEST_UNIT_2 PCNT_UNIT_2
#define PCNT_TEST_UNIT_3 PCNT_UNIT_3



#include "rdbg.h"
#include <Arduino.h>
#include "configuration_data_definition.h"
#include "ezTime_loc.h"
extern strConfig ESP_Config;
extern volatile bool pps_debug;
extern volatile time_t GPS_sync_time ;
extern volatile time_t GPS_last_sync_time ;
extern volatile unsigned long GPS_rising_pps_millis;
void WriteConfig();



// timer per misurare gli impulsi 
extern hw_timer_t * timer ;
extern portMUX_TYPE timerMux ;
extern uint8_t ppsPin ;   

extern volatile bool ppsFlag  ;
extern volatile bool gpsFlag  ;
extern volatile bool pps_loop_flag  ;
extern volatile uint8_t state_pps ;
extern volatile uint8_t value_ready ;



extern volatile uint16_t state ;
extern volatile uint8_t state2 ;
extern volatile uint8_t state_tmr ;
extern volatile uint8_t state_pps ;
extern volatile uint8_t value_ready ;


// MUX per ISR contatore PCNU
extern portMUX_TYPE pcntMux ;

// MUX per ISR segnale PPS da GPS
extern portMUX_TYPE ppsMux ;

extern volatile uint16_t state ;
extern volatile uint8_t state2 ;
extern volatile uint8_t state_tmr ;
extern volatile uint8_t state_pps ;
extern volatile uint8_t value_ready ;




// MUX per ISR contatore PCNU
extern portMUX_TYPE pcntMux ;

// MUX per ISR segnale PPS da GPS
extern portMUX_TYPE ppsMux ;

// interrupt service routine 
static void vTimerCallback1SecExpired(xTimerHandle pxTimer) ;
static void vTimerCallbackGPSExpired(xTimerHandle pxTimer) ;
static void vTimerCallbackLoopPPSExpired(xTimerHandle pxTimer) ;
static void IRAM_ATTR pps_intr_handler() ;
// cleanup interrupt and increment pcnt overflow counter
static void IRAM_ATTR pcnt_example_intr_handler(void *arg) ;

//******************* Initialize timer related services **********************
void PPS_Timers_setup() ;

//*********************** LoRa/VFO frequency measurement *****************************
void PPS_loop() ;


#endif
