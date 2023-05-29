// native_lora_handler.cpp
#include "native_lora_handler.h"

void native_loop() {     // main native service loop
  long r = random(0, 255);
  if(! LoRa_initialized ) return ;  // lora communication is not jet initialized or in standby mode.. just wait...
  // show native parking queue status
  int messagesWaiting = uxQueueMessagesWaiting(xQueue_native_park);
  int emptySpaces = uxQueueSpacesAvailable(xQueue_native_park);
  // Receive a message from the native parking queue.  Block for 10 ticks if a message is not immediately available.
  LDH_Message_v2 LDM_Msg_v2 ;
  if (xQueueReceive(xQueue_native_park, (void *)&LDM_Msg_v2, ( TickType_t ) 10 ) == pdTRUE) {  // wait  10 secs
    // show native parking queue status
    // if(LoRa_debug)  debugA("native_loop: ====>>>>>>  IPC messagesWaiting=%d - emptySpaces=%d\r", messagesWaiting, emptySpaces);
    if(LDM_Msg_v2.MsgType == 3 ){   // packet transmission request auto CAD
      // debugA("native_loop-MSG_IN-3: LoRa packet transmission request received via IPP MsgSqNbr =%d - dueTime=%ld .... \r", LDM_Msg_v2.MsgSqNbr , LDM_Msg_v2.dueTime  ); 
      if(((LDM_Msg_v2.dueTime == 0.0 ) || (millis() > LDM_Msg_v2.dueTime))  ){
        if(LoRa_debug) debugA("native_loop-MSG_IN-3: EXEC LoRa packet transmission MsgSqNbr =%d - dueTime=%ld .... \r", LDM_Msg_v2.MsgSqNbr , LDM_Msg_v2.dueTime  ); 
          LDH_sendNativeToLora(LDM_Msg_v2.bufData , LDM_Msg_v2.bufLen , (uint8_t)LDM_Msg_v2.SigRep[0] , true , false );  // this calls directly the LDH send function          
          }
        else {
          // packet scheduled for later time transmission...  re-enqueue transmission request
          // LDM_Msg_v2.dueTime = millis() + 5000 ;  // delay retrasmission by 5000 msec
          if( xQueueSend( xQueue_native_park, (void *)&LDM_Msg_v2, ( TickType_t ) 10 ) != pdPASS ) {
            // if(LoRa_debug) debugA("=====QUEUE_PARK===> native_loop: IPC TX request re-enqueueing failed: MsgSqNbr=%d....\r", LDM_Msg_v2.MsgSqNbr );
            // flag lost packet due to IPC overflow
            IPC_lost_msgs++ ;
            } 
          else {
            // if(LoRa_debug) debugA("=====QUEUE_NATIVE_PARK===> native_loop: IPC TX request re-enqueueing success: MsgSqNbr=%d....\r", LDM_Msg_v2.MsgSqNbr );
            };           
          };
        }    // end of if(LDM_Msg_v2.MsgType == 3 )  
      else if(LDM_Msg_v2.MsgType == 4 ){   // packet transmission request no CAD -  support message rescheduling
        // debugA("\r\rnative_loop-MSG_IN-4: LoRa packet transmission request received via IPP MsgSqNbr =%d - dueTime=%ld .... \r", LDM_Msg_v2.MsgSqNbr , LDM_Msg_v2.dueTime  ); 
        if(((LDM_Msg_v2.dueTime == 0.0 ) || (millis() > LDM_Msg_v2.dueTime))  ){
          if(LoRa_debug)debugA("native_loop-MSG_IN: EXEC LoRa packet transmission MsgSqNbr =%d - dueTime=%ld .... \r", LDM_Msg_v2.MsgSqNbr , LDM_Msg_v2.dueTime  ); 
          float LoraFreq_nominal =  LDM_Msg_v2.Lv.LoraFreq * ( 1.0 + (float)(LDM_Msg_v2.Lv.LoraFreqCorr)/10000000.0 );   
          if ( LDH_SetLoraConfig( LoraFreq_nominal, LDM_Msg_v2.Lv.LoraBw  ,LDM_Msg_v2.Lv.LoraSf , LDM_Msg_v2.Lv.LoraCodingRate ,  LDM_Msg_v2.Lv.LoraPower , LDM_Msg_v2.Lv.LoraSync ,  LDM_Msg_v2.Lv.LoraPreambleLen   , "" )){
            // debugA("native_loop-MSG_IN-4: LDH_SetLoraConfig success: LoraFreq=%8.4f - LoraBw=%8.4f - LoraSf=%d - LoraCodingRate=%d  - LoraPower=%d - LoraSync=%02X - LoraPreambleLen=%d \r", LDM_Msg_v2.Lv.LoraFreq, LDM_Msg_v2.Lv.LoraBw  ,LDM_Msg_v2.Lv.LoraSf , LDM_Msg_v2.Lv.LoraCodingRate ,  LDM_Msg_v2.Lv.LoraPower , LDM_Msg_v2.Lv.LoraSync ,  LDM_Msg_v2.Lv.LoraPreambleLen  ); 
            BCN_LDH_cfg = LDM_Msg_v2.Lv ;  // set BCN_LDH_cfg
            };
          byte BCN_Payload[40]; int BCN_PayloadLen = 0 ;
          BCN_PayloadLen = BeaconEncode( LDM_Msg_v2.Lv ,(byte *)BCN_Payload );   // create present spot binary payload 
          // debugA("================>   native_loop-MSG_IN-4: after BeaconEncode: BCN_Payload=%s - BCN_PayloadLen=%d \r", BCN_Payload , BCN_PayloadLen ); 
          LDH_sendNativeToLora_noCAD( (uint8_t *)BCN_Payload , BCN_PayloadLen , (uint8_t)LDM_Msg_v2.SigRep[0] , true , false);  // this calls directly the LDH send function          
          if(LDM_Msg_v2.ReschedTime){     // support for periodic rescheduling feature
            MsgStdSeqNbr++; LDM_Msg_v2.MsgSqNbr = MsgStdSeqNbr ;
            LDM_Msg_v2.dueTime = LDM_Msg_v2.dueTime + LDM_Msg_v2.ReschedTime ;
            // packet scheduled for later time transmission...  re-enqueue transmission request
            if( xQueueSend( xQueue_native_park, (void *)&LDM_Msg_v2, ( TickType_t ) 10 ) != pdPASS ) {
              // if(LoRa_debug) 
              //       debugA("=====QUEUE_NATIVE_PARK - RESCHEDULE BLOCKING FAULT===> native_loop - reschedule : IPC TX request re-enqueueing failed: MsgSqNbr=%d....\r", LDM_Msg_v2.MsgSqNbr );
              // flag lost packet due to IPC overflow
              IPC_lost_msgs++ ;
              } 
            else {
              // if(LoRa_debug) debugA("=====QUEUE_NATIVE_PARK===> native_loop - reschedule: IPC TX request re-enqueueing success: MsgSqNbr=%d....\r", LDM_Msg_v2.MsgSqNbr );
              };         
            };  // end of packet rescheduling
          }
        else {
          // packet scheduled for later time transmission...  re-enqueue transmission request
          if( xQueueSend( xQueue_native_park, (void *)&LDM_Msg_v2, ( TickType_t ) 10 ) != pdPASS ) {
            // if(LoRa_debug) debugA("=====QUEUE_NATIVE_PARK===> native_loop: IPC TX request re-enqueueing failed: MsgSqNbr=%d....\r", LDM_Msg_v2.MsgSqNbr );
            // flag lost packet due to IPC overflow
            IPC_lost_msgs++ ;
            } 
          else {
            // if(LoRa_debug) debugA("=====QUEUE_NATIVE_PARK===> native_loop: IPC TX request re-enqueueing success: MsgSqNbr=%d....\r", LDM_Msg_v2.MsgSqNbr );
            };                     
          };
        }   // end of if(LDM_Msg_v2.MsgType == 4 )  
      else if(LDM_Msg_v2.MsgType == 5 ){   // enter RX Beacon mode 
        // if(LoRa_debug) debugA("native_loop-MSG_IN-5: LoRa enter RX Beacon mode request received via IPP MsgSqNbr =%d - dueTime=%ld .... \r", LDM_Msg_v2.MsgSqNbr , LDM_Msg_v2.dueTime  ); 
        if( ((LDM_Msg_v2.dueTime == 0.0 ) || (millis() > LDM_Msg_v2.dueTime)) &&  (rtxFlag != 2) ){
          // if(LoRa_debug) debugA("native_loop-MSG_IN-5: EXEC LoRa enter RX Beacon mode MsgSqNbr =%d - dueTime=%ld .... \r", LDM_Msg_v2.MsgSqNbr , LDM_Msg_v2.dueTime  ); 
          String time_now =  Italy.dateTime("Ymd-H:i:s");
          //if(LoRa_debug) 
          debugA("\r\r\n\r\n----[%s]-----> native_loop: LDH_SetLoraConfig (enter RX Beacon mode)\r", time_now.c_str());        
          if(syslog_available && BeaconEngineType == 0 ) syslog.logf(LOG_INFO, "enter LDH_RX_Mode ( with TX ) beacon at [%s]", time_now.c_str() );
          if(syslog_available && BeaconEngineType == 1 ) syslog.logf(LOG_INFO, "enter LDH_RX_Mode ( no TX ) beacon at [%s]", time_now.c_str() );
          float LoraFreq_nominal =  LDM_Msg_v2.Lv.LoraFreq * ( 1.0 + (float)(LDM_Msg_v2.Lv.LoraFreqCorr)/10000000.0 );   
          if ( LDH_SetLoraConfig( LoraFreq_nominal, LDM_Msg_v2.Lv.LoraBw  ,LDM_Msg_v2.Lv.LoraSf , LDM_Msg_v2.Lv.LoraCodingRate ,  LDM_Msg_v2.Lv.LoraPower , LDM_Msg_v2.Lv.LoraSync ,  LDM_Msg_v2.Lv.LoraPreambleLen   , "" )){
            // if(LoRa_debug) debugA("native_loop-MSG_IN-5: LDH_SetLoraConfig success: LoraFreq=%8.4f - LoraBw=%8.4f - LoraSf=%d - LoraCodingRate=%d  - LoraPower=%d - LoraSync=%02X - LoraPreambleLen=%d \r", LDM_Msg_v2.Lv.LoraFreq, LDM_Msg_v2.Lv.LoraBw  ,LDM_Msg_v2.Lv.LoraSf , LDM_Msg_v2.Lv.LoraCodingRate ,  LDM_Msg_v2.Lv.LoraPower , LDM_Msg_v2.Lv.LoraSync ,  LDM_Msg_v2.Lv.LoraPreambleLen  ); 
            BCN_LDH_cfg = LDM_Msg_v2.Lv ;  // set BCN_LDH_cfg
            };
          LDH_RX_Mode = 1 ; // enter LDH_RX_Mode beacon
          // if(LoRa_debug) debugA("native_loop-MSG_IN-5: enter LDH_RX_Mode success: now in Beacon_Mode... \r" );       
          if(LDM_Msg_v2.ReschedTime){     // support for periodic rescheduling feature
            MsgStdSeqNbr++; LDM_Msg_v2.MsgSqNbr = MsgStdSeqNbr ;
            LDM_Msg_v2.dueTime = LDM_Msg_v2.dueTime + LDM_Msg_v2.ReschedTime ;
            if(LoRa_debug) debugA("native_loop-MSG_IN-5:  dueTime=%d  - ReschedTime=%d\r\n", LDM_Msg_v2.dueTime , LDM_Msg_v2.ReschedTime);
            // packet scheduled for later time transmission...  re-enqueue transmission request
            if( xQueueSend( xQueue_native_park, (void *)&LDM_Msg_v2, ( TickType_t ) 10 ) != pdPASS ) {
              // if(LoRa_debug)debugA("=====QUEUE_NATIVE_PARK - RESCHEDULE BLOCKING FAULT===> native_loop - reschedule : IPC TX request re-enqueueing failed: MsgSqNbr=%d....\r", LDM_Msg_v2.MsgSqNbr );
              // flag lost packet due to IPC overflow
              IPC_lost_msgs++ ;
              } 
            else {
              // if(LoRa_debug) debugA("=====QUEUE_NATIVE_PARK===> native_loop - reschedule: IPC TX request re-enqueueing success: MsgSqNbr=%d....\r", LDM_Msg_v2.MsgSqNbr );
              };       
            };  // end of packet rescheduling
          }
        else {
          // packet scheduled for later time transmission...  re-enqueue enter RX Beacon mode request
          if( xQueueSend( xQueue_native_park, (void *)&LDM_Msg_v2, ( TickType_t ) 10 ) != pdPASS ) {
            // if(LoRa_debug)  debugA("=====QUEUE_NATIVE_PARK===> native_loop: IPC TX request re-enqueueing failed: MsgSqNbr=%d....\r", LDM_Msg_v2.MsgSqNbr );
            // flag lost packet due to IPC overflow
            IPC_lost_msgs++ ;
            } 
          else {
            // if(LoRa_debug)   debugA("=====QUEUE_NATIVE_PARK===> native_loop: IPC TX request re-enqueueing success: MsgSqNbr=%d....\r", LDM_Msg_v2.MsgSqNbr );
            };                     
          };
        }   // end of if(LDM_Msg_v2.MsgType == 5 )   
      else if(LDM_Msg_v2.MsgType == 6 ){   // exit RX Beacon mode 
        // if(LoRa_debug) debugA("native_loop-MSG_IN-6: LoRa exit RX Beacon mode request received via IPP MsgSqNbr =%d - dueTime=%ld .... \r", LDM_Msg_v2.MsgSqNbr , LDM_Msg_v2.dueTime  ); 
        if( ((LDM_Msg_v2.dueTime == 0.0 ) || (millis() > LDM_Msg_v2.dueTime)) &&  (rtxFlag != 2) ){
          // if(LoRa_debug) debugA("native_loop-MSG_IN-6: EXEC LoRa exit RX Beacon mode MsgSqNbr =%d - dueTime=%ld .... \r", LDM_Msg_v2.MsgSqNbr , LDM_Msg_v2.dueTime  ); 
          String time_now =  Italy.dateTime("Ymd-H:i:s");
          // if(LoRa_debug) 
          debugA("\r----[%s]-----> native_loop: LDH_SetLoraConfig (exit RX Beacon mode) \r\r\n\r\n", time_now.c_str());        
          if(syslog_available && BeaconEngineType == 0 ) syslog.logf(LOG_INFO, "exit LDH_RX_Mode ( with TX ) beacon at [%s]", time_now.c_str() );
          if(syslog_available && BeaconEngineType == 1 ) syslog.logf(LOG_INFO, "exit LDH_RX_Mode ( no TX ) beacon at [%s]", time_now.c_str() );
          float LoraFreq_nominal =  BCN_APRS_cfg.LoraFreq * ( 1.0 + (float)(BCN_APRS_cfg.LoraFreqCorr)/10000000.0 );   
          if ( LDH_SetLoraConfig( LoraFreq_nominal, BCN_APRS_cfg.LoraBw  ,BCN_APRS_cfg.LoraSf , BCN_APRS_cfg.LoraCodingRate ,  BCN_APRS_cfg.LoraPower , BCN_APRS_cfg.LoraSync ,  BCN_APRS_cfg.LoraPreambleLen   , "" )){
            // if(LoRa_debug) debugA("native_loop-MSG_IN-6: LDH_SetLoraConfig success: LoraFreq=%8.4f - LoraBw=%8.4f - LoraSf=%d - LoraCodingRate=%d  - LoraPower=%d - LoraSync=%02X - LoraPreambleLen=%d \r", BCN_APRS_cfg.LoraFreq, BCN_APRS_cfg.LoraBw  ,BCN_APRS_cfg.LoraSf , BCN_APRS_cfg.LoraCodingRate ,  BCN_APRS_cfg.LoraPower , BCN_APRS_cfg.LoraSync ,  BCN_APRS_cfg.LoraPreambleLen  ); 
            BCN_LDH_cfg = BCN_APRS_cfg ;  // set BCN_LDH_cfg
            };        
          LDH_RX_Mode = 0 ; // exit LDH_RX_Mode beacon, now in APRS RX mode
          // if(LoRa_debug) debugA("native_loop-MSG_IN-6: exit LDH_RX_Mode success: now in APRS_Mode... \r" );       
          if(LDM_Msg_v2.ReschedTime){     // support for periodic rescheduling feature
            MsgStdSeqNbr++; LDM_Msg_v2.MsgSqNbr = MsgStdSeqNbr ;
            LDM_Msg_v2.dueTime = LDM_Msg_v2.dueTime + LDM_Msg_v2.ReschedTime ;
            // packet scheduled for later time transmission...  re-enqueue transmission request
            if( xQueueSend( xQueue_native_park, (void *)&LDM_Msg_v2, ( TickType_t ) 10 ) != pdPASS ) {
              // if(LoRa_debug) debugA("=====QUEUE_NATIVE_PARK - RESCHEDULE BLOCKING FAULT===> native_loop - reschedule : IPC TX request re-enqueueing failed: MsgSqNbr=%d....\r", LDM_Msg_v2.MsgSqNbr );
              // flag lost packet due to IPC overflow
              IPC_lost_msgs++ ;
              } 
            else {
              // if(LoRa_debug) debugA("=====QUEUE_NATIVE_PARK===> native_loop - reschedule: IPC TX request re-enqueueing success: MsgSqNbr=%d....\r", LDM_Msg_v2.MsgSqNbr );
              };       
            };  // end of packet rescheduling
          }
        else {
          // packet scheduled for later time transmission...  re-enqueue enter RX Beacon mode request
          if( xQueueSend( xQueue_native_park, (void *)&LDM_Msg_v2, ( TickType_t ) 10 ) != pdPASS ) {
            // if(LoRa_debug) debugA("=====QUEUE_NATIVE_PARK===> native_loop: IPC TX request re-enqueueing failed: MsgSqNbr=%d....\r", LDM_Msg_v2.MsgSqNbr );
            // flag lost packet due to IPC overflow
            IPC_lost_msgs++ ;
            } 
          else {
            // if(LoRa_debug) debugA("=====QUEUE_NATIVE_PARK===> native_loop: IPC TX request re-enqueueing success: MsgSqNbr=%d....\r", LDM_Msg_v2.MsgSqNbr );
            };                     
          };
        }   // end of if(LDM_Msg_v2.MsgType == 6 ) 

      else if(LDM_Msg_v2.MsgType == 7 ){   // schedule sending of aprs periodic beacon 
        // if(LoRa_debug) debugA("native_loop-MSG_IN-7: LoRa exit RX Beacon mode request received via IPP MsgSqNbr =%d - dueTime=%ld .... \r", LDM_Msg_v2.MsgSqNbr , LDM_Msg_v2.dueTime  ); 
        if( ((LDM_Msg_v2.dueTime == 0.0 ) || (millis() > LDM_Msg_v2.dueTime))){
          if(LoRa_debug) debugA("native_loop-MSG_IN-7: EXEC LoRa exit RX Beacon mode MsgSqNbr =%d - dueTime=%ld .... \r", LDM_Msg_v2.MsgSqNbr , LDM_Msg_v2.dueTime  ); 
            String time_now =  Italy.dateTime("Ymd-H:i:s");
            // if(LoRa_debug) 
            debugA("\r----[%s]-----> native_loop: LDH_SetLoraConfig (exit RX Beacon mode) \r\r\n\r\n", time_now.c_str());        
            if(syslog_available && BeaconEngineType == 0 ) syslog.logf(LOG_INFO, "Sched sending aprs beacon at [%s]", time_now.c_str() );
            if(syslog_available && BeaconEngineType == 1 ) syslog.logf(LOG_INFO, "Sched sending aprs beacon at [%s]", time_now.c_str() );
            requirePeriodicBeacon = true; // request to send aprs periodic beacon 
            if(LDM_Msg_v2.ReschedTime){     // support for periodic rescheduling feature
              MsgStdSeqNbr++; LDM_Msg_v2.MsgSqNbr = MsgStdSeqNbr ;
              LDM_Msg_v2.dueTime = LDM_Msg_v2.dueTime + LDM_Msg_v2.ReschedTime ;
              // packet scheduled for later time transmission...  re-enqueue transmission request
              if( xQueueSend( xQueue_native_park, (void *)&LDM_Msg_v2, ( TickType_t ) 10 ) != pdPASS ) {
                // if(LoRa_debug) debugA("=====QUEUE_NATIVE_PARK - RESCHEDULE BLOCKING FAULT===> native_loop - reschedule : IPC TX request re-enqueueing failed: MsgSqNbr=%d....\r", LDM_Msg_v2.MsgSqNbr );
                // flag lost packet due to IPC overflow
                IPC_lost_msgs++ ;
                } 
              else {
                // if(LoRa_debug) debugA("=====QUEUE_NATIVE_PARK===> native_loop - reschedule: IPC TX request re-enqueueing success: MsgSqNbr=%d....\r", LDM_Msg_v2.MsgSqNbr );
                };     
              };  // end of packet rescheduling
            }
          else {
            // packet scheduled for later time transmission...  re-enqueue send periodic baprs beacon 
            if( xQueueSend( xQueue_native_park, (void *)&LDM_Msg_v2, ( TickType_t ) 10 ) != pdPASS ) {
              // if(LoRa_debug) debugA("=====QUEUE_NATIVE_PARK===> native_loop: IPC TX request re-enqueueing failed: MsgSqNbr=%d....\r", LDM_Msg_v2.MsgSqNbr );
              // flag lost packet due to IPC overflow
              IPC_lost_msgs++ ;
              } 
            else {
              // if(LoRa_debug) debugA("=====QUEUE_NATIVE_PARK===> native_loop: IPC TX request re-enqueueing success: MsgSqNbr=%d....\r", LDM_Msg_v2.MsgSqNbr );
              };                     
            };
          }   // end of if(LDM_Msg_v2.MsgType == 7)  
        };

      // Receive a message from the native queue.  Block for 10 ticks if a message is not immediately available.
      LDH_Message_v2 LDM_Msg_RX_v2 ;
      if (xQueueReceive(xQueue_native, (void *)&LDM_Msg_RX_v2, ( TickType_t ) 10 ) == pdTRUE) {  // wait  10 secs
        // debugA("native_loop-MSG_RX-2: bufData=[%s] - bufLen=%d - SigRep=[%s]\r", LDM_Msg_RX_v2.bufData ,LDM_Msg_RX_v2.bufLen ,LDM_Msg_RX_v2.SigRep  );
        // debugA("native_loop-MSG_RX-2: ack=%d\r", LDM_Msg_RX_v2.ack);
        if(LDM_Msg_RX_v2.ack == 1) {
          // debugA("native_loop-MSG_RX-2: ack=%d =============================================> New LoRa Packet received\r", LDM_Msg_RX_v2.ack);
          // onLoraNativeDataAvailable(LDM_Msg_RX_v2.bufData, LDM_Msg_RX_v2.bufLen, LDM_Msg_RX_v2.SigRep, "Q_Rx");   // process new LoRa packet received
          onLoraNativeDataAvailable(LDM_Msg_RX_v2.bufData, LDM_Msg_RX_v2.bufLen, LDM_Msg_RX_v2.SigRep, LDM_Msg_RX_v2.SigRep);   // process new LoRa packet received
          } 
        else {
          // debugA("native_loop-MSG_RX-5: ack=%d ==> invalid data received\r", LDM_Msg_RX_v2.ack);
          };
        }    

  vTaskDelay(pdMS_TO_TICKS(50));
  // debugA("end of native_loop");
} // end of native_loop


