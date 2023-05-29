// Lora_Radiolib_Support_Functions.cpp
#include "Lora_Radiolib_Support_Functions.h"

// LoRa APRS additional functions

void initializeLoraPrsConfig() {
   LastFunction = __func__ ;  
   // this function will populate the global cfg structure that keeps all the relavant data for the LORA_APRS application, copied from the GUI interface  
   // this structure does not contain features related to the actual Lora HW in use
   cfg.LoraFreq = ESP_Config.LoraFreq.toFloat() * (1.0 + (float)(ESP_Config.LoraFreqCorr) /10000000.0) ; 
   cfg.LoraBw = ESP_Config.LoraBw.toFloat();  
   cfg.LoraSf = ESP_Config.LoraSf;
   cfg.LoraCodingRate = ESP_Config.LoraCodingRate;
   cfg.LoraSync = ESP_Config.LoraSync;
   cfg.LoraPower = ESP_Config.LoraPower;
   cfg.LoraPreambleLen = ESP_Config.LoraPreambleLen;
   // aprs configuration
   cfg.AprsHost = ESP_Config.AprsHost;
   cfg.AprsPort = ESP_Config.AprsPort;
   cfg.AprsLogin = ESP_Config.AprsLogin;
   cfg.AprsPass = ESP_Config.AprsPass;
   // These are typical filter values   
   //  cfg.AprsFilter = "r/35.60/139.80/25"; // multiple filters are space separated ( es. range latid long distanza 
   //  cfg.AprsFilter = "m/20"; // solo stazioni entro 50 Km dalla mia posizione
   // the RawBeacon message will be patched in realtime with present GPS coordinaates
   // ex. tracker cfg.AprsRawBeacon = "I8FUC-7>APZMDM,WIDE1-1:!GPS_LAT/GPS_LON#LoRa,15.6/SF9";  
   // ex. iGate cfg.AprsRawBeacon = "IQ8SO-10>APZMDM,WIDE1-1:!GPS_LAT/GPS_LON#iGate-LoRa, 15.6/SF9 ";
   debugA("initializeLoraPrsConfig:  ESP_Config.LoraFreq=%s ESP_Config.LoraBw=%s  --> cfg.LoraFreq=%f \r",ESP_Config.LoraFreq.c_str(), ESP_Config.LoraBw.c_str(),cfg.LoraFreq);

   if(iGate_Mode){  // iGate mode
      cfg.AprsRawBeacon = textToBeaconProto(ESP_Config.AprsRawBeaconIGATE);
      cfg.AprsRawBeaconPeriodSeconds = ESP_Config.AprsRawBeaconPeriodSecondsIGATE;
      cfg.AprsFilter = ESP_Config.AprsFilter;
      }
   else{  // tracker mode
      cfg.AprsRawBeacon = textToBeaconProto(ESP_Config.AprsRawBeaconTRACK );
      cfg.AprsRawBeaconPeriodSeconds = ESP_Config.AprsRawBeaconPeriodSecondsTRACK;
      };

   aprsBeacon_proto = cfg.AprsRawBeacon ;
   debugA("Using aprsBeacon_proto= %s \r",aprsBeacon_proto.c_str() );

   // bluetooth device name
   cfg.BtName = "loraprs";
   String _btName = WiFi.macAddress();
   _btName = "loraprs-" + _btName ;
   _btName.replace(":","");
   cfg.BtName = _btName;
   debugA("\rBtName= %s\r",cfg.BtName.c_str());

   // configuration flags and features
   cfg.EnableAutoFreqCorrection = false ;  // automatic tune to any incoming packet frequency
   cfg.EnableSignalReport = true;  // signal report will be added to the comment sent to aprsis
   cfg.EnablePersistentAprsConnection = true; // keep aprsis connection open, otherwise connect on new data only
   cfg.EnableBeacon = true;  // enable periodic AprsRawBeacon beacon to rf and aprsis if rf to aprsis is enabled
   cfg.EnableRfToIs  = EnableRfToIs ;
   cfg.EnableIsToRf  = EnableIsToRf ;
   cfg.EnableRepeater  = EnableRepeater;
   //  PrintLoraPrsConfig(); 
   debugA("initializeLoraPrsConfig: end of copy:  ESP_Config.LoraFreq=%s ESP_Config.LoraBw=%s  --> cfg.LoraFreq=%f \r",ESP_Config.LoraFreq.c_str(), ESP_Config.LoraBw.c_str(),cfg.LoraFreq);
}


