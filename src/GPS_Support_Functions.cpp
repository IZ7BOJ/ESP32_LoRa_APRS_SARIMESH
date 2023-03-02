// GPS_Support_Functions.cpp

#include "GPS_Support_Functions.h"

// The TinyGPSPlus objects we need/use
TinyGPSPlus gps;
TinyGPSCustom totalGPGSVMessages(gps, "GPGSV", 1); // $GPGSV sentence, first element
TinyGPSCustom messageNumber(gps, "GPGSV", 2);      // $GPGSV sentence, second element
TinyGPSCustom satsInView(gps, "GPGSV", 3);         // $GPGSV sentence, third element
TinyGPSCustom satNumber[4]; // to be initialized later
TinyGPSCustom elevation[4];
TinyGPSCustom azimuth[4];
TinyGPSCustom snr[4];

char buf[32];


// GPS related additional routines 

void gps_dump(){
  // dump RX1 from GPS engine
  while (Esp32Serial1.available() > 0) {
    char inByte = Esp32Serial1.read();
    Serial.write(inByte);
    };
}


void show_gps_status(){
  debugA("Please wait: collecting GPS Detailed Sats information...");
  // prints on debug console GPS infos 
  // it reports main GPS status infos and detailed extended infos
  int wait_for_gps_status = 60 ;
  while( wait_for_gps_status > 0 ){
    if (totalGPGSVMessages.isUpdated()) {     // satellite status is collected slowly and made available nearly every 30 secs
      debugAl("\r\n");  // got global picture of all visible sats
      for (int i=0; i<4; ++i){  // enumerate sats and populate local array with relevant data from GPS engine
        int no = atoi(satNumber[i].value());
        // debugAl("SatsNumber=%d ", no);
        if (no >= 1 && no <= MAX_SATELLITES) {
          sats[no-1].elevation = atoi(elevation[i].value());
          sats[no-1].azimuth = atoi(azimuth[i].value());
          sats[no-1].snr = atoi(snr[i].value());
          sats[no-1].active = true;
          };
        };
      if (totalMessages == currentMessage) {      // lists only actively used sats
        debugAl("\r\n Total Sats                  = %d" , gps.satellites.value());
        debugAl("\r\n PRN_Nums                    = " );
        for (int i=0; i<MAX_SATELLITES; ++i) if (sats[i].active) { debugAl("%d ",i+1); };  
        debugAl("\r\n Elevation                   = ");
        for (int i=0; i<MAX_SATELLITES; ++i) if (sats[i].active){ debugAl("%d ",sats[i].elevation); };
        debugAl("\r\n Azimuth                     = ");
        for (int i=0; i<MAX_SATELLITES; ++i) if (sats[i].active) { debugAl("%d ", sats[i].azimuth); };
        debugAl("\r\n SNR                         = ");
        for (int i=0; i<MAX_SATELLITES; ++i)if (sats[i].active) { debugAl("%d ",sats[i].snr);  };
        debugAl("\r\n");
        for (int i=0; i<MAX_SATELLITES; ++i) sats[i].active = false;
        };  // end of totalMessages == currentMessage
      wait_for_gps_status = 0;
      debugAl("\r\n");
    }  // end of totalGPGSVMessages.isUpdated()
else{
    wait_for_gps_status--;
    debugAl(".");
    vTaskDelay(pdMS_TO_TICKS(1000)); 
    };
  }; // end of while
  debugAl("\r\n");  
  show_gps_loop_extended(); // printouts extended GPS infos
  if (millis() > 5000 && gps.charsProcessed() < 10 && ! no_gps){
    debugA("gps_status: No GPS detected, check wiring\r\n");    
    };
  debugA("show_gps command complete.");  
  return;
}

