//DS3231_Support_Functions.cpp
// Master configuration directives: need to be customized depending on HW/SW configuratio of the device
#include "Master_Config.h"
#include "DS3231_Support_Functions.h"
#include "PPS_Support_Functions.h"



static void vTimerCallbackLoopPPSExpired(xTimerHandle pxTimer) {
//    Serial.println(">>--- vTimerCallbackLoopPPSExpired ------------------ pps_loop timer (every 1 sec) ---------------------------: ");
      pps_loop_flag = true;
      ppsFlag = true;
}


static void vTimerCallbackGPSExpired(xTimerHandle pxTimer) {
//    Serial.println(">>--- vTimerCallbackGPSExpired ------------------ GPS status timer (every 1 sec) ---------------------------: ");
      gpsFlag = true;
}


// interrupt service routine 
 void vTimerCallback1SecExpired(xTimerHandle pxTimer) {
//    Serial.println(">>--- vTimerCallback1SecExpired ------------- DS3231 PPS interrupt retriggering --------------------------------: ");
      ppsFlag = true;
}

 //void vTimerCallback1SecExpired(xTimerHandle pxTimer) ;
 //void vTimerCallbackGPSExpired(xTimerHandle pxTimer) ;
 // void vTimerCallbackLoopPPSExpired(xTimerHandle pxTimer) ;


bool CheckRTC_Valid(void){      // checks for RTC status validity and just in case performs a RTC_Update procedure
     LastFunction = __func__ ;
     bool boolCheck = true; 
     if(RTC_debug) debugA("CheckRTC_Valid:  ");
     if (xSemaphoreTake(i2c_mutex_v, ( TickType_t ) 2) ){
         if(RTC_debug) debugA("Checking RTC sanity...\r");
         if (!Rtc.IsDateTimeValid() && has_DS3231 ){
             if(RTC_debug) debugA("CheckRTC_Valid: RTC lost confidence in the DateTime!  Forcing RTC_Update\r");
             boolCheck = false;
             resync_counter = 0;    
             };
 
        if (!Rtc.GetIsRunning()  && has_DS3231 ){
            if(RTC_debug) debugA("CheckRTC_Valid: RTC was not actively running, starting now. Forcing RTC_Update\r");
            Rtc.SetIsRunning(true);
            boolCheck = false;
            resync_counter = 0;
            };
        if(RTC_debug) debugA("CheckRTC_Valid: done\r");
        xSemaphoreGive(i2c_mutex_v); 
        }
    else{
        if(RTC_debug) debugA(" ===> skipping check I2C_Busy !!!!!!\r");       
        };
   return true;     
}


// Utility print function
#define countof(a) (sizeof(a) / sizeof(a[0]))


void printRTC_DateTime(const RtcDateTime &dt){
    LastFunction = __func__ ;
    char datestring[20];
    snprintf_P(datestring, 
            countof(datestring),
            PSTR("%02u/%02u/%04u %02u:%02u:%02u"),
            dt.Month(),
            dt.Day(),
            dt.Year(),
            dt.Hour(),
            dt.Minute(),
            dt.Second() );
    if(RTC_debug) debugA("%s",datestring);
}