bool setup_LoraPrs_Service(void) {
   LastFunction = __func__ ;
   initializeLoraPrsConfig();  // his will initialize the cfg global structure devoted to keep the LORA_APRS application configuration
   if ( loraPrsService.setup(cfg) ){  // this will actually setup the LORA_APRS applicaion based on the cfg structure
      return(true);
      };
   return(false);
}


void AprsMonitor(void){
   if (loraPrsService.needsAprsis()) {
      // display_event = 9 ;   // send message on local display
      loraPrsService.reconnectAprsis();
      };
};

// manages LoRa APRS service startup and loop in a single main loop function to sync with other processes...
bool loop_Lora_aprs() {
   LastFunction = __func__ ;
   if(! LoraPrs_Service_flag )   return (false) ;   // APRS apps are unavailable or still under initialization... just return...
         
   if(APRS_Service_initialized) {   // service is correctly initialized.. just do loop component of the service
      loraPrsService.aprs_loop();
      }
   else if(! BT_KISS_Mode && ! Serial_KISS_Mode && !TCP_KISS_Mode){  // just wait to let initialize LoRa_Device_Handler
      if ( LoRa_initialized ) {
         cfg.IsClientMode = false ;       // this is the normal iGate/Tracker operation mode....
         if ( ! setup_LoraPrs_Service()){   // we need to initialize this service... do it...
            debugA("LORA_APRS Services initialization FAILED: services are un-available !!!\r"); 
            LoraPrs_Service_flag = false ;
            return(false);
            };
         APRS_Service_initialized = true ;      
         };          
      }
   else if(BT_KISS_Mode || Serial_KISS_Mode || TCP_KISS_Mode){   // this is the KISS mode 
      if ( LoRa_initialized ) {
         cfg.IsClientMode = true ; 
         if ( ! setup_LoraPrs_Service()){
            debugA("LORA_APRS Services initialization FAILED: services are un-available !!!\r"); 
            LoraPrs_Service_flag = false ;
            return(false);
            };              
         APRS_Service_initialized = true ;                  
         };
      };  

   return(true);
}


void PrintLoraPrsConfig() {
   debugA("PrintLoraPrsConfig: dump active loraaprs configuration\r");
   // lora parameters
   debugA("LoRa configuration\r");
   debugA("cfg.LoraFreq:                       %f Mhz\r", cfg.LoraFreq );    //  
   debugA("cfg.LoraBw:                         %f Khz\r", cfg.LoraBw );    //  
   debugA("cfg.LoraSf:                         %d \r", cfg.LoraSf );    //  
   debugA("cfg.LoraCodingRate:                 %d \r", cfg.LoraCodingRate );    //  
   debugA("cfg.LoraSync:                       %02X \r", cfg.LoraSync );    //  
   debugA("cfg.LoraPower:                      %d dbm\r", cfg.LoraPower );    //  
   // aprs configuration
   debugA("aprs configuration\r");
   debugA("cfg.AprsHost:                       %s\r", cfg.AprsHost.c_str() );    // client/server mode switch
   debugA("cfg.AprsPort:                       %d\r", cfg.AprsPort );    // client/server mode switch
   debugA("cfg.AprsLogin:                      %s\r", cfg.AprsLogin.c_str() );    // client/server mode switch
   debugA("cfg.AprsPass:                       %s\r", cfg.AprsPass.c_str() );    // client/server mode switch
   debugA("cfg.AprsFilter:                     %s\r", cfg.AprsFilter.c_str() );    // client/server mode switch
   debugA("cfg.AprsRawBeacon:                  %s\r", cfg.AprsRawBeacon.c_str() );    // client/server mode switch
   debugA("cfg.AprsRawBeaconPeriodSeconds:     %d\r", cfg.AprsRawBeaconPeriodSeconds );    // client/server mode switch
   debugA("cfg.BtName:                         %s\r", cfg.BtName.c_str() );    // client/server mode switch
   debugA("cfg.WifiSsid:                       %s\r", cfg.WifiSsid.c_str() );    // client/server mode switch
   debugA("cfg.WifiKey:                        %s\r", cfg.WifiKey.c_str() );    // client/server mode switch
   // configuration flags and features
   debugA("configuration flags and features\r");
   debugA("cfg.EnableAutoFreqCorrection:       %d\r", cfg.EnableAutoFreqCorrection );    // 
   debugA("cfg.EnableSignalReport:             %d\r", cfg.EnableSignalReport );    // 
   debugA("cfg.EnablePersistentAprsConnection: %d\r", cfg.EnablePersistentAprsConnection );    // 
   debugA("cfg.EnableRfToIs:                   %d\r", cfg.EnableRfToIs );    // 
   debugA("cfg.IsClientMode:                   %d\r", cfg.IsClientMode );    // client/server mode switch
   debugA("cfg.EnableIsToRf:                   %d\r", cfg.EnableIsToRf );    // 
   debugA("cfg.EnableRepeater:                 %d\r", cfg.EnableRepeater );    // 
   debugA("cfg.EnableBeacon:                   %d\r", cfg.EnableBeacon );    // 
   debugA("\r");
}