void show_gps_loop_extended(){
  static const double LONDON_LAT = 51.508131, LONDON_LON = -0.128002;
  debugAl(" GPS_status now --> [\r");  
  debugAl("\r\n Sats                        = "); printInt(gps.satellites.value(), gps.satellites.isValid(), 5);  // 1st value
  debugAl("\r\n HDOP                        ="); printFloat(gps.hdop.hdop(), gps.hdop.isValid(), 6, 1);
  debugAl("\r\n Latitude(deg)               = "); printFloat(gps.location.lat(), gps.location.isValid(), 11, 6);
  debugAl("\r\n Longitude(deg)              = "); printFloat(gps.location.lng(), gps.location.isValid(), 12, 6);
  debugAl("\r\n Fix Age                     = "); printInt(gps.location.age(), gps.location.isValid(), 5);
  debugAl("\r\n Fix Date/Time/Age           = "); printDateTime_gps(gps.date, gps.time);                          // day, hour, age time fix
  debugAl("\r\n Altitude(m)                 = "); printFloat(gps.altitude.meters(), gps.altitude.isValid(), 7, 2);  // 9 value
  debugAl("\r\n Course/Speed/Card           = "); printFloat(gps.course.deg(), gps.course.isValid(), 7, 2);
  printFloat(gps.speed.kmph(), gps.speed.isValid(), 6, 2);
  printStr(gps.course.isValid() ? TinyGPSPlus::cardinal(gps.course.deg()) : "*** ", 6);
  unsigned long distanceKmToLondon = (unsigned long)TinyGPSPlus::distanceBetween( gps.location.lat(), gps.location.lng(), LONDON_LAT,  LONDON_LON) / 1000;
  debugAl("\r\n Home Dist/Course/Card       = ");
  printInt(distanceKmToLondon, gps.location.isValid(), 9);
  double courseToLondon = TinyGPSPlus::courseTo( gps.location.lat(), gps.location.lng(), LONDON_LAT,  LONDON_LON);
  printFloat(courseToLondon, gps.location.isValid(), 7, 2);
  const char *cardinalToLondon = TinyGPSPlus::cardinal(courseToLondon);
  printStr(gps.location.isValid() ? cardinalToLondon : "*** ", 6);
  debugAl("\r\n GPS engine charsProcessed   = "); printInt(gps.charsProcessed(), true, 6);
  debugAl("\r\n GPS engine sentencesWithFix = "); printInt(gps.sentencesWithFix(), true, 10);
  debugAl("\r\n GPS engine failedChecksum   = "); printInt(gps.failedChecksum(), true, 9);
  debugAl("\r\n]\r\n");
  return ;
}


void gps_status(){
  LastFunction = __func__ ;
  // prints on serial GPS infos every second or like that....
  // this routine is skeduled based on a 1 sec sw counter in freeruning mode
  // it reports main GPS status infos and on demand detailed extended infos
  if (totalGPGSVMessages.isUpdated()) {     // satellite status is collected slowly and made available nearly every 30 secs
    debugAl("\nGPS Detailed Sats information: ");  // got global picture of all visible sats
    for (int i=0; i<4; ++i){  // enumerate sats and populate local array with relevant data from GPS engine
      int no = atoi(satNumber[i].value());
      // debugAl("SatsNumber=%d ", no);
      if (no >= 1 && no <= MAX_SATELLITES) {
        sats[no-1].elevation = atoi(elevation[i].value());
        sats[no-1].azimuth = atoi(azimuth[i].value());
        sats[no-1].snr = atoi(snr[i].value());
        sats[no-1].active = true;
        };
      };

    totalMessages = atoi(totalGPGSVMessages.value());
    currentMessage = atoi(messageNumber.value());
    if (totalMessages == currentMessage) {      // lists only actively used sats
      debugAl(" Sats=%d  Nums=" , gps.satellites.value());
      for (int i=0; i<MAX_SATELLITES; ++i) if (sats[i].active) { debugAl("%d ",i+1); };  
      debugAl(" Elevation=");
      for (int i=0; i<MAX_SATELLITES; ++i) if (sats[i].active){ debugAl("%d ",sats[i].elevation); };
      debugAl(" Azimuth=");
      for (int i=0; i<MAX_SATELLITES; ++i) if (sats[i].active) { debugAl("%d ", sats[i].azimuth); };
      debugAl(" SNR=");
      for (int i=0; i<MAX_SATELLITES; ++i)if (sats[i].active) { debugAl("%d ",sats[i].snr);  };
      debugAl("\r");
      for (int i=0; i<MAX_SATELLITES; ++i) sats[i].active = false;
    };  // end of totalMessages == currentMessage
  } ; // end of totalGPGSVMessages.isUpdated()
  debugAl("\r\n");  
      
//  debugA("                      Sats HDOP  Latitude   Longitude   Fix  Date       Time     Date Alt    Course Speed Card  Distance Course Card  Chars Sentences Checksum\n");
//  debugA("                                 (deg)      (deg)       Age                      Age  (m)    --- from GPS ----  ---- to London  ----  RX    RX        Fail\n");
//  debugA("                      ----------------------------------------------------------------------------------------------------------------------------------------\n");
  gps_loop_extended(); // printouts extended GPS infos

  if (millis() > 5000 && gps.charsProcessed() < 10 && ! no_gps){
    debugA("gps_status: No GPS detected, check wiring\r\n");    
    };
  return;
}