void displayRTC_DateTime(const RtcDateTime &dt){
    snprintf_P(RTCdatestring, 
            countof(RTCdatestring),
            PSTR("%02u/%02u/%04u %02u:%02u:%02u"),
            dt.Month(),
            dt.Day(),
            dt.Year(),
            dt.Hour(),
            dt.Minute(),
            dt.Second() );
     return;
}
// *************************  init DS3231 RTC  ************************************
void DS3231_Setup() {
LastFunction = __func__ ;
if(! has_DS3231){
     if(RTC_debug) debugA("device not available; skipping RTC setup.\r");
     }
else{   
  if(RTC_debug) debugA("Starting DS3231_Setup...\r");
  if(RTC_debug) debugA("Setting DS3231 chip configuration...\r");
  Rtc.Begin();                              // https://github.com/Makuna/Rtc/wiki/RtcDS3231-object
  Rtc.Enable32kHzPin(false);
  //  Rtc.SetSquareWavePin(DS3231SquareWavePin_ModeAlarmOne );   // SQW will trigger when Alarm_1 is generated (i.e. on top of every second)
  //  Rtc.SetSquareWavePin(DS3231SquareWavePin_ModeClock ); 
  //  Rtc.SetSquareWavePinClockFrequency(DS3231SquareWaveClock_1Hz);

  //  Alarm 1 set to trigger at the top of the seconds
  //  DS3231AlarmOne alarm1( 0,0,0,0, DS3231AlarmOneControl_OncePerSecond);   // https://github.com/Makuna/Rtc/wiki/RtcDS3231-AlarmOne
  //  Rtc.SetAlarmOne(alarm1);
  
  //  throw away any old alarm state before we ran
  //  Rtc.LatchAlarmsTriggeredFlags();
 
  if (!Rtc.GetIsRunning()){
      if(RTC_debug) debugA("   RTC was not actively running, starting now\r");
      Rtc.SetIsRunning(true);
      };

  // just in case force upate of RTC from NTP server or GPS if available
  resync_counter = 0;
   };   // end of has_DS3231
   
 
  timerHndl1Sec = xTimerCreate(
      "timer1Sec", // name 
      pdMS_TO_TICKS(100), // period/time 100 msec 
      pdFALSE, // one shot timer 
      (void*)0, // timer ID 
      vTimerCallback1SecExpired); // callback 

  if (timerHndl1Sec==NULL) {
      for(;;); // failure! 
      };
  
  timerGPS_Status = xTimerCreate(
      "timerGPS_Status", // name 
      pdMS_TO_TICKS(1000), // period/time 1000 msec 
      pdTRUE, // repetitive shot timer 
      (void*)0, // timer ID 
      vTimerCallbackGPSExpired); // callback 
  if (timerGPS_Status==NULL) {
      for(;;); // failure! 
      };
  // start now this repetitive timer 
  if (xTimerStart(timerGPS_Status, 0)!=pdPASS) {
    for(;;); // failure!?! 
    };


  timerPPS_LOOP = xTimerCreate(
      "timerPPS_LOOP", // name 
      pdMS_TO_TICKS(1000), // period/time 1000 msec 
      pdTRUE, // repetitive shot timer 
      (void*)0, // timer ID 
      vTimerCallbackLoopPPSExpired); // callback 

  if (timerPPS_LOOP==NULL) {
      for(;;); // failure! 
      };
  // start now this repetitive timer 
  if (xTimerStart(timerPPS_LOOP, 0)!=pdPASS) {
    for(;;); // failure!?! 
    };

  return ;  
}

