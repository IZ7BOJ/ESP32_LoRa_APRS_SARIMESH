// loraprs_service.cpp
#include "loraprs_service.h"

// start of namespace LoraPrs
namespace LoraPrs {

#ifndef BLUETOOTH_DISABLED

Service::Service() 
  : serialBt_()
  , kissState_(KissState::Void)
  , kissCmd_(KissCmd::NoCmd)
{
}

#else

Service::Service() 
  : aSerial()
  , kissState_(KissState::Void)
  , kissCmd_(KissCmd::NoCmd)
{
}
#endif


// this function will setup tha LORA_APRS application based on the cfg structure passed as input (conf)
bool Service::setup(const Config &conf){
   LastFunction = __func__ ;
   previousBeaconMs_ = 0;
   // config
   isClient_ = conf.IsClientMode;
   loraFreq_ = conf.LoraFreq;
   ownCallsign_ = AX25::Callsign(conf.AprsLogin);
   if (!ownCallsign_.IsValid()) {
      // if(APRS_debug) 
      debugA("\n\r\n\rWARNING: LoRa Service Setup: Own callsign (for aprs.fi access) is not valid- aborting setup\r");
      return(false);
      };
      
   // create APRS-IS credentials login string
   aprsLogin_ = String("user ") + conf.AprsLogin + String(" pass ") + conf.AprsPass + String(" vers ") + CfgLoraprsVersion;
   if (conf.AprsFilter.length() > 0) { aprsLogin_ += String(" filter ") + conf.AprsFilter; } ;
   aprsLogin_ += String("\n");
   aprsHost_ = conf.AprsHost;
   aprsPort_ = conf.AprsPort;
   aprsBeacon_ = conf.AprsRawBeacon;
   aprsBeaconPeriodSeconds_ = conf.AprsRawBeaconPeriodSeconds;
   aprsBeaconPeriodmSecs = conf.AprsRawBeaconPeriodSeconds * 1000;

   aprsShortBeaconPeriodSeconds = conf.AprsRawBeaconPeriodSeconds;
   autoCorrectFreq_ = conf.EnableAutoFreqCorrection;
   addSignalReport_ = conf.EnableSignalReport;
   persistentConn_ = conf.EnablePersistentAprsConnection;
   enableRfToIs_ = conf.EnableRfToIs;
   enableIsToRf_ = conf.EnableIsToRf;
   enableRepeater_ = conf.EnableRepeater;
   enableBeacon_ = conf.EnableBeacon;

   // setup LoRa device to use 
   // if(LoRa_debug) 
   debugA("Service::setup: LoRa Radiolib init and LoRa device setup...\r");
   bool ret = SetLoraConfig(conf.LoraFreq, conf.LoraBw, conf.LoraSf, conf.LoraCodingRate, conf.LoraPower, conf.LoraSync, conf.LoraPreambleLen, "");
   if( !ret){
      // if(LoRa_debug) 
      debugA("LoRa init failed: possible HW mismatch - aborting setup \r");
      return(false);
      };
   // if(LoRa_debug) 
   debugA("LoRa Service Setup: needsBt=%d - TCP_KISS_Mode=%d - BT_KISS_Mode=%d - Serial_KISS_Mode=%d\r",needsBt(), TCP_KISS_Mode, BT_KISS_Mode , BT_KISS_Mode, Serial_KISS_Mode );
   if (needsBt() || Serial_KISS_Mode || TCP_KISS_Mode) {
      setupBt(conf.BtName);  // this is for KISS mode operation
      };
   if (needsAprsis() && persistentConn_) {
      // if(LoRa_debug) 
      debugA("LoRa Service Setup: starting APRS-IS connection...\r");
      reconnectAprsis();
      };
   // PrintLoraPrsConfig_private(conf);
   debugA("LoRa Service Setup: Successfully completed !\r");
   return(true);
}

// Following routine try to manage wifi upstream dynamics... if upstream is available no problems... go on
// If wifi upstream is not available try to reconnect... if after few trial it appears that upstream do not
// connect just stop trying for next 200 trials... then try again just in case wifi reappears..
//
void Service::reconnectWifi(){  
   LastFunction = __func__ ;
   NetworkUpstreamMonitor();
}

bool Service::reconnectAprsis() {
   LastFunction = __func__ ;
#ifndef APRS_IS_DISABLE
   aprsis_available = false; 
   // if(APRS_debug)
   debugA("reconnectAprsis: APRSIS connecting...");
   int cntr = 5 ;
   while ( cntr-- > 0){
      display_event = 9 ;   // send message on local display
      if (aprs_client.connect(aprsHost_.c_str(), aprsPort_)) {
         // if(APRS_debug)
         debugA("ok\n");
         aprs_client.print(aprsLogin_);
         aprsis_available = true ;
         debugA("reconnectAprsis: success !! \r" ); 
         return true;
         };
      vTaskDelay(pdMS_TO_TICKS(100));    
      };
   debugA("reconnectAprsis: Failed to connect to %s:%d...  trying to recovery\n" , aprsHost_.c_str() , aprsPort_);

#ifndef WIFI_UPSTREAM
   Ethernet.maintain();
#endif
   return false; 
#endif  
}


bool Service::SetLoraConfig(float loraFreq, float bw, byte sf, byte cr, int pwr, byte sync, byte prlen , String lora_mod_type){
   LastFunction = __func__ ;
   if(LoRa_debug)  debugA("Service::SetLoraConfig: starting SetLoraConfig....\r");
   while(! LoRa_initialized ){  // wait for LDH initialization complete...
      if(LoRa_debug) debugA("Service::SetLoraConfig: waiting for LoRa_initialized ..%d..\r", LoRa_initialized );
      vTaskDelay(pdMS_TO_TICKS(500));
      };
   /*
   if ( ! LDH_SetLoraConfig( loraFreq,  bw,  sf,  cr,  pwr,  sync,  prlen ,  lora_mod_type)){
      if(LoRa_debug) debugA("\rService::SetLoraConfig: LDH_SetLoraConfig failed LoRa device setup....\r");
      return(false);
      };
   */

   static BeaconRunTimeConfig BCN_APRS_cfg_local  ;
   BCN_APRS_cfg_local = { 0, loraFreq, bw, sf, cr, sync, pwr, 0, prlen } ; 
   //  SetLoraConfig(BCN_APRS_cfg_local.LoraFreq, BCN_APRS_cfg_local.LoraBw,BCN_APRS_cfg_local.LoraSf, BCN_APRS_cfg_local.LoraCodingRate, BCN_APRS_cfg_local.LoraPower, BCN_APRS_cfg_local.LoraSync, BCN_APRS_cfg_local.LoraPreambleLen, "" );
   float LoraFreq_nominal =  BCN_APRS_cfg_local.LoraFreq * ( 1.0 + (float)(BCN_APRS_cfg_local.LoraFreqCorr)/10000000.0 );   
   if ( ! LDH_SetLoraConfig(LoraFreq_nominal, BCN_APRS_cfg_local.LoraBw,BCN_APRS_cfg_local.LoraSf, BCN_APRS_cfg_local.LoraCodingRate, BCN_APRS_cfg_local.LoraPower, BCN_APRS_cfg_local.LoraSync, BCN_APRS_cfg_local.LoraPreambleLen, "" )){
      if(LoRa_debug)  debugA("\rService::SetLoraConfig: LDH_SetLoraConfig failed LoRa device setup....\r");
      return(false);
      };
   BCN_APRS_cfg = BCN_APRS_cfg_local;   // save BCN_APRS_cfg to be able to restart LoRa APRS app dinamically
   BCN_LDH_cfg = BCN_APRS_cfg ;
   if(LoRa_debug)  debugA("\rService::SetLoraConfig: LDH_SetLoraConfig: LoRa device setup successfull: using BCN_APRS_cfg={%d, %8.5f, %8.5f, %d, 0x%02X, %d, %d, %d}!!\r", 0, loraFreq, bw,   sf, cr, sync, pwr, 0, prlen );
   return(true);       
}   // end of Service::SetLoraConfig


void Service::setupBt(const String &btName){
      // assigns a stream to handle serial communication in KISS modes
   if( !BT_KISS_Mode && Serial_KISS_Mode && !TCP_KISS_Mode) {
      if(APRS_debug) debugA("setupBt: using HW Serial interface ...");
      aSerial = &Esp32Serial1 ;  //  
      }
#ifndef BLUETOOTH_DISABLED          // to completely remove any bluetooth support
   else if( BT_KISS_Mode && !Serial_KISS_Mode && !TCP_KISS_Mode ){
      if(APRS_debug) debugA("setupBt: BT initialization  btName = " + btName + "...");
      if (serialBt_.begin(btName)) {
         if(APRS_debug) debugA("successfull !\r");
         aSerial = &serialBt_ ;
         }
      else{
         if(APRS_debug) debugA("failed \r");
         };
      }
#endif     
   else if( ! BT_KISS_Mode && !Serial_KISS_Mode && TCP_KISS_Mode ){  // Kiss_Over_TCP mode
      if(APRS_debug) debugA("setupBt: init TCP Kiss_server...  ");
      Kiss_server.begin();
      Kiss_client = Kiss_server.available(); 
      if(APRS_debug) debugA("successfull !\r");             
      };

   if(APRS_debug) debugA("setupBt: complete \r");
} // end of Service::setupBt


void Service::aprs_loop() {     // main LoRa APRS service loop
   long r = random(0, 255);
   LastFunction = __func__ ;
   if(! APRS_Service_initialized) return ;  // APRS service is not jet initialized or in standby mode.. just wait...

#ifdef WIFI_UPSTREAM
   if (needsWifi() && WiFi.status() != WL_CONNECTED) {
      // upstream network connection monitored by NetworkUpstreamMonitor.. so go on
      // reconnectWifi();
      };
     
   if (needsAprsis() && !aprs_client.connected() && persistentConn_) {
      aprsis_available = false ;  // upstream APRS_IS connection monitored by AprsMonitor so go on
      // reconnectAprsis();
      };

#endif

   //*******************************************
   // show parking queue status
   int messagesWaiting = uxQueueMessagesWaiting(xQueue_park);
   int emptySpaces = uxQueueSpacesAvailable(xQueue_park);
   // Receive a message from the parking queue.  Block for 10 ticks if a message is not immediately available.
   LDH_Message_v2 LDM_Msg_v2 ;
   if (xQueueReceive(xQueue_park, (void *)&LDM_Msg_v2, ( TickType_t ) 10 ) == pdTRUE) {  // wait  10 secs
      // show parking queue status
      // if(LoRa_debug)  debugA("aprs_loop: ====>>>>>>  IPC messagesWaiting=%d - emptySpaces=%d\r", messagesWaiting, emptySpaces);
      if(LDM_Msg_v2.MsgType == 3 ){   // packet transmission request auto CAD
         // debugA("aprs_loop-MSG_IN: LoRa packet transmission request received via IPP MsgSqNbr =%d - dueTime=%ld .... \r", LDM_Msg_v2.MsgSqNbr , LDM_Msg_v2.dueTime  ); 
         if(((LDM_Msg_v2.dueTime == 0.0 ) || (millis() > LDM_Msg_v2.dueTime))  ){
            if(APRS_debug) debugA("aprs_loop-MSG_PARK: EXEC LoRa packet transmission MsgSqNbr =%d - dueTime=%ld .... \r", LDM_Msg_v2.MsgSqNbr , LDM_Msg_v2.dueTime  ); 
            LDH_sendNativeToLora(LDM_Msg_v2.bufData , LDM_Msg_v2.bufLen , (uint8_t)LDM_Msg_v2.SigRep[0] , false , false);  // this calls directly the LDH send function          
            }
         else {
            // packet scheduled for later time transmission...  re-enqueue transmission request
            // LDM_Msg_v2.dueTime = millis() + 5000 ;  // delay retrasmission by 5000 msec
            if( xQueueSend( xQueue_park, (void *)&LDM_Msg_v2, ( TickType_t ) 10 ) != pdPASS ) {
               if(LoRa_debug) debugA("=====QUEUE_PARK===> aprs_loop: IPC TX request re-enqueueing failed: MsgSqNbr=%d....\r", LDM_Msg_v2.MsgSqNbr );
                  // flag lost packet due to IPC overflow
                  IPC_lost_msgs++ ;
                  } 
               else {
                  // if(LoRa_debug) debugA("=====QUEUE_PARK===> aprs_loop: IPC TX request re-enqueueing success: MsgSqNbr=%d....\r", LDM_Msg_v2.MsgSqNbr );
                 };           
            };
         }    // end of if(LDM_Msg_v2.MsgType == 3 )
      else if(LDM_Msg_v2.MsgType == 4 ){   // packet transmission request no CAD
         if(((LDM_Msg_v2.dueTime == 0.0 ) || (millis() > LDM_Msg_v2.dueTime))  ){
            if(APRS_debug) debugA("aprs_loop-MSG_IN: EXEC LoRa packet transmission MsgSqNbr =%d - dueTime=%ld .... \r", LDM_Msg_v2.MsgSqNbr , LDM_Msg_v2.dueTime  ); 
            LDH_sendNativeToLora_noCAD(LDM_Msg_v2.bufData , LDM_Msg_v2.bufLen , (uint8_t)LDM_Msg_v2.SigRep[0] , false, false  );  // this calls directly the LDH send function          
            }
         else {
            // packet scheduled for later time transmission...  re-enqueue transmission request
            if( xQueueSend( xQueue_park, (void *)&LDM_Msg_v2, ( TickType_t ) 10 ) != pdPASS ) {
               if(LoRa_debug) debugA("=====QUEUE_PARK===> aprs_loop: IPC TX request re-enqueueing failed: MsgSqNbr=%d....\r", LDM_Msg_v2.MsgSqNbr );
                  // flag lost packet due to IPC overflow
                  IPC_lost_msgs++ ;
                  } 
               else {
                  //  if(LoRa_debug) debugA("=====QUEUE_PARK===> aprs_loop: IPC TX request re-enqueueing success: MsgSqNbr=%d....\r", LDM_Msg_v2.MsgSqNbr );
                 };                     
               };
            }   // end of if(LDM_Msg_v2.MsgType == 4 )              
      };

   //*******************************************
   // Receive a message from the APRS queue.  Block for 10 ticks if a message is not immediately available.
   LDH_Message_v2 LDM_Msg_RX_v2 ;
   if (xQueueReceive(xQueue_APRS, (void *)&LDM_Msg_RX_v2, ( TickType_t ) 10 ) == pdTRUE) {  // wait  10 secs
      // debugA("aprs_loop-MSG_RX-2: SigRep=[%s] - bufData=[%s] - bufLen=%d\r",LDM_Msg_RX_v2.SigRep , LDM_Msg_RX_v2.bufData ,LDM_Msg_RX_v2.bufLen  );
      //  debugA("aprs_loop-MSG_RX-2: ack=%d\r", LDM_Msg_RX_v2.ack);
      if(LDM_Msg_RX_v2.ack == 1) {
         //  debugA("aprs_loop-MSG_RX-5: ack=%d ==> New LoRa Packet received\r", LDM_Msg_RX_v2.ack);
         onLoraDataAvailable(LDM_Msg_RX_v2.bufData, LDM_Msg_RX_v2.bufLen, LDM_Msg_RX_v2.SigRep, LDM_Msg_RX_v2.SigRep);   // process new LoRa packet received
         } 
      else {
         //  debugA("aprs_loop-MSG_RX-5: ack=%d ==> invalid data received\r", LDM_Msg_RX_v2.ack);
         };
      };
   //==========================================
   //   debugA("Starting TX path");
   if((BT_KISS_Mode || Serial_KISS_Mode) && !TCP_KISS_Mode){
      if (aSerial->available()) { onBtDataAvailable(); } ;  // check if a new KISS packet is available for transmission
      };
   if( !BT_KISS_Mode && !Serial_KISS_Mode && TCP_KISS_Mode){   // KISS_Over_TCP
      if(Kiss_client) {
         if(Kiss_client.connected()) { onBtDataAvailable(); }; // check if a new KISS packet is available for transmission
         }
      else {    
         Kiss_client = Kiss_server.available();   // try listening for incoming clients
         } ;      
      }  // KISS_Over_TCP
#ifndef APRS_IS_DISABLE        
   else if (aprs_client.available() > 0) {
      // debugA("Starting onAprsisDataAvailable");
      onAprsisDataAvailable();
      }
#endif       
   else if( (needsBeacon()|| requirePeriodicBeacon ) && ( (EncapType == 1) ||  (EncapType == 2)  )) {
      // debugA("Starting sendPeriodicBeacon requirePeriodicBeacon = %d\r", requirePeriodicBeacon); 
//      if( AgileBeaconing == 0) {        // check if Agilebeaconing has to be performed
         sendPeriodicBeacon();          // Agile is not enable so do standard beaconing
//         };
      }
   else if( (needsBeacon()|| requirePeriodicShortBeacon ) && (EncapType == 0 )) {
      //     debugA("Starting sendPeriodicShortBeacon requirePeriodicShortBeacon = %d\r", requirePeriodicShortBeacon); 
      sendPeriodicShortBeacon();
      };      

   // modified by MF 20201123  not needed in our context   
   //  vTaskDelay(pdMS_TO_TICKS(CfgPollDelayMs));
   vTaskDelay(pdMS_TO_TICKS(50));
   // debugA("end of LoRaaprs loop");
}  // end of Service::aprs_loop



void Service::sendPeriodicBeacon(){
   LastFunction = __func__ ;
   if( ! ( gps_fix_available || gps_fix_static ) ) { 
      if(gps_debug) debugA("sendPeriodicBeacon: skipping due to gps fix not jet available\r");          
      return ;  // nothing to do if gps fix is not jet available
      };

   if( ! LoRa_initialized ) { 
      if(LoRa_debug) debugA("sendPeriodicBeacon: skipping due to lora not jet initialized \r");          
      return ;  // nothing to do if lora is not jet initialized
      };

   long currentMs = millis();  long deltaMs_ = currentMs - previousBeaconMs_ ;
   if( AgileBeaconing != 0) { previousBeaconMs_ = currentMs; }; // check if AgileBeaconing is active and disable standard beacon...
   if (previousBeaconMs_ == 0 || currentMs - previousBeaconMs_ >= aprsBeaconPeriodSeconds_ * 1000 || requirePeriodicBeacon ) {   
      if( LDH_RX_Mode == 1 ) return ; 
      requirePeriodicBeacon = false;
      if( !(xSemaphoreTake(LocationManager_mutex_v, ( TickType_t ) 2) )){ debugA("xSemaphoreTake FAILED: aborting sendPeriodicBeacon"); return;  };
         aprsBeacon_ = aprsBeacon_proto;
         if(EncapType == 2){
            aprsBeacon_.replace("LoRa", "LoRa-OE"); 
            // aprsBeacon_.replace("APLS01", "APLT00");
            };
         aprsBeacon_.replace("GPS_LAT", (char *)&slat);  // LocationManager guarantes that this data are always available and valid for a spot
         aprsBeacon_.replace("GPS_LON", (char *)&slon);
         APRS_MsgSN++; if ( APRS_MsgSN >= 99) APRS_MsgSN = 0;  // increment APRS beacon sequence number
         aprsBeacon_.replace("BLK_TAG", String(APRS_BlkTag) );
         aprsBeacon_.replace("SQN_NBR", String(APRS_MsgSN) );
       
         if(APRS_MsgSN == 5 ) {
           aprsBeacon_.replace("APRS_WC", APRS_WCtag.c_str() );
           } 
         else {
           aprsBeacon_.replace("APRS_WC", "" );
           } ;

         xSemaphoreGive(LocationManager_mutex_v);  // release mutex 

         // create beacon payload based on device configuration
         AX25::Payload payload(aprsBeacon_);
         String signalReport ="";
         if(EncapType == 1){
            signalReport = String(" (") +
//            String(LoRa_DutyCycle)+
//            String(" ") +
            String(NodeName.c_str()) + 
//            String(" Beacon ") +
//            String(" ") + 
//            String(cpu_temp) +
            String(")") ;
            }
         else if(EncapType == 2){
            signalReport = String(" (") +
//            String(LoRa_DutyCycle)+       // REMOVED 20230507 BY i8fuc 
//            String(" ") +
            String(NodeName.c_str()) + 
//            String(" Beacon_OE ") +        // removed 20230507 by I8FUC
//            String(" ") +
//            String(cpu_temp)+
            String(")") ;   
            };  

         if( PayloadStyle == 1){
            payload.fromString(payload.ToString());   // add our passtag to the packet to be relayed or transferred to aprs-is
            }
         else{
            payload.fromString(payload.ToString() + signalReport);   // add our passtag to the packet to be relayed or transferred to aprs-is
            };
            
         if (payload.IsValid()) {
#ifndef APRS_IS_DISABLE        
            if (enableRfToIs_) {
               if(APRS_debug) debugA("To_APRS-IS (420) ==> [%s]\n",(payload.ToString()).c_str());       
               sendToAprsis(payload.ToString());
               };
#endif          
            if(APRS_debug) {  // send message to console and syslog for debugging purposes
               debugA("To_LoRa (420) ==> [%s]\n",(payload.ToString()).c_str());
               if(syslog_available) syslog.logf(LOG_INFO, "sendPeriodicBeacon: To_LoRa (420) ==> [%s] ", (payload.ToString()).c_str() );
               };
            display_event = 6 ;   // display event on local display  
            // 20230515 by MF removed the random delay to better cope with AgileBeaconing
//            bool ret = sendToLora(payload, true , true );   // send packet with random  delay and skip if needed was as soon as possible 20220214 by MF
            bool ret = sendToLora(payload, false , true );   // send packet without random  delay and skip if needed was as soon as possible 20220214 by MF

            if(APRS_debug && ret)debugA("--> sendPeriodicBeacon: Periodic beacon sent\n");
            }
         else {
            if(APRS_debug)debugA("--> sendPeriodicBeacon: Beacon payload is invalid\n");
            display_event = 16 ;   // display event on local display    
            }
         previousBeaconMs_ = currentMs - random(0, 2000);
      };
}  // end of Service::sendPeriodicBeacon


void Service::sendToAprsis(String aprsMessage) {
   LastFunction = __func__ ;

#ifdef WIFI_UPSTREAM  
   if (needsWifi() && WiFi.status() != WL_CONNECTED) {
      // reconnectWifi();
      debugA("To_LoRa (421) ==> [message dropped due to UpstreamNetwork NOT available]\r");
      return ;      
      };
#endif

   if (needsAprsis() && !aprs_client.connected()) {
      aprsis_available = false ; 
      // reconnectAprsis();
      debugA("To_LoRa (422) ==> [message dropped due to aprsis NOT available]\r");
      return ; 
      };

   aprs_client.println(aprsMessage);
   AprsIS_tx_packets++;
   if(APRS_debug) debugA("To_APRS-IS --> %s\r", aprsMessage.c_str()); 
   if (!persistentConn_) {
      aprs_client.stop();
      } ;
} // end of Service::sendToAprsis


void Service::onAprsisDataAvailable() {
   String aprsisData;
   LastFunction = __func__ ;

   //  if(APRS_debug) debugA("onAprsisDataAvailable: ===> Starting onAprsisDataAvailable\r");
   //  if(APRS_debug) debugA("From_APRS-IS  <-- ");
   uint16_t crt=0; bool skip_packt = false ;
   while (aprs_client.available() > 0) { // read in aprsis messsage
      char c = aprs_client.read();
      if (c == '\r') continue;
      if (c == '%') c = ' ';
      if (c == '`') c = ' ';
      if (c == '&') c = ' ';
      if(crt++ > 150) { // cut too long packets from internet!!! we cannot effort to relay them !
         skip_packt = true ; 
         break;    //  added by MF  20220312
         };
      // if(APRS_debug) debugA("%c",c);
      if (c == '\n') break;
      aprsisData += c;
      };

   if (aprsisData.indexOf("# ") == 0 ) {  
      // if(APRS_debug) debugA("Skipping heartbeet packet from internet (packet size = %d - payload=%s)...\n\r", crt, aprsisData.c_str()); 
      return ;
      };
   AprsIS_rx_packets++;   // report total aprs-is packets received....
   if(skip_packt){ 
      debugA("Skipping too long packet from internet (packet size = %d)!!!\n\r", crt); 
      AprsIS_dropped_packets++ ; 
      return ; 
      };   // cut too long packets from internet!!! we cannot effort to relay them !
     
   aprsisData.replace(")"," "); aprsisData.replace("("," ");  // sanify data from internet
   if (enableIsToRf_ && aprsisData.length() > 0) {
      AX25::Payload payload(aprsisData);
      if (payload.IsValid()) {
         if( (aprsisData.indexOf("LoRa") > 0 ) || ( aprsisData.indexOf("qAS") > 0 ) || ( aprsisData.indexOf("qAC") > 0 ) ) {
            if(APRS_debug) debugA("--> IsAprsRelayedLora: skipping APRS-IS relayed payload or LoRa repeated packets %s\r",aprsisData.c_str());
            AprsIS_dropped_packets++ ;
            return;
            };
         if(APRS_debug) debugA("From_APRS-IS  <-- %s \r", aprsisData.c_str());  
         String signalReport = String(" (") +
         String(LoRa_DutyCycle)+
         String(" ") +      
         String(NodeName.c_str()) + 
         String(" ") +
         String(" FromInt to LoRa ") +
         String(" ") +
         String(cpu_temp)+
         String(")") ;
         if( PayloadStyle == 1){
            payload.fromString(payload.ToString());   // add our passtag to the packet to be relayed or transferred to aprs-is
            }
         else{
            payload.fromString(payload.ToString() + signalReport);   // add our passtag to the packet to be relayed or transferred to aprs-is
            };    
         bool pol_checks = send_log_msg(payload.ToString(), signalReport, "Aprsis");   // check received payload and decide if send to spot log server/ Lora or discard packet
         if ( ! pol_checks) { 
            AprsIS_dropped_packets++ ;
            // debugA("==> pol_checks failed: dropping packet to lora\n");         
            return ;           // nothing to do due to packet discaded
            };  
         if( (payload.ToString()).indexOf("FromInt") > 0  ) { 
            String aprsis_packet = payload.ToString() ;   
            int callIndex = aprsis_packet.indexOf('>');
            int infoIndex = aprsis_packet.indexOf(':');
            String sender_call = aprsis_packet.substring(0, callIndex );
            String path = aprsis_packet.substring(callIndex +1 , infoIndex);
            String info = aprsis_packet.substring(infoIndex + 1);
            aprsis_packet = sender_call + ">" + "APZMDM,WIDE1-1" + ":" + info ;  // force LoRa network path to avoid extended looping
            if(APRS_debug) debugA("\r====path_translate=====>  original= [%s] - translated= [%s]\r",payload.ToString().c_str(),aprsis_packet.c_str()) ;
            payload.fromString(aprsis_packet);
            };
         bool ret;
         if(Beacon_Mode ){      
            ret = sendToLora(payload, false , true );   // send message to loRa without random delay in Beacon Mode
            }
         else{
            ret = sendToLora(payload, true , true );   // send message to loRa with random delay      
            };
         if (ret) { 
            AprsIS_relayed_packets++ ;
            if(APRS_debug) debugA("To_LoRa (407) ==> [%s]\n", (payload.ToString()).c_str());         
            } 
         else {
            //  if(APRS_debug)debugA("onAprsisDataAvailable: unable tra transmit packet to LoRa\n");
            AprsIS_dropped_packets++ ;
            };     
         }
      else {
         //  if(APRS_debug)debugA("onAprsisDataAvailable: Invalid payload from APRSIS\n");
         AprsIS_dropped_packets++ ;
         };
      };
}  // end of  Service::onAprsisDataAvailable

//1234

bool Service::sendToLora(const AX25::Payload &payload, bool rndDelay, bool SkipIfNeeded ) {
   LastFunction = __func__ ;
   byte LoRa_buf[1024];
   int bytesWritten;
  
   if(APRS_debug)  debugA("====> sendToLora (2023-in)  ==> [%s]\n",(payload.ToString()).c_str()); 
//   payload.fromString(payload.ToString() + signalReport);   // add our passtag to the packet to be relayed or transferred to aprs-is

   String aprsis_packet = payload.ToString() ;   
   String cmessage = APRS_Compress(aprsis_packet) ;
   AX25::Payload cpayload(cmessage);

//   payload.fromString(aprsis_packet);

   if(APRS_debug)  debugA("====> sendToLora (2023-out) ==> [%s]\n",(cpayload.ToString()).c_str()); 

   if(EncapType == 1 ){   // AX25 packet encapsulation
      bytesWritten = cpayload.ToBinary(LoRa_buf, sizeof(LoRa_buf));
      if(APRS_debug) debugA("\r  ---->  sendToLora_AX25: bytesWritten = %d - EncapType = %d\r", bytesWritten, EncapType); 
      } 
   else if(EncapType == 2 ){ // OE_Style packet encapsulation
      sprintf((char *)LoRa_buf,"%c%c%c%s",'<',0xFF, 0x01, (cpayload.ToString()).c_str());
      bytesWritten = (cpayload.ToString()).length() + 3;
      LoRa_buf[bytesWritten] = '\0';
      if(APRS_debug) debugA("---->  sendToLora_OE: bytesWritten = %d - EncapType = %d ", bytesWritten, EncapType);     
      };
   if (bytesWritten <= 0) {
      if(APRS_debug)debugA("sendToLora: Failed to serialize payload\n");
      return false;
      };
   if(APRS_debug){int i=0; while(i < bytesWritten ){if(! (i%16))debugAl("\n\r")  ; debugAl("%02X ", LoRa_buf[i]); i++; }; debugAl("\n\r"); };
   // if( LDH_RX_Mode == 1) rndDelay = true ;
   if(rndDelay){
      LDH_Ran_sendNativeToLora((uint8_t * )LoRa_buf , bytesWritten, EncapType, false, SkipIfNeeded);  // this calls directly the LDH send function 
      } 
   else {
      LDH_sendNativeToLora((uint8_t * )LoRa_buf , bytesWritten, EncapType, false, SkipIfNeeded );  // this calls directly the LDH send function 
      };
   //  sendNativeToLora((uint8_t * )LoRa_buf , bytesWritten, EncapType);  // this calls LDH send function via IPC
   return true ;
}; // end of Service::sendToLora


void Service::kissPDUcheckRX(uint8_t *rxBuf , int rxBufIndex) {
   if(APRS_debug){ int i=0; while(i < rxBufIndex ){if(! (i%16))debugAl("\n\r")  ; debugAl("%02X ", rxBuf[i]); i++;   }; debugAl("\n\r");};       
   AX25::Payload payload(rxBuf, rxBufIndex);  // assume to be an AX.25 payload and check for validity     
   if (payload.IsValid()) {  //  APRS_AX25_Style LoRa packet
      if(APRS_debug) debugA("----FROM_KISS_TO_LORA--->  valid AX25 payload received from KISS Host=[%s]\r",payload.ToString().c_str() );
      // it is a valid APRS payload;  add received rssi and snr values
      String textPayload = payload.ToString();
      // if(APRS_debug) debugA("-------------------------------------------------------> From_LoRa     <== textPayload=[%s]   signalReport=[%s] \r\n",textPayload.c_str(), signalReport.c_str());
      // write to FRAM log the sent LoRa packet and send spot message to sarimesh server
      // send_log_msg(textPayload, signalReport, my_spot);   // send to spot log server tag 1
      }  // end of payload.IsValid
   else {
      rxBuf[rxBufIndex]='\0';
      if(APRS_debug) debugA("----FROM_KISS_TO_LORA--->  FAKE AX25 payload received from KISS Host=[%s]\r", rxBuf);  
      };
   /*
   //  adapt PDU Encapsulation to adapt eventually to OE_Style
   if(EncapType == 2 ){ // OE_Style packet encapsulation mode
      byte LoRa_buf[1024];
      int bytesWritten;    
      sprintf((char *)LoRa_buf,"%c%c%c%s",'<',0xFF, 0x01, rxBuf);
      bytesWritten = rxBufIndex + 3;
      LoRa_buf[bytesWritten] = '\0';
      if(APRS_debug) debugA("---->  sendToLora_OE: bytesWritten = %d - EncapType = %d ", bytesWritten, EncapType);     
      };
   */  
}


void Service::kissPDUcheckTX(uint8_t *rxBuf , int rxBufIndex) {
   if(APRS_debug){ int i=0; while(i < rxBufIndex ){if(! (i%16))debugAl("\n\r")  ; debugAl("%02X ", rxBuf[i]); i++;   }; debugAl("\n\r");};     
   AX25::Payload payload(rxBuf, rxBufIndex);  // assume to be an AX.25 payload and check for validity     
   if (payload.IsValid()) {  //  APRS_AX25_Style LoRa packet
      if(APRS_debug) debugA("----FROM_LORA_TO_KISS--->  valid AX25 payload received from lora=[%s]\r",payload.ToString().c_str() );
      // it is a valid APRS payload;  add received rssi and snr values
      String textPayload = payload.ToString();
      //  if(APRS_debug) debugA("-------------------------------------------------------> From_LoRa     <== textPayload=[%s]   signalReport=[%s] \r\n",textPayload.c_str(), signalReport.c_str());
      // write to FRAM log the sent LoRa packet and send spot message to sarimesh server
      // send_log_msg(textPayload, signalReport, my_spot);   // send to spot log server tag 1
      }  // end of payload.IsValid
   else {
      rxBuf[rxBufIndex]='\0';
      if(APRS_debug) debugA("----FROM_LORA_TO_KISS--->  FAKE AX25 payload received from lora=[%s]\r", rxBuf);  
      };
}


void Service::kissForwardPacketFromLoRa(uint8_t *rxBuffer , int packetSize) {
   byte rxBuf[packetSize];  // local buffer
   kissPDUcheckTX(rxBuffer , packetSize) ;  // dump KISS packet we are going to send to KISS Host
   LastFunction = __func__ ;
  
   if((BT_KISS_Mode || Serial_KISS_Mode) && (!TCP_KISS_Mode )){
      aSerial->write(KissMarker::Fend);
      aSerial->write(KissCmd::Data);
      }
   else if(!BT_KISS_Mode && !Serial_KISS_Mode && TCP_KISS_Mode ){
      Kiss_client.write(KissMarker::Fend);
      Kiss_client.write(KissCmd::Data);
      };
   int ptr=0;   int rxBufIndex = 0;
   while (ptr <= packetSize) {
      byte rxByte = rxBuffer[ptr]; ptr++;
      if((BT_KISS_Mode || Serial_KISS_Mode) && (!TCP_KISS_Mode)){
         if (rxByte == KissMarker::Fend) {
            aSerial->write(KissMarker::Fesc);
            aSerial->write(KissMarker::Tfend);
            }
         else if(rxByte == KissMarker::Fesc){
            aSerial->write(KissMarker::Fesc);
            aSerial->write(KissMarker::Tfesc);
            }
         else {
            rxBuf[rxBufIndex++] = rxByte;
            aSerial->write(rxByte); 
            };  // send to serial if in KISS mode
         }
      else if(!BT_KISS_Mode && !Serial_KISS_Mode && TCP_KISS_Mode){
         if (rxByte == KissMarker::Fend) {
            Kiss_client.write(KissMarker::Fesc);
            Kiss_client.write(KissMarker::Tfend);
            }
         else if(rxByte == KissMarker::Fesc){
            Kiss_client.write(KissMarker::Fesc);
            Kiss_client.write(KissMarker::Tfesc);
            }
         else {
            rxBuf[rxBufIndex++] = rxByte;
            Kiss_client.write(rxByte); 
            };  // send to serial if in KISS mode
         };        
      };  // end of received packet scannning 
    
   if((BT_KISS_Mode || Serial_KISS_Mode) && (!TCP_KISS_Mode)){
      aSerial->write(KissMarker::Fend);
      if(APRS_debug) debugA("--> onLoraDataAvailable: end of Bt data repeat...\n");
      }
   else if(!BT_KISS_Mode && !Serial_KISS_Mode && TCP_KISS_Mode){
      Kiss_client.write(KissMarker::Fend);
      if(APRS_debug) debugA("--> onLoraDataAvailable: end of Bt data repeat...\n");  
      };
   
   Kiss_tx_packets++ ; // increment KISS TX packets counter
}

// all received packets from the LoRa radio goes trought this routine...
void Service::onLoraDataAvailable(uint8_t *rxBuffer , int packetSize, String signalReport, String my_spot) {
   static byte Msg[512]; Msg[0]='<' ; Msg[1]= 0xFF; Msg[2]=0x01;
   String OE_Packet_header = (char *)Msg ;    
   String OE_Packet = "";    
   float l_snr, rssi, bw, frequencyError ;
   byte rxBuf[packetSize];

   LastFunction = __func__ ;
   if(APRS_debug)  debugA("**************onLoraDataAvailable********************=> LoRa RX_packetSize=%d  \r", packetSize);
   int ptr=0;   int rxBufIndex = 0;
   while (ptr <= packetSize) {   // copy received packet to a local buffer
      byte rxByte = rxBuffer[ptr]; ptr++;
      rxBuf[rxBufIndex++] = rxByte;
      };  // end of received packet scannning   

   //  rxBufIndex-- ;   // by MF 20220130
   rxBuf[rxBufIndex +1 ]='\0'; // fix rxBuffer length
   //  rxBuf[rxBufIndex  ]='\0'; // fix rxBuffer length

   bool local_is_CRC_Errored = false ;
   int posCRC_Errored = signalReport.indexOf("CRC_Error");   // lets flag CRC_ErroredPackets
   if(posCRC_Errored > 0) {
      local_is_CRC_Errored = true;        
      };

   // we try now to identify received packet format... first just try to check if it is an AX.25 packet
   AX25::Payload payload(rxBuf, rxBufIndex);  // assume to be an AX.25 payload and check for validity     
   if (payload.IsValid()) {  //  APRS_AX25_Style LoRa packet
      if(APRS_debug) debugA("------->  valid AX25 payload received = [%s]\r",payload.ToString().c_str() );
      // it is a valid APRS payload;  add received rssi and snr values
      //  String textPayload = payload.ToString(addSignalReport_ ? signalReport : String());
      String textPayload = payload.ToString();
      // debugA("LoRa packet received => textPayload=["); payload.Dump(); debugA("signalReport: "); debugA(signalReport);debugA("]\r\n");
      if(APRS_debug) debugA("-------------------------------------------------------> From_LoRa     <== textPayload=[%s]   signalReport=[%s] \r\n",textPayload.c_str(), signalReport.c_str());
      // write to FRAM log the received LoRa packet and send spot message to sarimesh server
      send_log_msg(textPayload, signalReport, my_spot);   // send to spot log server tag 1
      // this is the "what to do with the received LoRa payload" task...
      if(local_is_CRC_Errored){ 
         debugA("--> Discarding  CRC_Errored packet= [%s]\r\n", textPayload.c_str());
         return ; 
         }; // no further actions for CRC errored packets

      if (isClient_) {        // just send the new packet to the KISS interface
         kissForwardPacketFromLoRa( rxBuffer ,  packetSize);   // send new lora packet to KISS interface
         return ;
         }
      else { //  !isClient_ so do futher processing ..... 
         if( (textPayload.indexOf("LoRa") < 0 ) || ( textPayload.indexOf("qAS") > 0 )) {
            debugA("--> IsRelayedAprsIs: skipping APRS-IS relayed payload or LoRa repeated packets %s\r\n",textPayload.c_str());
            return;
            } ;
         if(APRS_debug) debugA("onLoraDataAvailable: ===========> Packet: ownCallsign_= %s - textPayload= [%s]\r\n", ownCallsign_.ToString().c_str(), textPayload.c_str() );
         if (payload.Digirepeat(ownCallsign_)  ) {
            if(APRS_debug) debugA("\ronLoraDataAvailable: -------> payload after Digirepeat = [%s]\r",payload.ToString().c_str() );         
#ifndef APRS_IS_DISABLE
            if (enableRfToIs_  ) {  //  we are an iGate ... try to relay to APRS-IS by filtering payload
               if( (payload.ToString()).indexOf("FromInt") > 0  ) { 
                  if(APRS_debug) debugA("onLoraDataAvailable: Disposition: --> IsAprsIsOriginated; not sending to APRS-IS: %s\r",(payload.ToString()).c_str());
                  }
               else {
                  if( PayloadStyle == 1){
                     sendToAprsis( payload.ToString());  // pure_OE_style  no path tracing at all
                     }
                  else{
                     sendToAprsis( payload.ToString() + signalReport);
                     };
                  if(APRS_debug) debugA("Disposition: ==> Packet sent to APRS-IS\r");
               };
             };
#endif
         if( PayloadStyle == 1){
            payload.fromString(payload.ToString());   // add our passtag to the packet to be relayed or transferred to aprs-is
            }
         else{
            payload.fromString(payload.ToString() + signalReport);   // add our passtag to the packet to be relayed or transferred to aprs-is
            };      
         if ((enableRepeater_ ) && (RepeaterOperation == 0 )){
            if(APRS_debug) debugA("onLoraDataAvailable: Disposition: ==> Send Packet to LoRa..\r");
            vTaskDelay(pdMS_TO_TICKS(100));
            bool ret= sendToLora(payload, true, false );   // try do relay received packet to LoRa if this is required and filter payload 
            if(ret){ if(APRS_debug) debugA("To_LoRa (628) ==> [%s]\n",(payload.ToString()).c_str());  };
            if((APRS_debug)&& (ret))debugA("onLoraDataAvailable: Disposition => Packet digirepeated \r");
            };
         }
      else{
         if(APRS_debug) debugA("onLoraDataAvailable: Disposition => Packet NOT digirepeated: ownCallsign_= %s - payload=[%s]\r", ownCallsign_.ToString().c_str(), payload.ToString().c_str() );
         };
      }; // end of !isClient_
   }    // end of payload.IsValid() i.e. it is an AX.25 packet

   // following code is for NON AX.25 packets received
   // try to identify alternative encapsulation styles ( i.e. OE_Style , APRS434, etc.)
   // try for OE_Style packet format 
   else if ( (rxBuf[0] == '<') && (rxBuf[1] == 0xFF) && (rxBuf[2] == 0x01)  ) {     //  APRS_OE_Style LoRa packet
      rxBufIndex-- ;   // by MF 20220130
      rxBuf[rxBufIndex ]='\0';        
      OE_Packet =  (char *)rxBuf;  OE_Packet = OE_Packet.substring(3);
      //  OE_Packet = OE_Packet.substring(0, OE_Packet.length() - 1); // 20211116 by MF
      if(APRS_debug) debugA("onLoraDataAvailable: ---789---->   OE_Packet received= [%s]\n\r",OE_Packet.c_str() );
      // String OE_textPayload = addSignalReport_ ? signalReport : String();
      String OE_textPayload = "";
      // debugA("LoRa packet received => textPayload=["); payload.Dump(); debugA("signalReport: "); debugA(signalReport);debugA("]\r");
      // debugA("  From_LoRa_OE     <== [%s]\n",(OE_Packet + signalReport ).c_str());
      if(APRS_debug) debugA("From_LoRa_OE     <== OE_Packet=[%s]  signalReport=[%s] \n",OE_Packet.c_str(), signalReport.c_str());
      // write to FRAM log the received LoRa packet and send spot message to sarimesh server
      send_log_msg(OE_Packet, signalReport, my_spot);   // send to spot log server  tag 2
      if (isClient_) {  // just send the new packet to the KISS interface
         AX25::Payload AX25_payload(OE_Packet);   // create a standard payload in AX.25 format to be transferred to KISS host
         if (AX25_payload.IsValid()) {  //  APRS_AX25_Style LoRa packet
            if(APRS_debug) debugA("----AX25_payload ------->  valid AX25 payload created = [%s]\r",AX25_payload.ToString().c_str() );
            int new_buf_len= AX25_payload.ToString().length(); 
            byte new_buf[new_buf_len+1];
            AX25_payload.ToBinary( new_buf ,new_buf_len );                 
            kissForwardPacketFromLoRa( (uint8_t *)new_buf , new_buf_len );   // send new lora OE_packet transformed in AX.25 to KISS interface
            };
         return ;
         }
      else { //  !isClient_ futher processing ..... 
         // this is the "what to do with the received LoRa payload" task...
         if(local_is_CRC_Errored){ 
            debugA("--> Discarding  CRC_Errored packet= [%s]\r", OE_textPayload.c_str());
            return ; 
            }; // no further actions for CRC errored packets
         if( (OE_Packet.indexOf("LoRa") < 0 ) || ( OE_Packet.indexOf("qAS") > 0 )) {
            debugA("--> IsRelayedAprsIs: skipping APRS-IS relayed payload or LoRa repeated packets  [%s] - [%s]\r", OE_Packet.c_str(), OE_textPayload.c_str());
            return;
            } ;
         // it is a valid APRS_OE payload;  add received rssi and snr values
         OE_Packet.replace("\n","");  // fix "\n" in the packet  
         // fix  APLT00-1 OE_style destination to include WIDE1-1 in order to allow routing from a standard iGate repeater algoritm
         if(( OE_Packet.indexOf("APLT00-1") >= 0 ) && ( OE_Packet.indexOf("APLT00-1,WIDE1") < 0 ) )  {
            // perform APLT00-1 enforcement to APLT00-1,WIDE1-1
            if(APRS_debug) debugA("<==== performing  APLT00-1 fixing: OE_Packet= %s\r",OE_Packet.c_str() );
            OE_Packet.replace("APLT00-1","APLT00-1,WIDE1-1");
            if(APRS_debug) debugA("====> performing  APLT00-1 fixing: OE_Packet= %s\r",OE_Packet.c_str() );
            };
         if(( OE_Packet.indexOf("APLG01") >= 0 ) && ( OE_Packet.indexOf("APLG01,WIDE1") < 0 ) )  {
            // perform APLG01-1 enforcement to APLG01,WIDE1-1
            if(APRS_debug) debugA("<==== performing  APLG01-1 fixing: OE_Packet= %s\r",OE_Packet.c_str() );
            OE_Packet.replace("APLG01","APLG01,WIDE1-1");
            if(APRS_debug) debugA("====> performing  APLG01 fixing: OE_Packet= %s\r",OE_Packet.c_str() );
            };
         //  payload.fromString(OE_Packet + signalReport);   // create a standard payload and add our passtag to the packet to be relayed or transferred to aprs-is
         payload.fromString(OE_Packet);   // create a standard payload and add our passtag to the packet to be relayed or transferred to aprs-is
         // if(APRS_debug) debugA("onLoraDataAvailable: ====== AprsLoginRep= %s =====> OE_Packet: ownCallsign_= %s - textPayload= [%s]\r", AprsLoginRep.c_str(), ownCallsign_.ToString().c_str(), OE_textPayload.c_str() );
         if (payload.Digirepeat(ownCallsign_)  ) {
            if(APRS_debug) debugA("\ronLoraDataAvailable:  -------> payload after Digirepeat = [%s]\r",payload.ToString().c_str() );         
#ifndef APRS_IS_DISABLE
            if (enableRfToIs_  ) {  //  we are an iGate ... try to relay to APRS-IS by filtering payload
               if( (payload.ToString()).indexOf("FromInt") > 0  ) { 
                  if(APRS_debug) debugA("Disposition: --> IsAprsIsOriginated; not sending to APRS-IS: %s\r",(payload.ToString()).c_str());
                  }
               else {
                  if( PayloadStyle == 1){
                     sendToAprsis( payload.ToString());  // pure_OE_style  no path tracing at all
                     }
                  else{
                     sendToAprsis( payload.ToString() + signalReport);
                     };
                  if(APRS_debug) debugA("Disposition: ==> Packet sent to APRS-IS\r");
                  };
               };
#endif
            if( PayloadStyle == 1){
               payload.fromString(payload.ToString());   // add our passtag to the packet to be relayed or transferred to aprs-is
               }
            else{
               payload.fromString(payload.ToString() + signalReport);   // add our passtag to the packet to be relayed or transferred to aprs-is
               };

            if ((enableRepeater_ ) && (RepeaterOperation == 0 )){
               if(APRS_debug) debugA("onLoraDataAvailable: Disposition: ==> Packet sent to APRS-IS\r");
               vTaskDelay(pdMS_TO_TICKS(100));
               bool ret= sendToLora(payload, true , false);   // try to relay received packet to LoRa if this is required and filter payload 
               if(ret){if(APRS_debug)  debugA("  To_LoRa_OE (628) ==> [%s]\n",(payload.ToString()).c_str());  };
               if((APRS_debug)&& (ret))debugA("onLoraDataAvailable: Disposition => Packet digirepeated \r");
             };
            }
         else{
            if(APRS_debug) debugA("onLoraDataAvailable: Disposition => Packet NOT digirepeated: ownCallsign_= %s - payload= [%s]\r", ownCallsign_.ToString().c_str(), payload.ToString().c_str() );
            };
         }; // end of !isClient_ 
      }       // end of APRS_OE_Style LoRa packet

   else {   // this should never happen here... BTW just in case report this event
      if(LoRa_debug) debugA("onLoraDataAvailable: Disposition => Invalid or unsupported payload from LoRa: assume native LoRa packet \r");
      handle_nativeLoRaPacket(rxBuf, rxBufIndex, rssi, lora_snr, frequencyError);
   };
   
}  // end of  Service::onLoraDataAvailable


void Service::kissResetState() {
   kissCmd_ = KissCmd::NoCmd;
   kissState_ = KissState::Void;
} // end of Service::kissResetState


void Service::onBtDataAvailable() { // this is executed only in KIss mode... so no need to check for kiss mode explicitly herein
   int txBufIndex = 0; int bytesWritten=0;    
   uint8_t txBuffer[1024]; 
   byte LoRa_buf[1024];

   //  if(APRS_debug) debugA("onBtDataAvailable: enter BtDataAvailable...\n");

   int ptr=0;
   if( !TCP_KISS_Mode ){
      while (aSerial->available()) {
         byte txByte = aSerial->read();
         switch (kissState_) {
            case KissState::Void:
               if (txByte == KissMarker::Fend) {
                  kissCmd_ = KissCmd::NoCmd;
                  kissState_ = KissState::GetCmd;
                  };
               break;
            case KissState::GetCmd:
               if (txByte != KissMarker::Fend) {
                  if (txByte == KissCmd::Data) {
                     // LoRa.beginPacket();
                     ptr=0;
                     kissCmd_ = (KissCmd)txByte;
                     kissState_ = KissState::GetData;
                     }
                  else {
                     kissResetState();
                     };
                  };
               break;
            case KissState::GetData:
               if (txByte == KissMarker::Fesc) {
                  kissState_ = KissState::Escape;
                  }
               else if (txByte == KissMarker::Fend) {
                  if (kissCmd_ == KissCmd::Data) {
                     // LoRa.endPacket();
                     kissPDUcheckRX((uint8_t * )txBuffer, ptr);
                     Kiss_rx_packets++ ; // increment KISS TX packets counter
                     //  adapt PDU Encapsulation to adapt eventually to OE_Style
                     AX25::Payload tx_payload(txBuffer, ptr);  // assume to be an AX.25 payload and check for validity       
                     if(EncapType == 2 ){ // OE_Style packet encapsulation mode
                        sprintf((char *)LoRa_buf,"%c%c%c%s",'<',0xFF, 0x01, (tx_payload.ToString()).c_str());
                        bytesWritten = (tx_payload.ToString()).length() + 3;
                        if(APRS_debug) debugA("---->  sendToLora_OE: bytesWritten = %d - EncapType = %d ", bytesWritten, EncapType);     
                        }  
                     else { // AX25 packet encapsulation mode
                        sprintf((char *)LoRa_buf,"%s", txBuffer);
                        bytesWritten = ptr ;
                        if(APRS_debug) debugA("---->  sendToLora: bytesWritten = %d - EncapType = %d ", bytesWritten, EncapType);     
                        };               
                     LoRa_buf[bytesWritten] = '\0';
                     sendToLoraRaw((uint8_t * )LoRa_buf, bytesWritten , false, EncapType );  
                     };
                  kissResetState();
                  }
               else {
                  // LoRa.write(txByte);
                  txBuffer[ptr]= txByte ; ptr++;
                  };
               break;
            case KissState::Escape:
               if (txByte == KissMarker::Tfend) {
                  // LoRa.write(KissMarker::Fend);
                  txBuffer[ptr]= KissMarker::Fend ; ptr++;
                  kissState_ = KissState::GetData;
                  }
               else if (txByte == KissMarker::Tfesc) {
                  // LoRa.write(KissMarker::Fesc);
                  txBuffer[ptr]= KissMarker::Fesc ; ptr++;
                  kissState_ = KissState::GetData;
                  }
               else {
                  kissResetState();
                  };
               break;
            default:
               break;
            };
         };   // end of aSerial->available
      }  //  end of BT or Serial KISS mode
 
   else if(TCP_KISS_Mode){
      ptr=0;
      while (Kiss_client.available()) {
         byte txByte = Kiss_client.read();
         switch (kissState_) {
            case KissState::Void:
               if (txByte == KissMarker::Fend) {
                  kissCmd_ = KissCmd::NoCmd;
                  kissState_ = KissState::GetCmd;
                  };
               break;
            case KissState::GetCmd:
               if (txByte != KissMarker::Fend) {
                  if (txByte == KissCmd::Data) {
                     // LoRa.beginPacket();
                     ptr=0;
                     kissCmd_ = (KissCmd)txByte;
                     kissState_ = KissState::GetData;
                     }
                  else {
                     kissResetState();
                     };
                  };
               break;
            case KissState::GetData:
               if (txByte == KissMarker::Fesc) {
                  kissState_ = KissState::Escape;
                  }
               else if (txByte == KissMarker::Fend) {
                  if (kissCmd_ == KissCmd::Data) {
                     // LoRa.endPacket();
                     kissPDUcheckRX((uint8_t * )txBuffer, ptr);  
                     Kiss_rx_packets++ ; // increment KISS TX packets counter                    
                     //  adapt PDU Encapsulation to adapt eventually to OE_Style
                     AX25::Payload tx_payload(txBuffer, ptr);  // assume to be an AX.25 payload and check for validity       
                     if(EncapType == 2 ){ // OE_Style packet encapsulation mode
                        sprintf((char *)LoRa_buf,"%c%c%c%s",'<',0xFF, 0x01, (tx_payload.ToString()).c_str());
                        bytesWritten = (tx_payload.ToString()).length() + 3;
                        if(APRS_debug) debugA("---->  sendToLora_OE: bytesWritten = %d - EncapType = %d ", bytesWritten, EncapType);     
                        }  
                     else { // AX25 packet encapsulation mode
                        sprintf((char *)LoRa_buf,"%s", txBuffer);
                        bytesWritten = ptr ;
                        if(APRS_debug) debugA("---->  sendToLora: bytesWritten = %d - EncapType = %d ", bytesWritten, EncapType);     
                        };        
                     LoRa_buf[bytesWritten] = '\0';
                     sendToLoraRaw((uint8_t * )LoRa_buf, bytesWritten , true , EncapType );
                     };
                  kissResetState();
                  }
               else {
                  // LoRa.write(txByte);
                  txBuffer[ptr]= txByte ; ptr++;
                  };
               break;
            case KissState::Escape:
               if (txByte == KissMarker::Tfend) {
                  // LoRa.write(KissMarker::Fend);
                  txBuffer[ptr]= KissMarker::Fend ; ptr++;
                  kissState_ = KissState::GetData;
                  }
               else if (txByte == KissMarker::Tfesc) {
                  // LoRa.write(KissMarker::Fesc);
                  txBuffer[ptr]= KissMarker::Fesc ; ptr++;
                  kissState_ = KissState::GetData;
                  }
               else {
                  kissResetState();
                  };
               break;
            default:
               break;
            };
         };   // end of Kiss_client.available
      };  //  end of TCP KISS mode
}   // end of Service::onBtDataAvailable()


bool Service::sendToLoraRaw(uint8_t *buffer, int bytesToSend , bool SkipIfNeeded, uint8_t Encap) {
   // uint8_t Encap = 0; // native  data no encapsulation
   LDH_sendNativeToLora( (uint8_t * )buffer, bytesToSend , Encap , false,SkipIfNeeded ) ; // this calls directly the LDH send function 
   // sendNativeToLora((uint8_t * )buffer , bytesToSend, Encap);  // this calls LDH send function via IPC 
   return true;  
}  // end of Service::sendToLoraRaw



bool Service::sendNativeToLora(byte *txBuf, int txBufIndex, uint8_t Encap ) {
   LastFunction = __func__ ;
   LDH_Message_v2 LDM_Msg_v2 ;
   if(LoRa_debug) debugA("sendNativeToLora: IPC request to LDH to send a packet\r");
   // send packet to be transmitted to LDH via its xQueue_in
   LDM_Msg_v2.MsgType = 3 ;      // 3=transmit pkt (auto CAD selection) 4=transmit pkt no CAD
   LDM_Msg_v2.MsgSqNbr = MsgStdSeqNbr++ ;   // use MsgStdSeqNbr to tag this message and autoincrement
   LDM_Msg_v2.dueTime = 0.0 ;    // auto selection
   // LDM_Msg_v2.dueTime = millis() + 5000 ;    // auto selection  
   LDM_Msg_v2.ack = 0 ;          // indicates a query packet      
   //  memcpy(LDM_Msg_v2.bufData, LDH_Buffer, sizeof(LDH_Buffer[0])*packetSize);
   memcpy(LDM_Msg_v2.bufData, txBuf, txBufIndex );  // fill buffer with packet content to transmit
   LDM_Msg_v2.bufLen = txBufIndex ;                 // fill with packet size
   LDM_Msg_v2.SigRep[0] = (uint8_t) Encap ;         // we use this field here to signal packet encap for statistics
   // send LDM_Msg_v2 to LDH to perform packet transmission for us
   //  if(APRS_debug) debugA("sendNativeToLora: sending packet via xQueue_in....\r" );
   //  if(APRS_debug){ int i=0; while(i < LDM_Msg_v2.bufLen ){if(! (i%16))debugAl("\r")  ; debugAl("%02X ", LDM_Msg_v2.bufData[i]); i++;   }; debugAl("\r");};
   if( xQueueSend( xQueue_in, (void *)&LDM_Msg_v2, ( TickType_t ) 10 ) != pdPASS ) {
      if(LoRa_debug) debugA("sendNativeToLora: unable to queue message toward LDM xQueue_in....\r");
      // TX packet lost due to IPC fault
      IPC_lost_msgs++ ;
      return(false);
      };
   return(true);
}  // end of Service::sendNativeToLora


bool Service::SchedNativeToLora(byte *txBuf, int txBufIndex, uint8_t Encap , uint32_t schedTime, bool SkipIfNeeded) {
   LastFunction = __func__ ;
   LDH_Message_v2 LDM_Msg_v2 ;
   //  if(LoRa_debug) debugA("SchedNativeToLora: ===>  IPC request to park packet for later transmission\r");
   // send packet to be transmitted to LDH via its xQueue_in
   LDM_Msg_v2.MsgType = 3 ;      // 3=transmit pkt (auto CAD selection) 4=transmit pkt no CAD
   LDM_Msg_v2.SkipIfNeeded = SkipIfNeeded ;  
   LDM_Msg_v2.MsgSqNbr = MsgStdSeqNbr++ ;   // use MsgStdSeqNbr to tag this message and autoincrement
   // LDM_Msg_v2.dueTime = 0.0 ;    // auto selection
   // LDM_Msg_v2.dueTime = millis() + 2000 ;  // delay trasmission by 1000 msecs
   // LDM_Msg_v2.dueTime = millis() + 5000 ;    // auto selection
   if( schedTime == 0 ) {
      int rnddelay = random(0, 10)* 1500 ;
      if(LoRa_debug) debugA("SchedNativeToLora: ===>>>>>> random delay = %d\r",rnddelay);
      LDM_Msg_v2.dueTime = millis() + rnddelay ;  // delay trasmission by 1000 msecs
      }
   else {
      LDM_Msg_v2.dueTime = schedTime ;
      };   
   LDM_Msg_v2.ack = 0 ;          // indicates a query packet      
   memcpy(LDM_Msg_v2.bufData, txBuf, txBufIndex );  // fill buffer with packet content to transmit
   LDM_Msg_v2.bufLen = txBufIndex ;                 // fill with packet size
   LDM_Msg_v2.SigRep[0] = (uint8_t) Encap ;         // we use this field here to signal packet encap for statistics
              
   // send LDM_Msg_v2 to parking queue to perform delayed packet transmission
   if(LoRa_debug) debugA("SchedNativeToLora: enqueue  packet to xQueue_park....\r" );
   // if(LoRa_debug){ int i=0; while(i < LDM_Msg_v2.bufLen ){if(! (i%16))debugAl("\r")  ; debugAl("%02X ", LDM_Msg_v2.bufData[i]); i++;   }; debugAl("\r");};

   int messagesWaiting ;
   int emptySpaces ;  
   if( Encap > 0 ){
      if( xQueueSend( xQueue_park, (void *)&LDM_Msg_v2, ( TickType_t ) 10 ) != pdPASS ) {
         if(LoRa_debug) debugA("SchedNativeToLora: unable to enqueue message toward xQueue_park....\r");
         // TX packet lost due to IPC fault
         IPC_lost_msgs++ ;
         return(false);
         };
      messagesWaiting = uxQueueMessagesWaiting(xQueue_park);
      emptySpaces = uxQueueSpacesAvailable(xQueue_park);      
      if(LoRa_debug)  debugA("SchedNativeToLora: on exit ===>>>>>>  IPC xQueue_park:  messagesWaiting=%d - emptySpaces=%d - LoRa_ReSched_packets=%d\r", messagesWaiting, emptySpaces, LoRa_ReSched_packets + 1);                     
      }
   else {
      if( xQueueSend( xQueue_native_park, (void *)&LDM_Msg_v2, ( TickType_t ) 10 ) != pdPASS ) {
         if(LoRa_debug) debugA("SchedNativeToLora: unable to enqueue message toward xQueue_native_park....\r");
         // TX packet lost due to IPC fault
         IPC_lost_msgs++ ;
         return(false);
         };        
      messagesWaiting = uxQueueMessagesWaiting(xQueue_native_park);
      emptySpaces = uxQueueSpacesAvailable(xQueue_native_park);      
      if(LoRa_debug) debugA("SchedNativeToLora: on exit ===>>>>>>  IPC xQueue_native_park:  messagesWaiting=%d - emptySpaces=%d - LoRa_ReSched_packets=%d\r", messagesWaiting, emptySpaces, LoRa_ReSched_packets + 1);                
      };

   LoRa_ReSched_packets++;
   return(true);
}  // end of Service::SchedNativeToLora


bool Service::SchedNativeToLora_noCAD(byte *txBuf, int txBufIndex, uint8_t Encap , uint32_t schedTime , uint32_t repTime , struct BeaconRunTimeConfig &BCN_vector , bool SkipIfNeeded ) {
   LDH_Message_v2 LDM_Msg_v2 ;
   LastFunction = __func__ ;
   //  if(LoRa_debug) debugA("SchedNativeToLora_noCAD: =========================  RRRRRRRRRRRRRRRRRr ========================================>>>>>>  IPC request to park packet for later transmission\r");
   // send packet to be transmitted to LDH via its xQueue_park/native_park
   LDM_Msg_v2.MsgType = 4 ;      // 3=transmit pkt (auto CAD selection) 4=transmit pkt no CAD
   LDM_Msg_v2.MsgSqNbr = MsgStdSeqNbr++ ;   // use MsgStdSeqNbr to tag this message and autoincrement
   LDM_Msg_v2.SkipIfNeeded = SkipIfNeeded ;
   LDM_Msg_v2.Lv = BCN_vector ;
   LDM_Msg_v2.ReschedTime = repTime ;
   // LDM_Msg_v2.dueTime = 0.0 ;    // auto selection
   if( schedTime == 0 ) {
      int rnddelay = random(0, 10)* 1500 ;
      if(LoRa_debug) debugA("SchedNativeToLora_noCAD: ===>>>>>> random delay = %d\r",rnddelay);
      LDM_Msg_v2.dueTime = millis() + rnddelay ;  // delay trasmission by 1000 msecs
      }
   else {
      LDM_Msg_v2.dueTime = schedTime ;
      };  
   //LDM_Msg_v2.dueTime = millis() + 5000 ;    // auto selection
   LDM_Msg_v2.ack = 0 ;          // indicates a query packet      
   memcpy(LDM_Msg_v2.bufData, txBuf, txBufIndex );  // fill buffer with packet content to transmit
   LDM_Msg_v2.bufLen = txBufIndex ;                 // fill with packet size
   LDM_Msg_v2.SigRep[0] = (uint8_t) Encap ;         // we use this field here to signal packet encap for statistics
              
   // send LDM_Msg_v2 to parking queue to perform delayed packet transmission
   if(LoRa_debug) debugA("SchedNativeToLora_noCAD: enqueue  packet to xQueue_park....\r" );
   // if(LoRa_debug){ int i=0; while(i < LDM_Msg_v2.bufLen ){if(! (i%16))debugAl("\r")  ; debugAl("%02X ", LDM_Msg_v2.bufData[i]); i++;   }; debugAl("\r");};
   int messagesWaiting ;
   int emptySpaces ;      
   if( Encap > 0 ){
      if( xQueueSend( xQueue_park, (void *)&LDM_Msg_v2, ( TickType_t ) 10 ) != pdPASS ) {
         if(LoRa_debug) debugA("SchedNativeToLora_noCAD: unable to enqueue message toward xQueue_park....\r");
         // TX packet lost due to IPC fault
         IPC_lost_msgs++ ;
         return(false);
         };
      messagesWaiting = uxQueueMessagesWaiting(xQueue_park);
      emptySpaces = uxQueueSpacesAvailable(xQueue_native_park);      
      if(LoRa_debug) debugA("SchedNativeToLora_noCAD: on exit ===>>>>>>  IPC xQueue_park:  messagesWaiting=%d - emptySpaces=%d - LoRa_ReSched_packets=%d\r", messagesWaiting, emptySpaces, LoRa_ReSched_packets + 1);                     
      }
   else {
      if( xQueueSend( xQueue_native_park, (void *)&LDM_Msg_v2, ( TickType_t ) 10 ) != pdPASS ) {
         if(LoRa_debug) debugA("SchedNativeToLora_noCAD: unable to enqueue message toward xQueue_native_park....\r");
         // TX packet lost due to IPC fault
         IPC_lost_msgs++ ;
         return(false);
         }; 
      messagesWaiting = uxQueueMessagesWaiting(xQueue_native_park);
      emptySpaces = uxQueueSpacesAvailable(xQueue_native_park);      
      if(LoRa_debug) debugA("SchedNativeToLora_noCAD: on exit ===>>>>>>  IPC xQueue_native_park:  messagesWaiting=%d - emptySpaces=%d - LoRa_ReSched_packets=%d\r", messagesWaiting, emptySpaces, LoRa_ReSched_packets + 1);                
      };
         
   LoRa_ReSched_packets++;
   return(true);
}  // end of Service::SchedNativeToLora_noCAD

void Service::PrintLoraPrsConfig_private(const Config &cfg) {
   debugA("\nPrintLoraPrsConfig_private: dump active loraaprs private configuration");
   // lora parameters
   debugA("LoRa configuration\r");
   debugA("loraFreq_:                          %f Mhz\r", loraFreq_ );    //  
   debugA("cfg.LoraBw:                         %f Khz\r", cfg.LoraBw );    //  
   debugA("cfg.LoraSf:                         %d \r", cfg.LoraSf );    //  
   debugA("cfg.LoraCodingRate:                 %d \r", cfg.LoraCodingRate );    //  
   debugA("cfg.LoraSync:                       %02X \r", cfg.LoraSync );    //  
   debugA("cfg.LoraPower:                      %d dbm\r", cfg.LoraPower );    //  
   // aprs configuration
   debugA("aprs configuration\r");
   debugA("ownCallsign_:                       %s\r", ownCallsign_.ToString() );    // 
   debugA("aprsHost_:                          %s\r", aprsHost_.c_str() );    // client/server mode switch
   debugA("aprsPort_:                          %d\r", aprsPort_ );    // client/server mode switch
   debugA("aprsLogin_:                         %s", aprsLogin_.c_str() );    // client/server mode switch
   debugA("cfg.AprsPass:                       %s\r", cfg.AprsPass );    // client/server mode switch
   debugA("cfg.AprsFilter:                     %s\r", cfg.AprsFilter );    // client/server mode switch
   debugA("aprsBeacon_:                        %s\r", aprsBeacon_.c_str() );    // client/server mode switch
   debugA("aprsBeaconPeriodSeconds_:           %d\r", aprsBeaconPeriodSeconds_  );    // client/server mode switch
   debugA("cfg.BtName:                         %s\r", cfg.BtName.c_str() );    // client/server mode switch
   debugA("cfg.WifiSsid:                       %s\r", cfg.WifiSsid.c_str() );    // client/server mode switch
   debugA("cfg.WifiKey:                        %s\r", cfg.WifiKey.c_str() );    // client/server mode switch
   // configuration flags and features
   debugA("configuration flags and features\r");
   debugA("isClient_                           %d\r", isClient_ );    // client/server mode switch
   debugA("persistentConn_:                    %d\r", persistentConn_ );    // 
   debugA("enableRfToIs_:                      %d\r", enableRfToIs_  );    // 
   debugA("enableIsToRf_:                      %d\r", enableIsToRf_ );    // 
   debugA("enableRepeater_:                    %d\r", enableRepeater_ );    // 
   debugA("enableBeacon_:                      %d\r", enableBeacon_ );    // 
   debugA("autoCorrectFreq_:                   %d\r", autoCorrectFreq_ );    // 
   debugA("addSignalReport_ :                  %d\r", addSignalReport_ );    // 
   debugA("\r");
   debugA("\nPrintLoraPrsConfig_private: end of dump active loraaprs private configuration\r\r");
} // end of Service::PrintLoraPrsConfig_private



} // LoraPrs

//#endif //  LORA_FUNCTIONS_ENABLE 