void onLoraNativeDataAvailable( byte *rxBuf_BCN, const int packetSize, String signalReport , String my_spot){
  float frequencyError ; 
  rxBuf_BCN[packetSize + 1] = '\0'; 
  //  if(LoRa_debug) debugA("onLoraNativeDataAvailable: Disposition => new Native LoRa packet received \r");  
  //  if(LoRa_debug)  
  //   debugA("**************onLoraNativeDataAvailable********************=> bufData=[%s] - RX_packetSize=%d - signalReport=[%s] - tbd=[%s]\r", rxBuf_BCN, packetSize , signalReport.c_str() , my_spot.c_str());
  if(LoRa_debug) debugA("**************onLoraNativeDataAvailable********************=> bufData=[...] - RX_packetSize=%d - signalReport=[%s] - tbd=[%s]\r" , packetSize , signalReport.c_str() , my_spot.c_str());
  LoRa_rx_packets++;
  rxBuf_BCN[packetSize + 1] = '\0'; 
  int i=0;
  bool local_is_CRC_Errored = false ;
  int posCRC_Errored = signalReport.indexOf("CRC_Errored"); 
  if(posCRC_Errored > 0) {local_is_CRC_Errored = true;};
  // let this packet be reported anyway ... it could be a CRC_Errored packet
  String textPayload = BeaconDecode( rxBuf_BCN, packetSize) ;
  if(LoRa_debug) debugA("onLoraNativeDataAvailable:===> textPayload=[%s]\r", textPayload.c_str());
  i=0; while(i < packetSize ){if(! (i%16))debugAl("\r")  ; debugAl("%02X ", rxBuf_BCN[i]); i++;   }; debugAl("\r");
   
  // send_native_log_msg(textPayload, signalReport, my_spot);   // send to spot log server tag 1     
  if(rxBuf_BCN[0] == '<'){ 
    if(LoRa_debug) debugA("onLoraNativeDataAvailable: Found possibly CRC_Errored OE_APRS_Packet: payload = [%s]\r", (char *)rxBuf_BCN );    
    send_native_log_msg(textPayload, signalReport, my_spot);   // send to spot log server tag 1
    }   
  else if(rxBuf_BCN[0] == '$'){ 
    if(LoRa_debug) debugA("onLoraNativeDataAvailable: Found possibly CRC_Errored ShortBeaconPacket: payload = ");
    send_native_log_msg(textPayload, signalReport, my_spot);   // send to spot log server tag 1
    }
  else if ((rxBuf_BCN[0] == '#') && (! local_is_CRC_Errored) ){ 
    send_native_log_msg(textPayload, signalReport, my_spot);   // send to spot log server tag 1
    if(LoRa_debug) debugA("onLoraNativeDataAvailable: Found Magic packet: payload = ");
    i=0; while(i < packetSize ){if(! (i%16))debugAl("\r")  ; debugAl("%02X ", rxBuf_BCN[i]); i++;   }; debugAl("\r");
    // just reset the node by HW
    if(reboot_pin){  
      RebootTag = 123 ;
      fram.write8(2015,RebootTag);
      vTaskDelay(pdMS_TO_TICKS(50));
      hard_reboot();
      // pinMode(reboot_pin, OUTPUT);      // reset for LoRa 
      // digitalWrite(reboot_pin, LOW);
      }
    else{   
      ESP.restart();
      };
    }     
  else{
    if(LoRa_debug) debugA("onLoraNativeDataAvailable: Found Unknown packet type: payload = ");
    i=0; while(i < packetSize ){if(! (i%16))debugAl("\r\n")  ; debugAl("%02X ", rxBuf_BCN[i]); i++;   }; debugAl("\r\n");
    send_native_log_msg(textPayload, signalReport, my_spot);   // send to spot log server tag 1    
    // debugA("onLoraNativeDataAvailable: ===> rssi=%8.2f - lora_snr=%8.2f - freq_err=%8.2f\r\n", lora_rssi, lora_snr, frequencyError);    
    };   
}  // end of  onLoraNativeDataAvailable