//********************************** RTC DS3231 loop    ****************************************************
void DS3231_loop() {
  if(! has_DS3231){
        return ;
  };
        
    LastFunction = __func__ ;

  /*
  if(ppsFlag == true){   // allows to execute this segment only every 1 sec without blocking the main loop
       ppsFlag = false;
       RtcDateTime currTime;
       if(RTC_debug){ 
         if (xSemaphoreTake(i2c_mutex_v, ( TickType_t ) 2) ){
           LastFunction = __func__ + String("_xSemaphoreTake");
           if(RTC_debug) debugA("DS3231_loop: GPS_time_status=%d - NTP_time_status=%d - now/millis=%d/%d\r", GPS_time_status, NTP_time_status , now(),millis());  
           if(RTC_debug) debugA("DS3231_loop: >>> ezTime UTC.dateTime= %s \r" , UTC.dateTime(now(), F("d/n/Y H:i:s.v T")).c_str());
           currTime = Rtc.GetDateTime(); displayRTC_DateTime(currTime);
           if(RTC_debug) debugA("DS3231_loop: >>  Rtc.GetDateTime:%s\r", RTCdatestring );  
           xSemaphoreGive(i2c_mutex_v);
           };
         };
    
       if( ! resync_counter ){
          if((GPS_time_status == 2) || (NTP_time_status == 2) ){ // force RTC synch only if time references are available
             if(RTC_debug) debugA("   DS3231_loop: performing scheduled Rtc.SetDateTime adjustment...\r");
             if(xSemaphoreTake(i2c_mutex_v, ( TickType_t ) 2) ){
                LastFunction = __func__ + String("_xSemaphoreTake_currTime");
                currTime = Rtc.GetDateTime(); displayRTC_DateTime(currTime);
                if(RTC_debug) debugA("DS3231_loop: >>>> before Rtc.SetDateTime(now() -> Rtc.GetDateTime: %s\r", RTCdatestring);
                Rtc.SetDateTime(now() - 946684800UL);
                currTime = Rtc.GetDateTime(); displayRTC_DateTime(currTime);
                if(RTC_debug) debugA("DS3231_loop: >>>>> after Rtc.SetDateTime(now() -> Rtc.GetDateTime: %s\r", RTCdatestring); 
                xSemaphoreGive(i2c_mutex_v);
                };
             }
          else{
             if(RTC_debug) debugA("DS3231_loop: skipping scheduled Rtc.SetDateTime adjustment: time reference not available\r"); 
             };  // end of time_status reference check

          CheckRTC_Valid();  //  check  RTC validity      by MF 20220318    
          }; // end of resync check

       if(resync_counter++ > resync_counter_trsh){
          LastFunction = __func__ + String("_resync_counter");        
          resync_counter = 0; // schedules next RTC update at second boundary only after resync_counter_trsh seconds
          if(RTC_debug) debugA("DS3231_loop: RTC resync scheduled ");
          if (resync_counter_trsh < 600){
              resync_counter_trsh = 2*resync_counter_trsh;
              if(RTC_debug) debugA("\nDS3231_loop: resync_counter_trsh is now: %d\r ", resync_counter_trsh);
              };
          };
    };
*/
    // check if RTC is invalid and resync with NTP
    LastFunction = __func__ + String("_CheckRTC_Valid");            
    //    if (minuteChanged()) CheckRTC_Valid();
   return ;
}


