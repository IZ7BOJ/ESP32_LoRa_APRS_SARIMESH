// ESP32_RTOS_Support_Functions.h
#ifndef RTOSSF1_H
#define RTOSSF1_H

// this file includes most of freeRTOS related stuffs including snaplets for setup and poop sections.

extern "C" {
    #include "soc/pcnt_struct.h"
    }
#include "driver/pcnt.h"


// ************************************
// ** RTC Setup
// ************************************


unsigned long millisSync ;
unsigned long epochTime ;
unsigned long last_sync_time ;
unsigned long last_read_ms;
unsigned long last_sync_millis;

// this is the GPS-PPS signal from GPS
uint8_t ppsPin = 39;    // this is the pps signal coming from GPS breakout ( GPS_PPS J10 test point )

int16_t x;
volatile uint32_t us_time, us_time_diff;

// MUX per ISR contatore PCNU
portMUX_TYPE pcntMux = portMUX_INITIALIZER_UNLOCKED;

// MUX per ISR segnale PPS da GPS
portMUX_TYPE ppsMux = portMUX_INITIALIZER_UNLOCKED;

volatile uint16_t state = 0;
volatile uint8_t state2 = 0;
volatile uint8_t state_tmr = 10;
volatile uint8_t state_pps = 10;
volatile uint8_t value_ready = 0;
volatile int16_t ovfl_count ;


#define PCNT_TEST_UNIT PCNT_UNIT_0
#define PCNT_TEST_UNIT_1 PCNT_UNIT_1
#define PCNT_TEST_UNIT_2 PCNT_UNIT_2
#define PCNT_TEST_UNIT_3 PCNT_UNIT_3

#define PCNT_H_LIM_VAL 32767
#define PCNT_L_LIM_VAL -1


// SW timer for PPS signal timing
 xTimerHandle timerHndl1Sec;
 volatile bool ppsFlag = false ;

// SW timer for GPS Status display
 xTimerHandle timerGPS_Status;
 volatile bool gpsFlag = false ;

// SW timer for GPS Status display
 xTimerHandle timerPPS_LOOP;
 volatile bool pps_loop_flag = false ;


// PCNT ISR stuffs
pcnt_isr_handle_t user_isr_handle = NULL; //user's ISR service handle


#endif