void gps_status_header(){
  debugA("                      Sats HDOP  Latitude   Longitude   Fix  Date       Time     Date Alt    Course Speed Card  Distance Course Card  Chars Sentences Checksum\n");
  debugA("                                 (deg)      (deg)       Age                      Age  (m)    --- from GPS ----  ---- to London  ----  RX    RX        Fail\n");
  debugA("                      ----------------------------------------------------------------------------------------------------------------------------------------\n");
}

void gps_now(){
  LastFunction = __func__ ;
  char sz[32];
  //  printInt(gps.satellites.value(), gps.satellites.isValid(), 5);
  //  printFloat(gps.location.lat(), gps.location.isValid(), 11, 6);
  //  printFloat(gps.location.lng(), gps.location.isValid(), 12, 6);
  //  printDateTime_gps(gps.date, gps.time);
  sprintf(sz, " %02d/%02d/%02d ",gps.date.day(), gps.date.month(), gps.date.year()  );
  debugAl(sz);;    
  sprintf(sz, " %02d:%02d:%02d ", gps.time.hour(), gps.time.minute(), gps.time.second()+1 );
  debugAl(sz);;    
  // sprintf(sz, " - gps.time.value() = %12d  ",  gps.time.value());
  // Serial.print(sz);;
  printFloat(gps.time.value(), gps.location.isValid(), 11, 0);
  debugA("\r\n"); 
}

void gps_loop_extended(){
  LastFunction = __func__ ;
  static const double LONDON_LAT = 51.508131, LONDON_LON = -0.128002;
  debugAl("\r\rGPS_status --> [");
  printInt(gps.satellites.value(), gps.satellites.isValid(), 5);  // 1st value
  printFloat(gps.hdop.hdop(), gps.hdop.isValid(), 6, 1);
  printFloat(gps.location.lat(), gps.location.isValid(), 11, 6);
  printFloat(gps.location.lng(), gps.location.isValid(), 12, 6);
  printInt(gps.location.age(), gps.location.isValid(), 5);
  printDateTime_gps(gps.date, gps.time);                          // day, hour, age time fix
  printFloat(gps.altitude.meters(), gps.altitude.isValid(), 7, 2);  // 9 value
  printFloat(gps.course.deg(), gps.course.isValid(), 7, 2);
  printFloat(gps.speed.kmph(), gps.speed.isValid(), 6, 2);
  printStr(gps.course.isValid() ? TinyGPSPlus::cardinal(gps.course.deg()) : "*** ", 6);
  unsigned long distanceKmToLondon = (unsigned long)TinyGPSPlus::distanceBetween( gps.location.lat(), gps.location.lng(), LONDON_LAT,  LONDON_LON) / 1000;
  printInt(distanceKmToLondon, gps.location.isValid(), 9);
  double courseToLondon = TinyGPSPlus::courseTo( gps.location.lat(), gps.location.lng(), LONDON_LAT,  LONDON_LON);
  printFloat(courseToLondon, gps.location.isValid(), 7, 2);
  const char *cardinalToLondon = TinyGPSPlus::cardinal(courseToLondon);
  printStr(gps.location.isValid() ? cardinalToLondon : "*** ", 6);
  printInt(gps.charsProcessed(), true, 6);
  printInt(gps.sentencesWithFix(), true, 10);
  printInt(gps.failedChecksum(), true, 9);
  debugAl("]\r");
  return ;
}