void ClockManager( void * pvParameters ){      //ClockManager: task to keep updated local time clock available
  LastFunction = __func__ ;
  int counter5 =0;  
   debugA("ClockManager: starting on core=%d\r",xPortGetCoreID() ); 
   PPS_Timers_setup();
   DS3231_Setup();
  
   while (! gps_initialized){    // wait for GPS engine discovery: 1--> GPS discovery and initialization passed ( gps_initialized is an atomic variable)
     vTaskDelay(500);            // just release processor for a while...
     };  
   
   debugA( "Starting ezTime services setup...\r" );
   setDebug(INFO);  // set ezTime debug level
   Italy.setLocation("Europe/Rome");


#ifdef EZTIME_GPS_RTC
   debugA( "Using EZTIME_GPS LIB\r" );
   setServer(ESP_Config.ntpServerName); 
   ezTime_setup();
#else
   debugA( "Using standard EZTIME LIB\r" );
   if( ! no_gps){
     debugA( "GPS support not available: to enable it please select EZTIME_GPS_RTC; forcing no_gps\r" );
     no_gps = true ;
    };
    
  // setup timezone
  Italy.setLocation("Europe/Rome");
  Italy.setDefault();
  debugA("Setup timezone Italy time: " + Italy.dateTime()+"\r");
  setServer(ESP_Config.ntpServerName); 
  setInterval(1);
  vTaskDelay(pdMS_TO_TICKS(3000));
  events();
  if( ! standalone){
    setInterval(60);
    NTP_status = waitForSync(5);     
    }
  else {
    setInterval(0);
    NTP_status = false;
    };

  if(NTP_status) {NTP_time_status = timeSet;} else {NTP_time_status = timeNotSet ;};
#endif
   if( ! last_aux_time )   last_aux_time=now();

//*************************  Set real time based on time reference availability or RTC content as a default 
   debugA("Set real time based on time reference availability at %d/%d %s\r",last_aux_time,millis(), (UTC.dateTime(last_aux_time, "d/n/Y H:i:s.v T")).c_str());
   debugA("GPS_time_status=%d - GPS_last_sync_time=%d - has_GPS = %d - no_gps = %d\r",GPS_time_status, GPS_last_sync_time, has_GPS , no_gps);  // time & millis of last GPS_PPS rising edge
   debugA("NTP_time_status=%d - NTP_last_sync_time=%d - standalone = %d \r",NTP_time_status,lastNtpUpdateTime(), standalone);  

   // try to set op_status
   if( ( ( no_gps || ! has_GPS || (GPS_time_status != 2))  && (NTP_time_status != 2) ) && has_DS3231){   // no sync source available-> fallback to RTC ds3231 based operation 
       op_status = 1 ;    // RTC_disciplined 
       setInterval(0);     
       debugA("only RTC DS3231 available for realtime setup ---> use it...\r");    
       RtcDateTime rtc_time = Rtc.GetDateTime();
       debugA("unix time=%d - UTC.dateTime: %s\r", now(),( UTC.dateTime(now(), "d/n/Y H:i:s.v T") ).c_str())  ;
       setTime((rtc_time + 946684800UL));
       vTaskDelay(pdMS_TO_TICKS(30));
       debugA("rtc_time=%d - unix time=%d - UTC.dateTime= %s\r", rtc_time,now(), ( UTC.dateTime(now(), "d/n/Y H:i:s.v T")).c_str() );
       debugA("missing time references: entering RTC disciplined operation\r"); 
      }
   else if ((no_gps || ! has_GPS || (GPS_time_status != 2)) && (NTP_time_status == 2) ){      // only NTP available  --> use it 
       op_status = 2 ;     // NTP_disciplined 
       setInterval(61);
       debugA("entering NTP disciplined operation: realtime derived from NTP\r");      
      }
   else if (GPS_time_status == 2){  // gps available
       op_status = 3 ;     // GPS_disciplined
       setInterval(301);
       debugA("entering GPS disciplined operation: realtime derived from GPS\r");            
      }
  else {  
       setInterval(0);                  
       debugA("entering free running operation: realtime not available\r");      
      };
  ezt::update_NTP_GPS(); // perform ezTime time update
  debugA("End of ezTime synchronization init: op_status = %d\r", op_status);

  
  for(;;){ 
//   ClockManager_flag = true ;  // 
     DS3231_loop();  // perform DS3231 repetitive operations to keep closk alligned to GPS end NTP if available
     PPS_loop();     // executes every 1 sec - non blocking 
     events();       // alows ezTime event handling
     counter5++;
     vTaskDelay(50); // just release processor for a while...
     }               // end of task endless loop
  return ;   
}   // end of ClockManager