void printWifiStatus() {
   // print the SSID of the network you're attached to:
   debugA("WiFi SSID: ");
   debugA("%s",WiFi.SSID());
   // print your WiFi shield's IP address:
   IPAddress ip = WiFi.localIP();
   debugA("WiFi IP Address: ");
   debugA("%s",ip);
   // print the received signal strength:
   long rssi = WiFi.RSSI();
   debugA("WiFi signal strength (RSSI):");
   debugA("%s",rssi);
   debugA(" dBm\r");
}


void sendUdpMsg(char * Msg) {  //data will be sent to the statistics server
   LastFunction = __func__ ;
   uint8_t buffer[520] = "hello world\r";
   strcpy((char *)buffer,Msg);
   //send hello world to server
   if(udp.beginPacket(udpAddress.c_str(), udpPort) == 1) {
      udp.write(buffer,strlen(Msg));
      if(udp.endPacket() == 1)  { 
         if(APRS_debug) debugA("sendUdpMsg: endPacket ok\r");
         }
      else { 
         if(APRS_debug) debugA("sendUdpMsg: endPacket fail\r"); 
         };
      }
   else {
      if(APRS_debug) debugA("sendUdpMsg: beginPacket fail\r");
      }; 
   memset(buffer, 0, 520);
   //processing incoming packet, must be called before reading the buffer
   /*  udp.parsePacket();
   //receive response from server, it will be HELLO WORLD
   if(udp.read(buffer, 520) > 0){
      debugA("Server to client: ");
      debugAl("%s",(char *)buffer);
      }
   */
}

// create log messages with a format compatible with direwolf; allows to use geolocation with sarimesh tools
// also creates and write a record o local FRAM log

