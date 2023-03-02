// PPS_Support_Functions.cpp

#include "PPS_Support_Functions.h"
extern portMUX_TYPE ppsMux ;
extern volatile uint8_t state_pps ;
extern volatile uint8_t value_ready ;
extern volatile int16_t ovfl_count ;
extern volatile bool gpsFlag ;
extern uint8_t ppsPin ; 
extern volatile int16_t ovfl_count ;

 void IRAM_ATTR pps_intr_handler()
{
    portENTER_CRITICAL_ISR(&ppsMux);
    
    //  by MF
    GPS_rising_pps_millis = millis();
    GPS_sync_time = ezt::now();
//    GPS_sync_time = now() + 86400UL + 86400UL ;

switch (state_pps) {
  case 16 :
  case 15 :
  case 14 :
  case 13 :
  case 12 :
  case 11 :
  case 10 :
  case 9 :
  case 8 :
  case 7 :
  case 6 :
  case 5 :
  case 4 :
  case 3 :
  case 2 :
  // stiamo contando gli impulsi 
  value_ready = 0;
  state_pps = state_pps - 1;
  break;

case 1 :
  // finiamo di contare gli impulsi 
  pcnt_counter_pause(PCNT_TEST_UNIT);
  pcnt_counter_pause(PCNT_TEST_UNIT_2);
  ovfl_count = 0;
  value_ready = 1;
  state_pps = 0;
  break;

case 0 :
  // aspetta un secondo - omettiamo una chiamata programmata 
  value_ready = 0;
  state_pps = 10;
  pcnt_counter_clear(PCNT_TEST_UNIT);
  pcnt_counter_clear(PCNT_TEST_UNIT_2);

  pcnt_counter_resume(PCNT_TEST_UNIT);
  pcnt_counter_resume(PCNT_TEST_UNIT_2);

  break;

default:
  break;
}
    portEXIT_CRITICAL_ISR(&ppsMux);
}

 
// cleanup interrupt and increment pcnt overflow counter

/*
static void IRAM_ATTR pcnt_example_intr_handler(void *arg)
{
    portENTER_CRITICAL_ISR(&pcntMux);
  
    uint32_t intr_status = PCNT.int_st.val;
    int i;

    // cleanup interrupts&events
    for (i = 0; i < PCNT_UNIT_MAX; i++) {
         if (intr_status & (BIT(i))) {
            if(i == 0){ ovfl_count++; };
            PCNT.int_clr.val = BIT(i);
            };
         };
     
    portEXIT_CRITICAL_ISR(&pcntMux);
}

*/
//******************* Initialize timer related services **********************

void PPS_Timers_setup()
{
  Serial.print("Starting PPS_Timers_setup ... ");
  pinMode(ppsPin,INPUT_PULLUP);
  attachInterrupt(digitalPinToInterrupt(ppsPin), pps_intr_handler, RISING );    // for GPS
  Serial.println("done.");
}


//*********************** LoRa/VFO frequency measurement *****************************
void PPS_loop(){
//  Serial.printf("pps_loop_flag=%d - GPS_last_sync_time = %d - GPS_rising_pps_millis : %d \r\n ", pps_loop_flag ,GPS_last_sync_time, GPS_rising_pps_millis);  // time & millis of last GPS_PPS rising edge

  if (pps_loop_flag == true){    // allows to execute this segment only every 1 sec without blocking the main loop
      pps_loop_flag = false;
      if(pps_debug) Serial.printf("GPS_last_sync_time = %d - GPS_rising_pps_millis : %d \r\n ",GPS_last_sync_time, GPS_rising_pps_millis);  // time & millis of last GPS_PPS rising edge
     }; // end of if pps_loop_flag
}

