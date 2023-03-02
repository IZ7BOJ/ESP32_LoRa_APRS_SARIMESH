//DS3231_Support_Functions.h
#ifndef DS3231_SUPPORT_FUNCTIONS_H
#define DS3231_SUPPORT_FUNCTIONS_H 

#include "rdbg.h"
#include "Arduino.h"
#include <RtcDS3231.h>
#include "configuration_data_definition.h"
#include "ezTime_loc.h"      // local copy patched by MF for GPS/RTC integration

#include <Wire.h>
#include <RtcDS3231.h>
#include "RtcDateTime.h"
#include "RtcUtility.h"
#include <Syslog.h>

extern strConfig espConfig ;
extern strConfig ESP_Config;
extern RtcDS3231<TwoWire> Rtc;
extern String LastFunction ;
extern volatile time_t GPS_last_sync_time ;
extern timeStatus_t GPS_time_status ;
extern volatile bool has_GPS ;                // autodiscovery
extern volatile bool no_gps ;                // autodiscovery
extern  int wifi_unavailable ;
extern volatile time_t NTP_last_sync_time ;
extern xTimerHandle timerHndl1Sec;
extern SemaphoreHandle_t i2c_mutex_v;
extern volatile bool ezTime_debug ;
extern volatile bool RTC_debug  ;          // to debug RTC operation
extern volatile bool pps_debug  ;          // to debug PulsePerSecond functions
extern volatile bool has_DS3231  ;              // autodiscovery
extern volatile bool has_DS3231_eeprom ;        // autodiscovery
extern volatile int op_status ;  // operation status: 0=not_available 1=RTC_disciplined 2=NTP_disciplined  3=GPS_disciplined
extern uint32_t last_millis;     // to implement milli second events
extern  uint32_t loop_time;      // 
extern RtcDateTime rtc_time ;
extern SemaphoreHandle_t ClockManager_mutex_v;
extern volatile int gps_initialized ;   // 1--> GPS discovery and initialization passed
extern bool standalone ;
extern volatile bool I2C_Busy ;
extern uint32_t last_aux_time;            // to implement one second events
extern volatile bool NTP_status ;
extern char RTCdatestring[20];
extern volatile int resync_counter;            // used for RTC resync with remote NTP server
extern volatile int resync_counter_trsh ;     // used for RTC resync with remote NTP server
extern Syslog syslog;

extern xTimerHandle timerGPS_Status;
extern xTimerHandle timerPPS_LOOP;

// global GPS ezTime support variables
extern timeStatus_t GPS_time_status ;
extern timeStatus_t NTP_time_status ;
extern Timezone Italy;

void PPS_Timers_setup() ;
void PPS_loop() ;
void check_time_sync_source_availability(void); // proto definition
static void printRTC_DateTime(const RtcDateTime &dt);  // prototype definition
void displayRTC_DateTime(const RtcDateTime &dt);  // prototype definition
void ClockManager( void * pvParameters );      //ClockManager: task to keep updated local time clock available


#endif