bool send_log_msg(String textPayload , String signalReport, String my_spot){   // sends spots to our private server via UDP
   LastFunction = __func__ ;
   char Msg[1024]; char Msg1[512];
   time_t ora = ezt::now();
   bool local_is_CRC_Errored = false ;
   int posCRC_Errored = signalReport.indexOf("CRC_Errored"); 
   if(posCRC_Errored > 0) {
      local_is_CRC_Errored = true;
      sprintf(Msg,"2,%d",ora);  // for remote server
      sprintf(Msg1,"2,%d",ora);   // for local log in FRAM         
      }
   else{
      sprintf(Msg,"0,%d",ora);  // for remote server
      sprintf(Msg1,"0,%d",ora);   // for local log in FRAM       
      };  
   String message = Msg ;    // ex. 0,1628957071,
   String spot_time = Msg1 ; // ex. 1628957071    
   String time_now =  UTC.dateTime(",Ymd_His ");
   time_now.toCharArray(Msg,time_now.length() ); 
   message = message + Msg + "," ;        // ex. 0,1628957071,2021-08-14T18:04:31Z,
   // debugA("Setup timezone Italy time: " + Italy.dateTime("l, d/n/Y H:i:s.v T\r"));
   // sample payload strings:  I8FUC-7>APLS01,WIDE1-1:!4038.71N/01424.58E#LoRa,31.25/SF7 (I8FUC-7 Beacon  57.19) (IQ8SO-10 -90.00 3.25 0  77.22)

   String newDecMessage = APRS_Expand (textPayload); 
   textPayload = newDecMessage  ;

   int callIndex = textPayload.indexOf('>');
   int infoIndex = textPayload.indexOf(':');
   int posIndex = textPayload.indexOf('!'); if(posIndex <0) {posIndex = textPayload.indexOf('=');};
   int altposIndex = textPayload.indexOf('@') ;if(altposIndex <0) {altposIndex = textPayload.indexOf('/');};
   String sender_call = textPayload.substring(0, callIndex );
   String path = textPayload.substring(callIndex +1 , infoIndex);
   String info = textPayload.substring(infoIndex + 1);
   String mylat = "0000.00N"; String mylon = "00000.00E"; String com =""; String pos=""; int intIndex ;
   if(posIndex != -1 ) {            //  position based on lat lon
      pos = textPayload.substring(posIndex +1 , posIndex +19);
      com = textPayload.substring(posIndex +19 );
      intIndex = posIndex + 8 ;       // symbol table index  
      mylat = pos.substring(0, 8);
      mylon = pos.substring(9);
      }
   else if (altposIndex != -1 ) {  // position based on time lat lon 
      pos = textPayload.substring(altposIndex +1 , altposIndex +26);
      com = textPayload.substring(altposIndex +26 );
      intIndex = posIndex +15 ;       // symbol table index  
      String mytime = pos.substring(0, 7);
      mylat = pos.substring(7, 15);
      mylon = pos.substring(16);        
      }
   else{
      com = textPayload.substring(infoIndex + 1 );
      };
   // translate APRS position in decimal position (float)
   String mylat_int = mylat.substring(0, 2) ; String  mylat_frac =mylat.substring(2, 7) ; float mylat_norm = mylat_int.toFloat() + mylat_frac.toFloat() /60.0 ;
   if ( mylat.substring(7) == "S") {mylat_norm = - mylat_norm ;};
   String mylon_int = mylon.substring(0, 3) ; String  mylon_frac =mylon.substring(3, 8) ; float mylon_norm = mylon_int.toFloat() + mylon_frac.toFloat() /60.0 ;
   if ( mylon.substring(8) == "W") {mylon_norm = - mylon_norm ;};

   String first_node_rep , last_node_rep , last_node ;String DutyCycleNow ;
   if( textPayload.indexOf("LoRa")){   // only for lora packets try to extract node passage reports
      // extract first and last reception reports from incoming packet if LoRa packet
      int first_nodeIndex = textPayload.indexOf('(');
      int first_node_endIndex = textPayload.indexOf(')');
      first_node_rep = textPayload.substring(first_nodeIndex +1 , first_node_endIndex);   // 20230507
      //first_node_rep = textPayload.substring(first_nodeIndex +1 , first_node_endIndex - 1);  // by MF 20220130
      int last_nodeIndex = textPayload.lastIndexOf('(');
      int last_node_endIndex = textPayload.lastIndexOf(')');
      if( last_nodeIndex != -1){
         last_node_rep = textPayload.substring(last_nodeIndex +1 , last_node_endIndex);
         int last_node_nameIndex = last_node_rep.indexOf(' ');
//         DutyCycleNow = last_node_rep.substring(0,last_node_nameIndex);
         last_node = last_node_rep.substring(0,last_node_nameIndex);
         int nodeName_last_node_nameIndex = last_node_rep.indexOf(' ', last_node_nameIndex +1);
//         last_node = last_node_rep.substring(last_node_nameIndex + 1 , nodeName_last_node_nameIndex); 
         String thisnode = String(" ") + last_node + String(" ") ;
         if( currNodeList.indexOf(thisnode) == -1 ){
            currNodeList = currNodeList + last_node + String(" ") ;
            if(DutyCycleNow.toFloat() ){
               currChanCong = currChanCong + DutyCycleNow.toFloat();
               };
            };
         // debugA("=================== %s ============>>>>>>>> DutyCycleNow=[%s] --> currChanCong=%5.2f - last_node=%s\r",currNodeList.c_str() , DutyCycleNow.c_str(), currChanCong , last_node.c_str());        
         }
      else{
         last_node_rep = "NA";  
         last_node = "===NA==="; 
         DutyCycleNow = "0.0" ;                 
         };
      if(APRS_debug) debugA("----> send_log_msg: textPayload=[%s] - signalReport=[%s] - my_spot=[%s] - first_node_rep=[%s] - last_node_rep=[%s] \r", textPayload.c_str() , signalReport.c_str(), my_spot.c_str() ,  first_node_rep.c_str(), last_node_rep.c_str() );

      //      unsigned long distancemTo = (unsigned long)TinyGPSPlus::distanceBetween( mylat_norm, mylon_norm, homelat, homelon);  // distance to present  device position or static position
      //      unsigned long distancemTo = (unsigned long)TinyGPSPlus::distanceBetween( mylat_norm, mylon_norm, old_Lat, old_Lon);
      //      debugA("===== DELTA DISTANCE ====[%f,%f]===========> distancemTo = %lu\r", mylat_norm, mylon_norm, distancemTo);
      old_Lat = mylat_norm ; old_Lon = mylon_norm ;
      //      if( distancemTo < 3 ) return ;  // skip if too close to previous spot....
      };

   if(APRS_debug){
      debugAl("====> call    %s\r\n" , sender_call.c_str() );
      debugAl("====> path    %s\r\n" , path.c_str() );
      debugAl("====> pos     %s\r\n" , pos.c_str() );
      debugAl("====> mylat   %s\r\n" , mylat.c_str() );
      debugAl("====> mylon   %s\r\n" , mylon.c_str() );
      debugAl("====> comment %s\r\n" , com.c_str() );
      debugAl("====> mylat   %f\r\n",  mylat_norm );
      debugAl("====> mylon   %f\r\n",  mylon_norm );
      debugAl("====> fn_rep  [%s]\r\n",  first_node_rep.c_str() );
      debugAl("====> ln_rep  [%s]\r\n",  last_node_rep.c_str() );       
      };
           
   // we just accept only packets including a valid position 
   if(( mylat_norm == 0.0) || ( mylon_norm == 0.0)) {
      if(APRS_debug) debugA("====> Discarding packet after policy checks\r");
      return (false);
      };

   int CRC_errored_packets_APRS = (LoRa_CRC_errored_packets - LoRa_CRC_errored_packets_BCN);
   // OK finaly we was able to say that a new LoRa packet has been processed correctly... send log message to remote sarimesh server
   message = message + sender_call + "," + sender_call + ",,,," + sender_call + ",/#," + String( mylat_norm,7) + "," + String(mylon_norm,7) + ",,,,,,,,,," + com + signalReport + " CRC_err_APRS=" + String(CRC_errored_packets_APRS) ;
   // set display content and flag display event and sound buzzer 
   int DispIndex = first_node_rep.indexOf(' ');
   ui3_payload = "origin " + first_node_rep.substring(DispIndex +1) ;   // 20230507
   //  ui4_payload = first_node_rep.substring(DispIndex +1) ;    // 20230507
//   if((first_node_rep != last_node_rep) || ( first_node_rep.indexOf("FromInt") > 0 )){
   if((first_node_rep != last_node) || ( first_node_rep.indexOf("FromInt") > 0 )){

      DispIndex = last_node_rep.indexOf(' ');
      ui4_payload = last_node + " " + last_node_rep.substring(DispIndex +1)  ;  // 20230507
      ui5_payload = last_node + " " + last_node_rep.substring(DispIndex +1)  ;
      //   ui5_payload = last_node_rep ;
      }
   else {
      ui5_payload = "---------------" ;     
      ui4_payload = "direct" ;     

      };
   display_event = 1 ;
   soundBuzzer();
   lora_payload = textPayload;
     
   // set dashboard variables
   //  RX_Last_Spot_payload = textPayload.substring(0, textPayload.length() - 1); // 20211116 by MF
   RX_Last_Spot_payload = textPayload ;
   RX_Last_Spot_payload.replace("|"," ");
   // RX_Last_Spot_payload = RX_Last_Spot_payload.substring(0, RX_Last_Spot_payload.length() - 1);    // by MF 20220131
   RX_Last_Spot_level = signalReport; 
   if((first_node_rep != last_node_rep) || ( first_node_rep.indexOf("FromInt") > 0 )){
      RX_Last_Path_Report = first_node_rep + "<br>--> " + last_node_rep + "<br>---->" + signalReport ; 
      }
   else {
      RX_Last_Path_Report = first_node_rep + "<br>--> " + "   --  " + "<br>---->" + signalReport ; 
      };

   // send syslog message
   if(syslog_available) syslog.logf(LOG_INFO, "%s", message.c_str());
      // send new spot to remote sarimesh server
      message = message + " \r\n\r";
      message.toCharArray(Msg,message.length()); 
      sendUdpMsg(Msg);                                 
      // return (true);              
      // write new APRS/OE_Style spot to FRAM
      String my_local_spot;
      my_spot.replace(" (","");
      int my_spot_init = my_spot.indexOf(" ");  int my_spot_end = my_spot.lastIndexOf(" ");
      my_spot = my_spot.substring(my_spot_init + 1, my_spot_end - my_spot_init +1 );   
      //    my_spot_end = my_spot.lastIndexOf(" ");  my_spot = my_spot.substring(0 , my_spot_end + 1 ); 
      if(local_is_CRC_Errored) { my_local_spot = "1|" ; } else { my_local_spot = "0|" ; };           
         if((first_node_rep != last_node_rep) || ( first_node_rep.indexOf("FromInt") > 0 )){
            my_local_spot= my_local_spot + String( mylat_norm,5) + "|" + String(mylon_norm,5)+ "|" + String(sender_call) + "|" + String(my_spot) + "|" +  String(last_node);
            }
         else {
            my_local_spot= my_local_spot + String( mylat_norm,5) + "|" + String(mylon_norm,5)+ "|" + String(sender_call) + "|" + String(my_spot) + "|" +  "--------+++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++";
            };
      my_local_spot =  my_local_spot.substring(0, 52);  // clip any local spot to only the first 63 bytes
      write_spot_to_log( my_local_spot); 
      return (true);                
}