// This custom version of delay () ensures that the gps object is being "fed".
static void smartDelay(unsigned long ms){
  unsigned long start = millis();
  do  {
    while (Esp32Serial1.available())
      gps.encode(Esp32Serial1.read());
  } while (millis() - start < ms);
}

static void printFloat(float val, bool valid, int len, int prec){
  if (!valid){
    while (len-- > 1)
      debugAl("*"); debugAl(" ");      
      }
  else {
    debugAl("%8.5f", val);
    int vi = abs((int)val);
    int flen = prec + (val < 0.0 ? 2 : 1); // . and -
    flen += vi >= 1000 ? 4 : vi >= 100 ? 3 : vi >= 10 ? 2 : 1;
    for (int i=flen; i<len; ++i)
    debugAl(" ");
  };
  return ;
}


static void printInt(unsigned long val, bool valid, int len){
  char sz[32] = "*****************";
  if (valid) sprintf(sz, "%ld", val);
  sz[len] = 0;
  for (int i=strlen(sz); i<len; ++i) sz[i] = ' ';
  if (len > 0)  sz[len-1] = ' ';
  debugAl("%s",sz);
  return ;
}


static void printDateTime_gps(TinyGPSDate &d, TinyGPSTime &t){
  if (!d.isValid()) {
    debugAl("********** ");
    }
  else {
    char sz[32];
    sprintf(sz, "%02d/%02d/%02d ", d.month(), d.day(), d.year());
    debugAl("%s",sz);
    };
  
  if (!t.isValid()) {
     debugAl("******** ");
     }
  else {
    char sz[32];
    sprintf(sz, "%02d:%02d:%02d ", t.hour(), t.minute(), t.second());
    debugAl("%s",sz);
    };

  printInt(d.age(), d.isValid(), 5);
}


static void printStr(const char *str, int len){
  int slen = strlen(str);
  for (int i=0; i<len; ++i)
    if(i<slen) {
      debugAl("%c",str[i]);
       }
    else {
       debugAl(" ");        
    };
  return;
}

void displayGPS_TimeDate(){
  debugA(">>GPS.DateTime: ");
  if (gps.date.isValid()){ 
    Serial.print(gps.date.day());
    Serial.print("/");
    Serial.print(gps.date.month());
    Serial.print("/");
    Serial.print(gps.date.year());
    }
  else{
    Serial.print("INVALID");
    };

  Serial.print(" ");
  if (gps.time.isValid()){
    if (gps.time.hour() < 10) Serial.print("0");
    Serial.print(gps.time.hour());
    Serial.print(":");
    if (gps.time.minute() < 10) Serial.print("0");
    Serial.print(gps.time.minute());
    Serial.print(":");
    if (gps.time.second() < 10) Serial.print("0");
    Serial.print(gps.time.second());
    Serial.print(".");
    if (gps.time.centisecond() < 10) Serial.print("0");
    Serial.print(gps.time.centisecond());
    }
  else{
    Serial.print("INVALID");
    };
  Serial.print("\r\n");
}

void displayGPS_Info(){
  Serial.print(">> Location: "); 
  if (gps.location.isValid()){
    Serial.print(gps.location.lat());
    Serial.print(",");
    Serial.print(gps.location.lng());
    }
  else{
    Serial.print("INVALID");
    };

  displayGPS_TimeDate();
}