void sendMagicPacket(){
  debugA("sendMagicPacket: passing checkpoint...\r ");
  long currentMs = millis();
  byte LoRa_buf[102];

  // if (previousShortBeaconMs == 0 || currentMs - previousShortBeaconMs >= aprsShortBeaconPeriodSeconds  * 1000 || requirePeriodicBeacon ) {
  requirePeriodicShortBeacon = false;
  //   sprintf((char *)LoRa_buf, "$IQ8SO EF34je27");
  sprintf((char *)LoRa_buf, "#123");

  uint8_t EncapType =  0; // native packet no encapsulation
  // LDH_sendNativeToLora(LoRa_buf, sizeof(LoRa_buf, EncapType, false , false  ));
  // LDH_sendNativeToLora_noCAD(LoRa_buf, 15, EncapType , false, false );
  LDH_sendNativeToLora_noCAD(LoRa_buf, 8, EncapType , false, false  );
     
  // previousShortBeaconMs = currentMs;
  return ;
}


uint32_t GetNextSlice( uint32_t Interval, uint32_t Offset){
  uint32_t timeNow = UTC.now(); 
  uint32_t timeSlice = timeNow  - timeNow%Interval ;  // timeslice is the last n-sec boundary
  uint32_t schedTime = Offset * 1000 + millis() + (timeSlice + Interval - timeNow)*1000 -ms() + 35;
  if(LoRa_debug) debugA("----> GetNextSlice: ===> Interval=%d -  Offset=%d - timeNow=%d - timeSlice=%d - millis()=%d - schedTime=%d \r ", Interval, Offset , timeNow, timeSlice,millis(),schedTime );
  return(schedTime);
}