void DutyCycle_Monitor(void){
    if(millis() - activity_start >  2000) {
      uint32_t hang_time= millis() - activity_start ;
      syslog.logf(LOG_INFO, "activity_Monitor: system hang at %s - hang_time=%d - activity_time=%d - LastFunction=%s ",  (UTC.dateTime( ezt::now(), "d/n/Y H:i:s.v T")).c_str(), hang_time, activity_time , LastFunction.c_str());
      if(hang_time > 300000 ) {
       debugA("========== DutyCycle_Monitor - node Reboot ========>>>>>>>> hang_time=%d - activity_time=%d\r", hang_time , activity_time );        
       write_event_to_log("== system reboot by DutyCycle_Monitor(hang_time) =====");
       syslog.logf(LOG_INFO, "system reboot by DutyCycle_Monitor(hang_time treshold exceded); LastFunction=%s", LastFunction.c_str());
       debugA(">>>>>>>>>>> Now Rebooting... \r");
       vTaskDelay(pdMS_TO_TICKS(3000));
       if(reboot_pin){
          hard_reboot();  
          }
       else{
         ESP.restart();
         };         
        };
      };
    LoRa_DutyCycle = (4*LoRa_DutyCycle +  LoRa_OnAirTime_Tot / 600.0 )/5 ;  // LoRa duty cycle in %
    LoRa_OnAirTime_Tot = 0 ;
    currChanCong = currChanCong + LoRa_DutyCycle ;
    ChanCong = ( 4*ChanCong + currChanCong )/5;  // channel congestion as average of reported peers dutycycles
    nodeList = currNodeList ;  // last minute node list
    currNodeList = String(" ");        // current node list
    if(currChanCong > 1000.0) {
       debugA("========== DutyCycle_Monitor - node Reboot ========>>>>>>>> ChanCong=%5.2f - nodeList=%s\r", ChanCong , nodeList.c_str()  );        
       write_event_to_log("== system reboot by DutyCycle_Monitor =====");
       debugA(">>>>>>>>>>> Now Rebooting... \r");
       vTaskDelay(pdMS_TO_TICKS(3000));
          
       if(reboot_pin){
          hard_reboot();   // force reboot we are entering BT_KISS mode
          }
       else{
         ESP.restart();
         }; 
          
       }
   else{
 //      debugA("========== DutyCycle_Monitor ========>>>>>>>> ChanCong=%5.2f - nodeList=%s\r", ChanCong , nodeList.c_str()  );        
       };
   currChanCong = 0.0 ;       // current channel congestion while scanning network
}