void getGPSdata(){
  LastFunction = __func__ ; 
  // update present GPS position in string (lat_now,lon_now) , chararray (slat,slon) and decimal (homelat,homelon) global variables
  // Serial.print(gps.location.lat(), 6);Serial.print(",");Serial.println(gps.location.lng(), 6);
  if( !(xSemaphoreTake(LocationManager_mutex_v, ( TickType_t ) 2) )){ debugA("xSemaphoreTake FAILED: aborting getGPSdata"); return;  };
  
  if (gps.location.isValid()){  // we have a valid fix from the GPS engine update GPS readout !
    float latnum = gps.location.lat(); homelat = latnum;
    float lonnum = gps.location.lng(); homelon= lonnum; 
    gps_fix_available = 1 ; // flag that a GPS fix is available at moment
    }
  else{                              // GPS location fix is not valid at moment... get a workaround...
    if(( homelat != 0.0 ) &&  (homelon != 0.0)) {   // we had a valid position before...  just keep it and go on..
      if(gps_debug) debugA("\r=======> GPS location is not valid at moment.. keep old position \r");
      }
    else if(Loc_Lat && Loc_Lon ){     // use static location from GUI if available 
      // translate GUI APRS position in decimal position (float)
      String mylat_int = Loc_Lat.substring(0, 2) ; String  mylat_frac =Loc_Lat.substring(2, 7) ; float mylat_norm = mylat_int.toFloat() + mylat_frac.toFloat() /60.0 ;
      if ( Loc_Lat.substring(7) == "S") {mylat_norm = - mylat_norm ;};
      String mylon_int = Loc_Lon.substring(0, 3) ; String  mylon_frac =Loc_Lon.substring(3, 8) ; float mylon_norm = mylon_int.toFloat() + mylon_frac.toFloat() /60.0 ;
      if ( Loc_Lon.substring(8) == "W") {mylon_norm = - mylon_norm ;};  
      homelat=  mylat_norm ; homelon=mylon_norm ;
      gps_fix_available = 0 ; // flag that a GPS fix is NOT available at moment
      }
    else{                          // noting better possible....  assume we are at Rome - Caput Mundi !!!!!!!
      // this coud be upgraded in the future by introducing FRAM support for last reliable fix 
      homelat=41.904183;
      homelon=12.473612;
      gps_fix_available = 0 ; // flag that a GPS fix is NOT available at moment
      };
    };
  // translate homelat,homelon  in the APRS format in String and char array format
  int integer_part = floor(homelat); 
  float centi_part = fmod((homelat*60.0),60);
  char temp[3]; sprintf(temp,"%02d",integer_part);
  String latCents = String(centi_part,2);
  if(latCents.length()< 5){latCents="0" + latCents;};
  if(homelat>0){lat_now = String(temp) + latCents + String("N");}
  else{lat_now = String(temp) + latCents + String("S");};
  integer_part = floor(homelon); 
  centi_part = fmod((homelon*60.0),60);
  sprintf(temp,"%03d",integer_part);
  String lonCents = String(centi_part,2); 
  if(lonCents.length()< 5){lonCents="0" + lonCents;};
  if(homelon>0){lon_now = String(temp) + lonCents + String("E");}
  else{lon_now = String(temp) + lonCents + String("W");};
  lat_now.toCharArray(slat, 9);
  lon_now.toCharArray(slon, 10);
   
  // float homelat,homelon ;           // present GPS location in decimal format as per GPS native fix data
  // String lat_now ; String lon_now ; // present GPS location in APRS String format 
  // char slat[9]; char slon[10];      // present GPS location in APRS formats char arrays
  // int gps_fix_available  ;          // 1--> a GPS fix is available ( either from gps or from manal setup) initialized as a trap to 1
  // if(gps_debug) debugA("gps_fix_available=%d - homelat=%8.5f - homelon=%8.5f - lat_now=%s - lon_now=%s -  slat=%s - slon=%s\r", gps_fix_available , homelat,homelon, lat_now.c_str(), lon_now.c_str(), slat, slon);
  // we can assume that a valid position is always reported irrelevant of possible unavailable GPS fix

  xSemaphoreGive(LocationManager_mutex_v);  // release mutex 
  return; 
}


