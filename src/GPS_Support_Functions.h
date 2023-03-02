//GPS_Support_Functions.h
#ifndef GPS_SUP_H
#define GPS_SUP_H

#include "rdbg.h"
#include <Syslog.h>
#include <TinyGPS++.h>
#include <math.h>

struct
{
  bool active;
  int elevation;
  int azimuth;
  int snr;
} sats[40];

extern String LastFunction ;
extern SemaphoreHandle_t LocationManager_mutex_v;
extern TaskHandle_t LocationManager_handle;         // handles device location
extern volatile bool LocationManager_flag ;
extern Syslog syslog ;
extern bool syslog_initialized ;
extern volatile bool syslog_available ;
extern int wifi_unavailable ;
extern HardwareSerial Esp32Serial1; // GPS
extern volatile uint8_t gps_rx;
extern volatile uint8_t gps_tx;
extern volatile bool gps_debug  ;            // to debug gps operation
extern volatile bool has_GPS ;                // autodiscovery
extern volatile bool no_gps ;
extern volatile bool gpsFlag ;
extern int volatile gps_fix_available  ;   // 1--> a GPS fix is available from gps
extern int volatile gps_fix_static  ;   // 1--> a GPS fix is statically assigned by configuration
extern int volatile gps_initialized  ;   // 1--> GPS discovery and initialization passed
extern int totalMessages ;
extern int currentMessage ;
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
// these are the latest GPS position lat/lon in string and char array format , updated by any call to getGPSdata()
// these data are protected via the LocationManager_mutex_v
extern String Loc_Lat ; // these are the static location lat/lon as setup by GUI interface in String format
extern String Loc_Lon ; 
extern String lat_now ; // present GPS location in String format
extern String lon_now ;  
extern char slat[];    // present GPS location in APRS formatas char arrays
extern char slon[];   
extern float homelat , homelon ;   // aux data 
extern uint32_t GPSBaud ;
extern int MAX_SATELLITES ;
static void smartDelay(unsigned long ms);
static void printFloat(float val, bool valid, int len, int prec);
static void printInt(unsigned long val, bool valid, int len);
static void printStr(const char *str, int len);
static void printDateTime_gps(TinyGPSDate &d, TinyGPSTime &t);

void LocationManager( void * pvParameters );    //LocationManager: task to keep updated location data available
void gps_dump();
void gps_status();
void gps_status_header();
void gps_now();
void gps_loop_extended();
void show_gps_loop_extended();
void displayGPS_TimeDate();
void displayGPS_Info();
void getGPSdata();
void gps_setup();
void gps_loop();
void gps_detect();

#endif