void show_dashboard(void){
  char TEMP[32],TEMP1[32],TEMP2[32],TEMP3[32], TEMP4[32], TEMP5[32], TEMP6[32];
  sprintf(TEMP,"%d",LoRa_OnAirTime);String OnAirTime=TEMP;
  sprintf(TEMP1,"%5.2f",cpu_temp) ;String CpuTemp = TEMP1;  
  uint32_t uptime = esp_timer_get_time()/1000000 ;sprintf(TEMP2,"%d",uptime) ; String Up_Time = TEMP2;  
  sprintf(TEMP3,"%5.2f",ppm_runtime) ;String LoRa_FreqJitter = TEMP3; 
  sprintf(TEMP4,"%5.2f",LoRa_DutyCycle) ; String DutyCycle=TEMP4;
  sprintf(TEMP5,"%5.2f",ChanCong) ; String ChanCongStr=TEMP5;
  sprintf(TEMP6,"%5.2f",LoRa_ENL) ;String LoRa_ENLStr=TEMP6;
  String device_date=UTC.dateTime( ezt::now(), F("d/n/Y H:i:s T")) ;
  // gps.satellites.value(), gps.satellites.isValid()
  String gps_pos="Not_Available";
  String gps_sats="Not_Available";  
  gps_sats= gps.satellites.value();
  if(gps.satellites.isValid() && (gps.satellites.value() >= 2)){
     gps_pos=lat_now+","+lon_now;
     //   gps_sats= gps.satellites.value();
     };
     
  String device_type="Booting" ;
  if(ESP_Config.Tracker_Mode) {device_type = "Tracker Mode";};
  if(ESP_Config.iGate_Mode) {device_type = "iGate Mode";};
  if(ESP_Config.Admin_Mode) {device_type = "Admin Mode";};
  if(ESP_Config.Serial_KISS_Mode) {device_type = "Serial_KISS Mode";};
  if(ESP_Config.TCP_KISS_Mode) {device_type = "TCP_KISS Mode";};

  String op_status_clear="Not_synchronized";
  if(op_status == 1 ) {op_status_clear = "RTC_disciplined";};
  if(op_status == 2 ) {op_status_clear = "NTP_disciplined";};
  if(op_status == 3 ) {op_status_clear = "GPS_disciplined";};
  
   debugAl("================== Dashboard start ==========================\r\n");
   
   debugAl("   SW_version                = %s\r\n", SW_Revision.c_str());   
   debugAl("   Device Date/Time          = %s\r\n", device_date.c_str());
   debugAl("   Operation Status          = %s\r\n", op_status_clear.c_str());
   debugAl("   Clock Jitter(msec)        = %s\r\n", (String(time_jitter)).c_str());
   debugAl("   GPS Fix                   = %s\r\n", gps_pos.c_str());
   debugAl("   GPS Sats                  = %s\r\n", gps_sats.c_str());
   debugAl("   Last_RX_Pkt_Report        = %s\r\n", RX_Last_Spot_level.c_str());
   debugAl("   Last_RX_Pkt_payload       = %s\r\n", RX_Last_Spot_payload.c_str());
   debugAl("   Last_RX_Path              = %s\r\n", RX_Last_Path_Report.c_str());

   debugAl("   LoRa_rx_packets           = %d\r\n", LoRa_rx_packets);
   debugAl("   LoRa_tx_packets           = %d\r\n", LoRa_tx_packets);
   debugAl("   LoRa_rx_AX25_packets      = %d\r\n", LoRa_rx_AX25_packets);
   debugAl("   LoRa_tx_AX25_packets      = %d\r\n", LoRa_tx_AX25_packets);   
   debugAl("   LoRa_rx_OEStyle_packets   = %d\r\n", LoRa_rx_OEStyle_packets);
   debugAl("   LoRa_tx_OEStyle_packets   = %d\r\n", LoRa_tx_OEStyle_packets); 
   debugAl("   LoRa_rx_native_packets    = %d\r\n", LoRa_rx_native_packets);
   debugAl("   LoRa_tx_native_packets    = %d\r\n", LoRa_tx_native_packets);  
   
   debugAl("   LoRa_lost_packets         = %d\r\n", LoRa_lost_packets);
   debugAl("   LoRa_CRC_errored_packets  = %d/%d\r\n", LoRa_CRC_errored_packets, LoRa_CRC_errored_packets_BCN);
   debugAl("   LoRa_UMN_errored_packets  = %d\r\n", LoRa_UMN_errored_packets);
   debugAl("   LoRa_CAD_errors           = %d\r\n", LoRa_CAD_errors);
   debugAl("   LoRa_ReSched_packets      = %d\r\n", LoRa_ReSched_packets);

 if(iGate_Mode){
   debugAl("   AprsIS_rx_packets         = %d\r\n", AprsIS_rx_packets);
   debugAl("   AprsIS_tx_packets         = %d\r\n", AprsIS_tx_packets);
   debugAl("   AprsIS_dropped_packets    = %d\r\n", AprsIS_dropped_packets);
   debugAl("   AprsIS_relayed_packets    = %d\r\n", AprsIS_relayed_packets);  
   };
   debugAl("   IPC_lost_msgs             = %d\r\n", IPC_lost_msgs);
   debugAl("   LoRa_OnAirTime(msec)      = %s\r\n", OnAirTime.c_str());
   debugAl("   LoRa_DutyCycle(percentage = %s\r\n", DutyCycle.c_str());
   debugAl("   LoRa_ChanCong(percentage) = %s\r\n", ChanCongStr.c_str());
   debugAl("   LoRa_ENL(dbm)             = %s\r\n", LoRa_ENLStr.c_str());
   debugAl("   LoRa_FreqJitter(ppm)      = %s\r\n", LoRa_FreqJitter.c_str());
   debugAl("   CPU Temperature(C°)       = %s\r\n", CpuTemp.c_str());
   debugAl("   Processor_UpTime(secs)    = %s\r\n", Up_Time.c_str());
   debugAl("================== Dashboard end ==========================\r\n");
}