void sendNativePeriodicShortBeacon(void){
  long currentMs = millis();
  byte LoRa_buf[102];
  BeaconRunTimeConfig BCN_cfg_single  ;
  uint8_t EncapType  = 0; // native payload no encapsulation
  if (previousShortBeaconMs == 0 || currentMs - previousShortBeaconMs >= aprsShortBeaconPeriodSeconds  * 1000 || requirePeriodicBeacon ) {
    requirePeriodicShortBeacon = false;
    sprintf((char *)LoRa_buf, "$IQ8SO EF34je27");
    //uint32_t schedTime = GetNextSlice(30, 0);
    uint32_t schedTime = GetNextSlice(BCN_TimeSlot[BCN_TimeSlotSync], BCN_TimeSlotOffset);
    //   LDH_sendNativeToLora(LoRa_buf, sizeof(LoRa_buf), EncapType, false , false);
    //     LDH_sendNativeToLora(LoRa_buf, 15, EncapType , false );
    //     LDH_sendNativeToLora_noCAD(LoRa_buf, 15, EncapType , true , false );
    //     loraPrsService.SchedNativeToLora_noCAD(rxBuf, rxBufIndex, Encap, 0 ,0 , NULL);
    loraPrsService.SchedNativeToLora_noCAD(LoRa_buf, 15 , EncapType, schedTime , BCN_TimeSlot[BCN_TimeSlotSync] * 1000 , BCN_cfg_single, false ) ;
    previousShortBeaconMs = currentMs;
    return ;
    };
}