void gps_setup(){
  LastFunction = __func__ ;
  debugA("Starting gps_setup ...  ");
  if(syslog_available) syslog.logf(LOG_INFO, "Starting gps_setup ...");

// set gps port at 4800 bts
/*
  Esp32Serial1.begin(9600);
  Esp32Serial1.print("$PUBX,41,1,0007,0003,4800,0*13\r\n");
  delay (100);
  Esp32Serial1.end();
  delay (100);
  Esp32Serial1.begin(4800);
*/
 // set gps port at 9600 bts
/*  
  Esp32Serial1.begin(4800);
  Esp32Serial1.print("$PUBX,41,1,0007,0003,9600,0*10\r\n");
  delay (100);
  Esp32Serial1.end();
  delay (100);
  
  Esp32Serial1.begin(9600);
*/

// Disable or rate limit gps sentences
  Esp32Serial1.print(F("$PUBX,40,VTG,0,0,0,0,0,0*5E\r\n"));  // disable VTG
  Esp32Serial1.print(F("$PUBX,40,GLL,0,0,0,0,0,0*5C\r\n"));  // disable GLL
  Esp32Serial1.print(F("$PUBX,40,GSA,0,35,0,0,0,0*78\r\n"));  // GSA ogni 35 sec
  Esp32Serial1.print(F("$PUBX,40,GSV,0,33,0,0,0,0*69\r\n"));  // GSV ogni 33 sec

//  Esp32Serial1.print("$PUBX,40,GSA,0,0,0,0,0,0*4E\r\n");  // GSA disabled
//  Esp32Serial1.print("$PUBX,40,GSV,0,0,0,0,0,0*59\r\n");  // GSV disabled

  Esp32Serial1.print(F("$PUBX,40,GGA,0,3,0,0,0,0*59\r\n"));  // GGA ogni 3 sec
  Esp32Serial1.print(F("$PUBX,40,RMC,0,1,0,0,0,0*46\r\n"));  // RMC ogni 1 sec
//  Esp32Serial1.print(F("$PUBX,40,RMC,0,5,0,0,0,0*42\r\n"));  // RMC ogni 5 sec
//  Esp32Serial1.print(F("$PUBX,40,GGA,0,17,0,0,0,0*6C\r\n"));  // GGA ogni 17 sec
//  Esp32Serial1.print(F("$PUBX,40,RMC,0,13,0,0,0,0*75\r\n"));  // RMC ogni 13 sec

// initialize GPS printout 
  if(gps_debug){
    debugA("Sats HDOP  Latitude   Longitude   Fix  Date       Time     Date Alt    Course Speed Card  Distance Course Card  Chars Sentences Checksum\r");
    debugA("           (deg)      (deg)       Age                      Age  (m)    --- from GPS ----  ---- to London  ----  RX    RX        Fail\r");
    debugA("----------------------------------------------------------------------------------------------------------------------------------------\r");
    };
  vTaskDelay(pdMS_TO_TICKS(3000));
  // Initialize all the uninitialized TinyGPSCustom objects
  for (int i=0; i<4; ++i) {
    satNumber[i].begin(gps, "GPGSV", 4 + 4 * i); // offsets 4, 8, 12, 16
    elevation[i].begin(gps, "GPGSV", 5 + 4 * i); // offsets 5, 9, 13, 17
    azimuth[i].begin(gps, "GPGSV", 6 + 4 * i); // offsets 6, 10, 14, 18
    snr[i].begin(gps, "GPGSV", 7 + 4 * i); // offsets 7, 11, 15, 19
    };
  if(gps_debug) debugA("done.\r");

}