void show_stats(void){
   debugAl("================== Stats start ==========================\r\n");
   debugAl("   SW_version                = %s\r\n", SW_Revision.c_str());
   debugAl("   LoRa_rx_packets           = %d\r\n", LoRa_rx_packets);
   debugAl("   LoRa_tx_packets           = %d\r\n", LoRa_tx_packets);
   debugAl("   LoRa_rx_AX25_packets      = %d\r\n", LoRa_rx_AX25_packets);
   debugAl("   LoRa_tx_AX25_packets      = %d\r\n", LoRa_tx_AX25_packets);   
   debugAl("   LoRa_lost_packets         = %d\r\n", LoRa_lost_packets);
   debugAl("   LoRa_CRC_errored_packets  = %d/%d\r\n", LoRa_CRC_errored_packets, LoRa_CRC_errored_packets_BCN);

   if(iGate_Mode){
      debugAl("   AprsIS_rx_packets      = %d\r\n", AprsIS_rx_packets);
      debugAl("   AprsIS_tx_packets      = %d\r\n", AprsIS_tx_packets);
      debugAl("   AprsIS_dropped_packets = %d\r\n", AprsIS_dropped_packets);
      };
   debugAl("   LoRa_OnAirTime(msec)   = %d\r\n", LoRa_OnAirTime);
   debugAl("   CPU Temperature(C°)    = %5.2f\r\n", cpu_temp);
   debugAl("   Processor_UpTime(secs) = %d\r\n", esp_timer_get_time()/1000000);
   debugAl("================== Stats end ==========================\r\n");
}