void check_time_sync_source_availability(void){
   LastFunction = __func__ ; 
   int new_op_status = 0 ;
   bool ezTime_debug_old = ezTime_debug; // ezTime_debug= 1 ;
   if(ezTime_debug || RTC_debug){ 
      if( ! last_aux_time )   last_aux_time=now();
//      debugA("check_time_sync_source_availability at %d/%d %s\r",now(),millis(), (UTC.dateTime(now(), "d/n/Y H:i:s.v T")).c_str());
      debugA("check_time_sync_source_availability at %d/%d %s\r",last_aux_time,millis(), (UTC.dateTime(last_aux_time, "d/n/Y H:i:s.v T")).c_str());
      debugA("GPS_time_status=%d - GPS_last_sync_time=%d - has_GPS = %d - no_gps = %d\r",GPS_time_status, GPS_last_sync_time, has_GPS , no_gps);  // time & millis of last GPS_PPS rising edge
      debugA("NTP_time_status=%d - NTP_last_sync_time=%d - standalone = %d \r",NTP_time_status,lastNtpUpdateTime(), standalone);  
      }  

   ezt::update_NTP_GPS(); // perform ezTime time update
   
   //*************************  Set real time based on time reference availability or RTC content as a default 
   if(ezTime_debug){ 
      debugA("After update_NTP_GPS and updateNTP checks ....\r"); 
      debugA("GPS_time_status=%d - GPS_last_sync_time=%d - has_GPS = %d - no_gps = %d\r",GPS_time_status, GPS_last_sync_time, has_GPS , no_gps);  // time & millis of last GPS_PPS rising edge
      debugA("NTP_time_status=%d - NTP_last_sync_time=%d - standalone = %d \r",NTP_time_status,lastNtpUpdateTime(), standalone);  
      };
   // try to set op_status
   if( ( ( no_gps || ! has_GPS || (GPS_time_status != 2))  && (NTP_time_status != 2) ) && has_DS3231){   // no sync source available-> fallback to RTC ds3231 based operation 
       new_op_status = 1 ;    // RTC_disciplined 
       if(ezTime_debug) debugA("response: missing time references; needs to enter RTC disciplined operation \r"); 
      }
   else if ((no_gps || ! has_GPS || (GPS_time_status != 2)) && (NTP_time_status == 2) ){      // only NTP available  --> use it 
       new_op_status = 2 ;     // NTP_disciplined 
       if(ezTime_debug) debugA("response: can enter NTP disciplined operation; realtime can be derived from NTP\r");      
      }
   else if (GPS_time_status == 2){  // gps available
       new_op_status = 3 ;     // GPS_disciplined
       if(ezTime_debug) debugA("response: can enter GPS disciplined operation; realtime can be derived from GPS\r");            
      }
  else {  
///////     op_status = 0 ;   /// DO NOT CHANGE STATUS... JUST WAIT
       if(ezTime_debug) debugA("response: no sync source available: needs entering free running operation; realtime not available\r");      
      };
      
 
  if((new_op_status == 3 ) && ( ! no_gps ) &&  has_GPS && ( op_status != 3) ) {
     if(ezTime_debug) debugA("End of ezTime synchronization check: Immediatelly entering GPS synchronized status: old op_status= %d - new_op_status = %d\r", op_status,  new_op_status);
     op_status = 3 ;
     }
  else if((new_op_status == 2 ) && ( ! standalone ) && (  op_status != 2)) {
     if(ezTime_debug) debugA("End of ezTime synchronization check: Immediatelly entering NTP synchronized status: old op_status= %d - new_op_status = %d\r", op_status,  new_op_status);
     op_status = 2 ;
     }
  else if((new_op_status == 1 ) && ( has_DS3231 ) && (  op_status != 1)) {
     if(ezTime_debug) debugA("End of ezTime synchronization check: Immediatelly entering RTC synchronized status: old op_status= %d - new_op_status = %d\r", op_status,  new_op_status);
     op_status = 1 ;
     }
  else if((new_op_status == 0 ) && (  op_status != 0)) {
     if(ezTime_debug) debugA("End of ezTime synchronization check: Immediatelly entering free running status: old op_status= %d - new_op_status = %d\r", op_status,  new_op_status);
///////     op_status = 0 ;   /// DO NOT CHANGE STATUS... JUST WAIT
     }
  else {   
     if(ezTime_debug) debugA("End of ezTime synchronization check: old op_status= %d - new_op_status = %d\r", op_status,  new_op_status);
     };

  //if(syslog_available) syslog.logf(LOG_INFO, "check_time_sync_source_availability:UTC.dateTime: %s ",  (UTC.dateTime(now(), "d/n/Y H:i:s.v T")).c_str());
  ezTime_debug = ezTime_debug_old ;     
  return ;
}

void printLocalTime(){
  struct tm timeinfo;
  if(!getLocalTime(&timeinfo)){
     debugA("Failed to obtain time\r");
     return;
     };
//   Serial.printf("printLocalTime: Success to obtain NTP time: %s\r", (&timeinfo, "%A, %B %d %Y %H:%M:%S"));
   Serial.printf("printLocalTime: Success to obtain NTP time: "); Serial.println(&timeinfo, "%A, %B %d %Y %H:%M:%S");

   
   return ;
}