void gps_loop(){
  LastFunction = __func__ ;
  // check if the recursive timer for the GPS status printout is elapsed, and perform it just in case...
  if (gpsFlag == true){   // gpsFlag allows to execute this segment only every 1 sec without blocking any other loop
    gpsFlag = false;
    if(gps_debug) gps_status();           // print debug infos for GPS operation
      getGPSdata();                         // update present GPS location data
      };
    //   This snaplet lets collect GPS data as fast as possible and pass it to the encode function to collect and report NMEA sentencies updating GPS status data
    if(has_GPS && ! no_gps){    // scan GPS interface only if GPS is available and is not disabled by command 
      int gps_chars=0;
      while (Esp32Serial1.available() > 0){
        if(gps_chars++ > 250) return ;   // abandon gps if too many chars received...  may be it is a fake..
        LastFunction = __func__ + String("_Esp32Serial1.available") ;
        gps.encode(Esp32Serial1.read());
        };      // end of while  (Esp32Serial1.available() > 0) 
      };   
}


void gps_detect(){ // try to detect if GPS is equipped and working
  LastFunction = __func__ ;
  if(syslog_available) syslog.logf(LOG_INFO, "Starting gps_detect ...");
  uint32_t start_gps_test= millis();
  while ((millis() - start_gps_test) < 10000 ){
    while (Esp32Serial1.available() > 0){   // try to read something from the GPS
      gps.encode(Esp32Serial1.read());
      }; // end of while  (Esp32Serial1.available() > 0) 
    debugA("GPS charsProcessed=%d - passedChecksum=%d\r", gps.charsProcessed(), gps.passedChecksum()  );
    if ( gps.passedChecksum() > 3){
      debugA("\rgps.passedChecksum=%d\r",gps.passedChecksum());
      has_GPS = true ;
      return; 
      };
    vTaskDelay(pdMS_TO_TICKS(1000));  //  20211222
  };
  has_GPS = false ; 
  debugA("\rno GPS engine found: gps.passedChecksum=%d ", gps.passedChecksum());
  return;
}


void LocationManager( void * pvParameters ){      //LocationManager: task to keep updated location data available
  LastFunction = __func__ ;
  int counter3 =0;  
  debugA("starting on core=%d - gps_rx=%d - gps_tx=%d\r",xPortGetCoreID(),gps_rx, gps_tx  ); 
  // we  should have available a GPS engine: try to initialize it  
  // initialize serial port 1 to talk with the GPS.
  Esp32Serial1.begin(9600, SERIAL_8N1, gps_rx, gps_tx);    // RX1/TX1  --> GPS interface    
  gps_setup();
  gps_detect();
  if ( ! has_GPS || no_gps ) { 
    debugA("\rGPS engine NOT found or not to be used by configuration\r"); 
    }
  else{
    debugA("\rGPS engine found !!!!!\r"); 
    };

  gps_initialized = 1 ; // flag end of GPS initialization
  // we have now a clear picture of GPS engine availability..  go on with permanet GPS status update
  if(syslog_available) syslog.logf(LOG_INFO, "Starting LocationManager infinite loop ...");
  for(;;){ 
    // LocationManager_flag = true ;  // 
    // if(gps_debug){debugA("LocationManager running on core=%d - LocationManager_flag=%d - localcounter=%d\r\n",xPortGetCoreID(),LocationManager_flag, counter3); };
     gps_loop(); // perform GPS serial polling and periodic GPS status update; 
     counter3++;
     vTaskDelay(pdMS_TO_TICKS(200)); // just release processor for a while...
    }                    // end of task endless loop
}   // end of locationManager


// GPS passtrougth loop to check GPS fron U-center
void loop_GPS_Control() {
  while (Esp32Serial1.available() > 0) {
    char inByte = Esp32Serial1.read();
    Serial.write(inByte);
    };

  while (Serial.available() > 0 ) {
    char inByte = Serial.read();
    Esp32Serial1.write(inByte);
    }; 
}
