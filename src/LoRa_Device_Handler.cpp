// LoRa_Device_Handler.cpp
#include "LoRa_Device_Handler.h"

extern LoraPrs::Service loraPrsService;  // this is the manager ol the LORA_APRS application
extern volatile uint16_t state ;

// following function is called when a complete packet
// is transmitted or received by the LDH module
// IMPORTANT: this function MUST be 'void' type
// and MUST NOT have any arguments!
void IRAM_ATTR  setFlag(void) {
  // check if the interrupt is enabled
  if(!enableInterrupt) {
    return;
    };
  // we sent a packet, set the flag to RX or TX event based on rtxFlag
  if(xSemaphoreTakeFromISR(LoRa_Device_Handler_mutex_v, NULL) ){ 
    if(rtxFlag == 1){
      receivedFlag = true;
      }
    else if (rtxFlag == 2){
      transmittedFlag = true;
      };
    xSemaphoreGiveFromISR(LoRa_Device_Handler_mutex_v, NULL);  // release mutex    
    };
} // end of setFlag


bool Setup_LDH_IPP(void) {
  xQueue_in = xQueueCreate( 10, sizeof(LDH_Message_v2) );    
  if( xQueue_in == NULL ){
    if(LoRa_debug) debugA("Setup_LDF_IPP: failed to create xQueue_in: aborting\r");    // start listening for LoRa packets
    return(false);
    };
  // Create a queue for LDH output messages
  xQueue_APRS = xQueueCreate( 10, sizeof( LDH_Message_v2) );
  if( xQueue_APRS == NULL ){
    if(LoRa_debug) debugA("Setup_LDF_IPP: failed to create xQueue_APRS: aborting\r");    // start listening for LoRa packets
    return(false);
    };
  // Create a queue for LDH output messages
  xQueue_native = xQueueCreate( 10, sizeof( LDH_Message_v2) );
  if( xQueue_native == NULL ){
    if(LoRa_debug) debugA("Setup_LDF_IPP: failed to create xQueue_native: aborting\r");    // start listening for LoRa packets
    return(false);
    };
  // Create a queue for APRS LoRa messages  parking for transmission      
  xQueue_park = xQueueCreate( 30, sizeof(LDH_Message_v2) );    
  if( xQueue_park == NULL ){
    if(LoRa_debug) debugA("Setup_LDF_IPP: failed to create xQueue_park: aborting\r");    // start listening for LoRa packets
    return(false);
    };
      
  // Create a queue for native LoRa messages  parking for transmission      
  xQueue_native_park = xQueueCreate( 30, sizeof(LDH_Message_v2) );    
  if( xQueue_native_park == NULL ){
    if(LoRa_debug) debugA("Setup_LDF_IPP: failed to create xQueue_native_park: aborting\r");    // start listening for LoRa packets
    return(false);
    };
      
  return(true);
}


bool LDH_SetLoraConfig(float loraFreq, float bw, byte sf, byte cr, int pwr, byte sync, byte prlen , String lora_mod_type){
  LastFunction = __func__ ;
  // this function will drive the LoRa device on board ( just one ) according to input parameters
  if(LoRa_debug) debugA("LDH_SetLoraConfig starting.... (LDH_Message_v2 size=%d)\r",sizeof( struct LDH_Message_v2 ) );
  //  vTaskDelay(pdMS_TO_TICKS(20000)); // for testing only

  if(LoraDeviceType == 1){  // second  generation lora chips
                                  // initializzation depends on chips 1st or 2nd generation and module type
                                  // and on specific device chip for the module
    if( ! strcmp(LoRaDevice.c_str() , "E22_400M30S")) {
      if(LoRa_debug) debugA("[SX1268 - E22_400M30S] Initializing: loraFreq=%f - bw=%f - sf=%d - cr=%d - sync=%02X - pwr=%d - prlen=%d....", loraFreq, bw, sf, cr,sync,(int )pwr , prlen);
      // int state = lora_2g.begin(433.725, 15.6, 9, 7,0x34,-9,15,0);  
      // int state = lora_2g.begin(433.725, 31.25, 7, 8, 0x34, -9, 15);  // example
      // int state = lora_2g.begin(loraFreq, bw , sf , cr , sync, pwr , prlen);
      int state = lora_2g.begin(loraFreq, bw , sf , cr , sync, pwr , prlen, 2.4, false );  // by MF 20220609 eByte   
      if (state == RADIOLIB_ERR_NONE) {   // state was passed by the .begin() above
        if(LoRa_debug) debugA("success!\r");
        LoRaFreqNow = loraFreq ;
        }
      else {
        if(LoRa_debug) debugA("failed, code %d: Aborting... please fix and reboot  3 !\r ",state );
        return (false) ;
        };
      lora_2g.setRfSwitchPins(16, 17);
      // eByte E22-900M22S uses DIO3 to supply the external TCXO

      if (lora_2g.setTCXO(2.4) == RADIOLIB_ERR_INVALID_TCXO_VOLTAGE){ if(LoRa_debug) debugA("\rSelected TCXO voltage is invalid for this module!\r");} ;
      lora_2g.setCurrentLimit(140.0);
      if (lora_2g.setOutputPower((int)pwr) != RADIOLIB_ERR_NONE){ if(LoRa_debug) debugA("\rSelected outputPower is invalid for this module!\r");};
      debugA("\rgetCurrentLimit() = %f \r",lora_2g.getCurrentLimit());
      // lora.setTCXO(2.4);
      // lora.setDio2AsRfSwitch(true);
      // pinMode(RADIO_TXEN, OUTPUT);
      // pinMode(RADIO_RXEN, OUTPUT);
      // rxEnable();
      // setup rx interrupt support
      lora_2g.setDio1Action(setFlag);
      // fixed:  PreambleLength=15, RXgain=0

      if(LoRa_debug) debugA("[SX1268 - E22_400M30S] Starting to listen ... ");    // start listening for LoRa packets
      state = lora_2g.startReceive();
      if (state == RADIOLIB_ERR_NONE) {
        if(LoRa_debug) debugA("success!\r");
        }     
      else {
        if(LoRa_debug) debugA("failed, code %d: Aborting... please fix and reboot 4 !\r ",state );
        return (false) ;
        };
      // additional device setup features
      lora_2g.setSyncWord(sync);
      lora_2g.setSpreadingFactor(sf);
      lora_2g.setBandwidth(bw);
      lora_2g.setCodingRate(cr);
      lora_2g.setOutputPower((int)pwr);
      lora_2g.setPreambleLength(prlen);
      //  lora_2g.setGain(0);
      //  if(sf > 10) lora_2g.forceLDRO(true);
      //  lora_2g.forceLDRO(false);
      lora_2g.autoLDRO() ;
      lora_2g.setCRC(true);
      } 

    else if (! strcmp(LoRaDevice.c_str(),"RA_01S")) {
      if(LoRa_debug) debugA("[SX1268 - RA_01S] Initializing: loraFreq=%f - bw=%f - sf=%d - cr=%d - sync=%02X - pwr=%d - prlen=%d....", loraFreq, bw, sf, cr,sync,(int )pwr , prlen);
      int state = lora_2g.begin(loraFreq, bw , sf , cr , sync, pwr , prlen, 0, true );  // by MF 20221207  RA-01S
      if (state == RADIOLIB_ERR_NONE) {   // state was passed by the .begin() above
        if(LoRa_debug) debugA("success!\r");
        LoRaFreqNow = loraFreq ;  
        }
      else {
        if(LoRa_debug) debugA("failed, code %d: Aborting... please fix and reboot 5 !\r ",state );
        return (false) ;
        };
      lora_2g.setRfSwitchPins(16, 17);   
      if (lora_2g.setOutputPower((int)pwr) != RADIOLIB_ERR_NONE){ if(LoRa_debug) debugA("\rSelected outputPower is invalid for this module!\r");};
      debugA("\rgetCurrentLimit() = %f \r",lora_2g.getCurrentLimit());
      // setup rx interrupt support
      lora_2g.setDio1Action(setFlag);
      if(LoRa_debug) debugA("[SX1268 - RA_01S] Starting to listen ... ");    // start listening for LoRa packets
      state = lora_2g.startReceive();
      if (state == RADIOLIB_ERR_NONE) {
        if(LoRa_debug) debugA("success!\r");
        }     
      else {
        if(LoRa_debug) debugA("failed, code %d: Aborting... please fix and reboot 6 !\r ",state );
        return (false) ;
        };
    
      // additional device setup features
      lora_2g.setSyncWord(sync);
      lora_2g.setSpreadingFactor(sf);
      lora_2g.setBandwidth(bw);
      lora_2g.setCodingRate(cr);
      lora_2g.setOutputPower((int)pwr);
      lora_2g.setPreambleLength(prlen);
      lora_2g.autoLDRO() ;
      lora_2g.setCRC(true);
      };
  }
         // end of 2nd generation chips then  first generation chips
else {
  if(! strcmp(LoRaDevice.c_str(), "RFM98")) {
    debugA("[SX1278 - RFM98] Initializing: loraFreq=%f - bw=%f - sf=%d - cr=%d - sync=%02X - pwr=%d ....", loraFreq, bw, sf, cr, sync, (int)pwr);
    // int state = lora_1g.begin(433.725, 15.6, 9, 7,0x34,16,15,0);
    // int state = lora_1g.begin(loraFreq, bw, sf, cr,sync,(int)pwr,prlen,0);
    // int state = lora_1g.begin(loraFreq, bw , sf , cr , sync, pwr , prlen,0);
    if (pwr >17 ) pwr=17 ;
    if( (ESP_Config.cpu_type == "TTGO") ||  (ESP_Config.cpu_type == "HELTEC")  )  initPMU();
    int state = lora_1g.begin(loraFreq, bw , sf , cr , sync, (int)pwr , prlen, 0 );
    if (state == RADIOLIB_ERR_NONE) {
      LoRaChipVersion = lora_1g.getChipVersion();
      LoRaFreqNow = loraFreq ;
      debugA("success (LoRaChipVersion=%d)!\r", LoRaChipVersion);
      }
    else {
      debugA("failed, code %d: Aborting... please fix and reboot 1 !\r ",state );
      return(false);
      };
//    lora_1g.setDio0Action(setFlag, 1 );          // 20230508          
    lora_1g.setDio0Action(setFlag, RISING);
                                    
    debugA("[SX1278 - RFM98] Starting to listen ... ");  // start listening for LoRa packets
    state = lora_1g.startReceive();
    if (state == RADIOLIB_ERR_NONE) {
      debugA("success!\r");
      }    
    else {
      debugA("failed, code %d: Aborting... please fix and reboot 2 !\r ",state );
      return(false);
      };

    lora_1g.setSyncWord(sync);
    lora_1g.setSpreadingFactor(sf);
    lora_1g.setBandwidth(bw);
    lora_1g.setCodingRate(cr);
    // lora_1g.setOutputPower((int)pwr);
    lora_1g.setOutputPower(pwr);
    lora_1g.setPreambleLength(prlen);
    lora_1g.setGain(0);
    //  if(sf > 10) lora_1g.forceLDRO(true);
    //  lora_1g.forceLDRO(false);
    //  lora_1g.autoLDRO() ;
    lora_1g.setCRC(true);
    };   // end of RFM98
  };  // end of first generation chips

  if(xSemaphoreTake(LoRa_Device_Handler_mutex_v,  ( TickType_t ) 2 ) ){ 
    receivedFlag = false;
    transmittedFlag = false;
    rtxFlag = 1;
    enableInterrupt = true;
    xSemaphoreGive(LoRa_Device_Handler_mutex_v);
    } 
  else {
    return(false);
    };
  //  RED_LED_disable();     
  debugA("LDH_SetLoraConfig: LoRa device setup OK: up & running! (enableInterrupt = true)\r");
  return(true);
}

//************************* LDH_sendNativeToLora_noCAD **************************************************
// Starts transmission of a native LoRa packet by using CAD procedure
// Could be used by any other function inside the main arduino task/loop only
// not thread safe 
// Input is a packet buffer with explicit length indication
// Input must specify Encapsulation type for statistcs purposes and for rescheduling purposes
// Input can request automatic packet transmission rescheduling in case X is unavailable at moment
// Rescheduled packets are directed to APRS park queue for APRS traffic ( Encap= 1,2) or to native park queue for Encap=0 
// This is the only function to use to start a packet transmission with noCAD
//
// This _noCAD version has retrigger feature to allow to automagically reschedule a packet trasmission
// after a defined period of time ( in msecs )
//


bool LDH_sendNativeToLora_noCAD(byte *rxBuf, int rxBufIndex, uint8_t Encap , bool NoShed , bool SkipIfNeeded) {
  LastFunction = __func__ ;
  byte LoRa_buf[512];
  // if(LoRa_debug) debugA("sendNativeToLora_noCAD: passing checkpoint... rxBufIndex=%d\r", rxBufIndex);
#ifdef TX_DISABLE
  return true ;
#endif

  if ((rtxFlag == 2)|| (LDH_RX_Mode == 1 ) && ( LoraDeviceType == 1 ) ){  // check if we are able to transmit a new LoRa packet
    BeaconRunTimeConfig BCN_cfg_single  ;
    if( ! NoShed && loraPrsService.SchedNativeToLora_noCAD(rxBuf, rxBufIndex, Encap, 0 , 0 , BCN_cfg_single, SkipIfNeeded )) return true;  // try to enqueue packet in parking queue
    LoRa_lost_packets++;    // increment lost packets count
    if(LoRa_debug) debugA("sendNativeToLora_noCAD: unable to send new packet, device is busy and xQueue_park is full: LoRa_lost_packets=%d  rtxFlag=%d\r",LoRa_lost_packets,rtxFlag );
    return false;  
    };
  
  int bytesWritten = rxBufIndex ;
  if (bytesWritten <= 0) {
    if(LoRa_debug)debugA("sendNativeToLora_noCAD: invalid packet content\n");
    return false;
    };
  if(LoRa_debug) debugA("sendNativeToLora_noCAD: bytesWritten = %d\n", bytesWritten);
  
  if(xSemaphoreTake(LoRa_Device_Handler_mutex_v,  ( TickType_t ) 2 ) ){ 
    enableInterrupt = false ;
    transmittedFlag = false;
    receivedFlag = false;
    rtxFlag = 2;   // 2--> we are transmitting
    xSemaphoreGive(LoRa_Device_Handler_mutex_v);
    }
  else {
    return(false);
    };

  uint8_t retryCAD = 0;  bool sendSuccess = false;
  time_t loraTime = millis();
  if(LoraDeviceType == 0){  // first generation lora chips
    int state = lora_1g.startTransmit(rxBuf, rxBufIndex );
    }
  else {
    int state = lora_2g.startTransmit(rxBuf, rxBufIndex );
    };

  if (state == RADIOLIB_ERR_NONE){
    String time_now =  Italy.dateTime("Ymd-H:i:s");
    //  debugA("\r----[%s]-----> sendNativeToLora_noCAD: startTransmit entered (pkt_len = %d )\r", time_now.c_str(), rxBufIndex);        
    //   if(LoRa_debug) debugA("----[%s]-------> sendNativeToLora_noCAD: startTransmit entered at %d millis\r", time_now.c_str(), loraTime);        
    sendSuccess = true;
    LoRa_tx_packets++;
    if(Encap == 0 ) LoRa_tx_native_packets++ ;         
    if(Encap == 1 ) LoRa_tx_AX25_packets++ ;
    if(Encap == 2 ) LoRa_tx_OEStyle_packets++ ;
    if(xSemaphoreTake(LoRa_Device_Handler_mutex_v,  ( TickType_t ) 2 ) ){ 
      rtxFlag = 2;   // 2--> we are transmitting
      transmittedFlag = false;
      receivedFlag = false;
      enableInterrupt = true ;
      xSemaphoreGive(LoRa_Device_Handler_mutex_v);
      }
    else {
      return(false);
      };

    RED_LED_enable();
    return true;                 // we entered tx mode waiting for end of transmission via interrupt
    };      
  //  if(LoRa_debug) debugA("--> sendNativeToLora_noCAD: Channel is too busy, give up (retryCAD=%d)\n",retryCAD);
  LoRa_lost_packets++;    // increment lost packets count
  if(LoRa_debug) debugA("sendNativeToLora_noCAD: unable to send new packet: state=%d - LoRa_lost_packets=%d\r", state ,LoRa_lost_packets);

  // transmission is not possible re-enter rx mode
  if(LoraDeviceType == 0){  // first generation lora chips
    lora_1g.startReceive();
    }
  else{
    lora_2g.startReceive();
    };
  if(xSemaphoreTake(LoRa_Device_Handler_mutex_v,  ( TickType_t ) 2 ) ){ 
    receivedFlag == false;
    // transmittedFlag = false;
    rtxFlag = 1;   // 1--> we are receiving
    enableInterrupt = true ;
    xSemaphoreGive(LoRa_Device_Handler_mutex_v);
    }
  else {
    return(false);
    };  
  return false;   // packet transmission failed
}

//      LDM_Msg_v2.dueTime = millis() +(random(0, 10)* 1500 ;  // delay trasmission by 1000 msecs

bool LDH_Ran_sendNativeToLora(byte *rxBuf, int rxBufIndex, uint8_t Encap, bool NoShed , bool SkipIfNeeded) {
  LastFunction = __func__ ;
  if( (! NoShed )&& loraPrsService.SchedNativeToLora(rxBuf, rxBufIndex, Encap, 0 , SkipIfNeeded)) return true;  // try to enqueue packet in parking queue
  LoRa_lost_packets++;    // increment lost packets count
  //  if(LoRa_debug) 
  debugA("LDH_Ran_sendNativeToLora: unable to send new packet, device is busy and xQueue_park is full: LoRa_lost_packets=%d  rtxFlag=%d  NoShed=%d SkipIfNeeded=%d  \r",LoRa_lost_packets,rtxFlag, NoShed , SkipIfNeeded   );
  return false;  
}

bool LDH_Ran_sendNativeToLora_noCAD(byte *rxBuf, int rxBufIndex, uint8_t Encap , bool NoShed , bool SkipIfNeeded ) {
  LastFunction = __func__ ;
  BeaconRunTimeConfig BCN_cfg_single  ;
  if( ! NoShed && loraPrsService.SchedNativeToLora_noCAD(rxBuf, rxBufIndex, Encap , 0, 0 , BCN_cfg_single, SkipIfNeeded)) return true;  // try to enqueue packet in parking queue
  LoRa_lost_packets++;    // increment lost packets count
  if(LoRa_debug) debugA("LDH_Ran_sendNativeToLora_noCAD: unable to send new packet, device is busy and xQueue_park is full: LoRa_lost_packets=%d  rtxFlag=%d\r",LoRa_lost_packets,rtxFlag );
  return false;  
}

//************************* LDH_sendNativeToLora **************************************************
// Starts transmission of a native LoRa packet by using CAD procedure
// Could be used by any other function inside the main arduino task/loop only
// not thread safe 
// Input is a packet buffer with explicit length indication
// Input must specify Encapsulation taype for statistcs purposes and for rescheduling purposes
// Input can request automatic packet transmission rescheduling in case X is unavailable at moment
// Rescheduled packets are directed to APRS park queue for APRS traffic ( Encap= 1,2) or to native park queue for Encap=0 
// This is tne only function to use to start a packet transmission with CAD
//

bool LDH_sendNativeToLora(byte *rxBuf, int rxBufIndex, uint8_t Encap , bool NoShed , bool SkipIfNeeded) {
  LastFunction = __func__ ;

#ifdef TX_DISABLE
  return (true);
#endif

  if( LoraDeviceType == 0 ){ // for 1st gen LoRa chips do not use CAD...
    bool xret = LDH_sendNativeToLora_noCAD(rxBuf,rxBufIndex, Encap, NoShed , false ); 
    return xret ; 
    }; 

#ifdef force_CAD_disable   
  // workaround for montechiaro noCAD !!!!!! 
  bool xret = LDH_sendNativeToLora_noCAD(rxBuf,rxBufIndex, Encap, NoShed , false ); 
  return xret ; 
#endif

  byte LoRa_buf[512];
  if ( (LDH_RX_Mode == 1) ||  (rtxFlag == 2) ){  // check if we are able to transmit a new LoRa packet
    if( ! NoShed && loraPrsService.SchedNativeToLora(rxBuf, rxBufIndex, Encap, 0 , SkipIfNeeded)) return true;  // try to enqueue packet in parking queue
    LoRa_lost_packets++;    // increment lost packets count
    if(LoRa_debug) debugA("sendNativeToLora: unable to send new packet, device is busy or xQueue_park is full: LoRa_lost_packets=%d  rtxFlag=%d\r",LoRa_lost_packets,rtxFlag );
    return false;  
    }; 
    
  if (rxBufIndex <= 0) {
    if(LoRa_debug)debugA("sendNativeToLora: invalid packet content\n");
    return false;
    };

  // debugA("sendNativeToLora: bytesWritten = %d\n", bytesWritten);
  if(xSemaphoreTake(LoRa_Device_Handler_mutex_v,  ( TickType_t ) 2 ) ){ 
    enableInterrupt = false ;
    transmittedFlag = false;
    receivedFlag = false;
    rtxFlag = 2;   // 2--> we are transmitting
    xSemaphoreGive(LoRa_Device_Handler_mutex_v);
    }
  else {
    return(false);
    };

  uint8_t retryCAD = 0;  bool sendSuccess = false;int state; int errorsDetected = 0;
  while ((!sendSuccess) && (retryCAD < 10)){  // was originally 10
    if(LoRa_debug)debugA("--> sendNativeToLora_CAD: check for channel free \r"); 
    time_t loraTime = millis();
    // Check 100ms for an opportunity to send
    if(LoraDeviceType == 0){  // first generation lora chips
      while (( state=lora_1g.scanChannel() != RADIOLIB_CHANNEL_FREE) && (retryCAD++ < 10)) { // was originally 10
        if (state == RADIOLIB_LORA_DETECTED) {
          if(LoRa_debug) debugA("LDH_sendNativeToLora: ===> RADIOLIB_LORA_DETECTED LoRa preamble  detected!\r"); 
          } 
        else if (state == RADIOLIB_CHANNEL_FREE) {
          if(LoRa_debug) debugA("LDH_sendNativeToLora: ===> RADIOLIB_CHANNEL_FREE channel is free!\r");
          } 
        else if (state == 384 ) {
          if(LoRa_debug) debugA("LDH_sendNativeToLora: ===> RADIOLIB_CHANNEL_FREE + RADIOLIB_LORA_DETECTED channel is free!\r");
          }         
        else {
          errorsDetected++;
          if(LoRa_debug) debugA("LDH_sendNativeToLora: ===> failed, other error occurred, code=%d\r",state );
          };  
        vTaskDelay(pdMS_TO_TICKS(100));
        };        // end of while scanChannel loop
      }       
    else{
      while (( state=lora_2g.scanChannel() != RADIOLIB_CHANNEL_FREE) && (retryCAD++ < 10)) { // was originally 10
        if (state == RADIOLIB_LORA_DETECTED) {
          if(LoRa_debug) debugA("LDH_sendNativeToLora: ===> RADIOLIB_LORA_DETECTED LoRa preamble  detected!\r"); 
          } 
        else if (state == RADIOLIB_CHANNEL_FREE) {
          if(LoRa_debug) debugA("LDH_sendNativeToLora: ===> RADIOLIB_CHANNEL_FREE channel is free!\r");
          } 
        else if (state == 384 ) {
          if(LoRa_debug) debugA("LDH_sendNativeToLora: ===> RADIOLIB_CHANNEL_FREE + RADIOLIB_LORA_DETECTED channel is free!\r");
          }         
        else {
          errorsDetected++;
          if(LoRa_debug) debugA("LDH_sendNativeToLora: ===> failed, other error occurred, code=%d\r",state );
          };  
        vTaskDelay(pdMS_TO_TICKS(100));
        };        // end of scanChannel loop
      };

    if(errorsDetected) LoRa_CAD_errors++;       
    if(LoRa_debug)debugA("--> sendNativeToLora_CAD: channel scan finished after %ldms; tried %d times at 100 msec\n", (millis() - loraTime), retryCAD);
    if (retryCAD < 10){   // Channel is free, start sending    was originally 10
      if(LoRa_debug) debugA("LDH_sendNativeToLora: ===> RADIOLIB_CHANNEL_FREE channel is free!\r");
      loraTime = millis();
      if(LoraDeviceType == 0){  // first generation lora chips
        int state = lora_1g.startTransmit(rxBuf, rxBufIndex ); 
        }
      else{
        int state = lora_2g.startTransmit(rxBuf, rxBufIndex ); 
        };  
      if (state == RADIOLIB_ERR_NONE){
        String time_now =  Italy.dateTime("Ymd-H:i:s");
        // if(LoRa_debug)debugA("----[%s]-----> sendNativeToLora: startTransmit entered after %ldms with state %d \r", time_now.c_str(), (millis() - loraTime), state);        
        // debugA("----[%s]-----> sendNativeToLora: startTransmit entered (pkt_len = %d)\r", time_now.c_str(), rxBufIndex);        
        sendSuccess = true;
        LoRa_tx_packets++;
        if(Encap == 0 ) LoRa_tx_native_packets++ ;         
        if(Encap == 1 ) LoRa_tx_AX25_packets++ ;
        if(Encap == 2 ) LoRa_tx_OEStyle_packets++ ;
        if(xSemaphoreTake(LoRa_Device_Handler_mutex_v,  ( TickType_t ) 2 ) ){ 
          rtxFlag = 2;   // 2--> we are transmitting
          transmittedFlag = false;
          receivedFlag = false;
          enableInterrupt = true ;
          xSemaphoreGive(LoRa_Device_Handler_mutex_v);
          }
        else {
          return(false);
          };
        RED_LED_enable();
        return true;                 // we entered tx mode waiting for end of transmission via interrupt
        };
      };  // end of if (retryCAD < 10) 
  };    // end of while sendSuccess ....

  if(LoRa_debug) debugA("--> sendNativeToLora: Channel is too busy, give up (retryCAD=%d)\n",retryCAD);
  LoRa_lost_packets++;    // increment lost packets count
  if(LoRa_debug)  debugA("sendNativeToLora: unable to send new packet: LoRa_lost_packets=%d\r",LoRa_lost_packets);

  // transmission is not possible re-enter rx mode
  if(LoraDeviceType == 0){  // first generation lora chips
    lora_1g.startReceive();
    }
  else{
    lora_2g.startReceive();
    };
  if(xSemaphoreTake(LoRa_Device_Handler_mutex_v,  ( TickType_t ) 2 ) ){ 
    receivedFlag == false;
    //  transmittedFlag = false;
    rtxFlag = 1;   // 1--> we are receiving
    enableInterrupt = true ;
    xSemaphoreGive(LoRa_Device_Handler_mutex_v);
    }
  else {
    return(false);
    };
  
  return false;   // packet transmission failed

} // end of LDH_sendNativeToLora



//************************* LoRa_Device_Handler_setup **************************************************
//   initialize LDH by performing a dummy LDH_SetLoraConfig to check correct LoRa chipset opeation
//   sets LoRa_initialized = true is everything is OK
//

void LoRa_Device_Handler_setup( void ){      //LoRa_Device_Handler: task to handle LoRa device operation
  LastFunction = __func__ ;
  uint8_t LDH_Buffer[255];                     //volatile uint8_t LDH_Buffer[RADIOLIB_SX127X_MAX_PACKET_LENGTH]; 

  int packetSize ;
  LDH_Message_v2 LDM_Msg_v2 ;
  vTaskDelay(15000);             // for testing only
  debugA("LoRa_Device_Handler: starting on core=%d\r",xPortGetCoreID() ); 
  // this task will manage the LoRa device and allow other tasks to send and receive packets via FreeRTOS quees 
  // an input queue wil receive TX LoRa requests or other LDH actions requests and two output queues will 
  // dispatch received packets: one for APRS traffic and another for non APRS traffic (native traffic)

  // LoRa module init allways done with default parameters
  //   if ( ! LDH_SetLoraConfig( loraFreq,  bw,  sf,  cr, pwr, sync,  prlen , lora_mod_type)){
  if ( ! LDH_SetLoraConfig( 433.775,  125,  12,  8,  10,  0x12,  10 , "")){
    debugA("LoRa_Device_Handler: LDH_SetLoraConfig pre-initialization failed LoRa device setup..  will try later....\r");
    }
  else{
    debugA("\rLoRa_Device_Handler: LDH_SetLoraConfig pre-initialization successfull... LoRa device OK!\r");
    };

  BCN_LDH_cfg = BCN_APRS_cfg ;  // set BCN_LDH_cfg        
  //   vTaskDelay(15000);             // for testing only
  LoRa_initialized = true ;      // indicate that we are able to process incoming service requests

}


//************************* LoRa_Device_Handler_loop **************************************************
// Main loop of LDH:
//   scans input queue and executes actions according to received  messages
//   TX path end of transmission management
//   RX path new packet received management; 
//      sends messages to APRS service manager via APRS_Queue and to Native service manager via native_queue 
// 

void LoRa_Device_Handler_loop( void ){      //LoRa_Device_Handler: task to handle LoRa device operation
  LastFunction = __func__ ;
  if(!LoRa_initialized) return ;   
  uint8_t LDH_Buffer[256];                     //volatile uint8_t LDH_Buffer[RADIOLIB_SX127X_MAX_PACKET_LENGTH]; 

  int packetSize ;
  LDH_Message_v2 LDM_Msg_v2 ;
  // we will start now waiting for actions to be performed via the input queue     
  //  for(;;){ 
  // LoRa_Device_Handler_flag = true ;  // 
  // if(LoRa_debug){debugA("LoRa_Device_Handler running on core=%d - LoRa_Device_Handler_flag=%d - localcounter=%d\r",xPortGetCoreID(),LoRa_Device_Handler_flag, counter6); };
  // if(LoRa_debug)debugA("========LDH_LOOP=========> LoRa_Device_Handler: rtxFlag=%d - transmittedFlag = %d - receivedFlag = %d - enableInterrupt = %d -localcounter=%d\n", rtxFlag,  transmittedFlag,receivedFlag,enableInterrupt, counter6 ); 
  // check input queue for any service request; Block for 10 ticks if a message is not immediately available.
  if (xQueueReceive(xQueue_in, (void *)&LDM_Msg_v2, 10) == pdTRUE) {
    //   debugA("LoRa_Device_Handler-MSG_RX-3: SigRep=%s\r",LDM_Msg_v2.SigRep);
    //   debugA("LoRa_Device_Handler-MSG_RX-3: ack=%d\r", LDM_Msg_v2.ack);
    if(LoRa_debug) debugA("LoRa_Device_Handler-MSG_RX-3:   SigRep=[%s] - MsgType=%d -   bufLen =%d - bufData=[%s] - LoraFreq=%8.4f - LoraBw=%8.4f - LoraSf=%d - LoraCodingRate=%d - LoraPower=%d - LoraSync=0x%02X - LoraPreambleLen=%d\r", (char *)LDM_Msg_v2.SigRep ,  LDM_Msg_v2.MsgType ,  LDM_Msg_v2.bufLen , (char *)LDM_Msg_v2.bufData ,(float) LDM_Msg_v2.Lv.LoraFreq, (float )LDM_Msg_v2.Lv.LoraBw  , (byte )LDM_Msg_v2.Lv.LoraSf , (byte)LDM_Msg_v2.Lv.LoraCodingRate,  LDM_Msg_v2.Lv.LoraPower , LDM_Msg_v2.Lv.LoraSync ,  LDM_Msg_v2.Lv.LoraPreambleLen  );

    if(LDM_Msg_v2.MsgType == 1 ){   // LoRa Mode Initialization
      // debugA("LoRa_Device_Handler-MSG_IN: LoraFreq=%8.4f - LoraBw=%8.4f - LoraSf=%d - LoraCodingRate=%d  - LoraPower=%d - LoraSync=%02X - LoraPreambleLen=%d \r", LDM_Msg_v2.Lv.LoraFreq, LDM_Msg_v2.Lv.LoraBw  ,LDM_Msg_v2.Lv.LoraSf , LDM_Msg_v2.Lv.LoraCodingRate ,  LDM_Msg_v2.Lv.LoraPower , LDM_Msg_v2.Lv.LoraSync ,  LDM_Msg_v2.Lv.LoraPreambleLen  ); 
      float LoraFreq_nominal =  LDM_Msg_v2.Lv.LoraFreq * ( 1.0 + (float)(LDM_Msg_v2.Lv.LoraFreqCorr)/10000000.0 );   
      if ( LDH_SetLoraConfig( LoraFreq_nominal, LDM_Msg_v2.Lv.LoraBw  ,LDM_Msg_v2.Lv.LoraSf , LDM_Msg_v2.Lv.LoraCodingRate ,  LDM_Msg_v2.Lv.LoraPower , LDM_Msg_v2.Lv.LoraSync ,  LDM_Msg_v2.Lv.LoraPreambleLen   , "" )){
        // if ( LDH_SetLoraConfig(433.725,  31.25,  7,  7,  22,  0x34,  -9 ,"" )){ // example 
        if(LoRa_debug) debugA("\rLoRa_Device_Handler: LDH_SetLoraConfig successfull ....\r");
        BCN_LDH_cfg = LDM_Msg_v2.Lv ;  // set BCN_LDH_cfg
        BCN_APRS_cfg = BCN_LDH_cfg ;
        // send ACK=1 back via xQueue_APRS
        LDM_Msg_v2.ack = 1 ;  // ACK=1 successfull operation
        if( xQueueSend( xQueue_APRS, (void *)&LDM_Msg_v2, ( TickType_t ) 10 ) != pdPASS ) {
          if(LoRa_debug) debugA("Service::SetLoraConfig: unable to queue message toward LDH xQueue_in: failed LoRa device setup....\r");
          IPC_lost_msgs++ ;
          };
        }
      else {
        if(LoRa_debug) debugA("\rLoRa_Device_Handler: LDH_SetLoraConfig failed to setup LoRa device....\r");
        };  
      } // end of if(LDM_Msg_v2.MsgType == 1 )  
          
    else if(LDM_Msg_v2.MsgType == 3 ){   // packet transmission request auto CAD
      // debugA("LoRa_Device_Handler-MSG_IN: LoRa packet transmission request received via IPP MsgSqNbr =%d - dueTime=%ld .... \r", LDM_Msg_v2.MsgSqNbr , LDM_Msg_v2.dueTime  ); 
      if(((LDM_Msg_v2.dueTime == 0.0 )) && (rtxFlag != 2 ) && (LDH_RX_Mode == 0) ){
        //   if(LoRa_debug) 
        debugA("LoRa_Device_Handler-MSG_IN: EXEC LoRa packet transmission MsgSqNbr =%d - dueTime=%ld .... \r", LDM_Msg_v2.MsgSqNbr , LDM_Msg_v2.dueTime  ); 
        LDH_sendNativeToLora(LDM_Msg_v2.bufData , LDM_Msg_v2.bufLen , (uint8_t)LDM_Msg_v2.SigRep[0] , false , false );  // this calls directly the LDH send function          
        }
      else if( (millis() > LDM_Msg_v2.dueTime) && (rtxFlag != 2 )&& (LDH_RX_Mode == 0) ){
        //   if(LoRa_debug) 
        debugA("LoRa_Device_Handler-MSG_IN: EXEC LoRa packet transmission MsgSqNbr =%d - dueTime=%ld .... \r", LDM_Msg_v2.MsgSqNbr , LDM_Msg_v2.dueTime  ); 
        LDH_sendNativeToLora(LDM_Msg_v2.bufData , LDM_Msg_v2.bufLen , (uint8_t)LDM_Msg_v2.SigRep[0] , true , false  );  // this calls directly the LDH send function          
        }              
      else {
        // packet scheduled for later time transmission...  re-enqueue transmission request
        if( xQueueSend( xQueue_in, (void *)&LDM_Msg_v2, ( TickType_t ) 10 ) != pdPASS ) {
          if(LoRa_debug) debugA("=====LDH_QUEUE_IN===> LoRa_Device_Handler: IPC TX request re-enqueueing failed: MsgSqNbr=%d....\r", LDM_Msg_v2.MsgSqNbr );
          // flag lost packet due to IPC overflow
          IPC_lost_msgs++ ;
          } 
        else {
          if(LoRa_debug) debugA("=====LDH_QUEUE_IN===> LoRa_Device_Handler: IPC TX request re-enqueueing success: MsgSqNbr=%d....\r", LDM_Msg_v2.MsgSqNbr );
          };           
        };
            
      }    // end of if(LDM_Msg_v2.MsgType == 3 )
          
    else if(LDM_Msg_v2.MsgType == 4 ){   // packet transmission request no CAD
      // debugA("LoRa_Device_Handler-MSG_IN: LoRa packet transmission request received via IPP MsgSqNbr =%d - dueTime=%ld .... \r", LDM_Msg_v2.MsgSqNbr , LDM_Msg_v2.dueTime  ); 
      if(((LDM_Msg_v2.dueTime == 0.0 ) || (millis() > LDM_Msg_v2.dueTime)) && (rtxFlag != 2 ) && (LDH_RX_Mode == 0) ){
        //   if(LoRa_debug) 
        debugA("LoRa_Device_Handler-MSG_IN: EXEC LoRa packet transmission MsgSqNbr =%d - dueTime=%ld .... \r", LDM_Msg_v2.MsgSqNbr , LDM_Msg_v2.dueTime  ); 
        LDH_sendNativeToLora_noCAD(LDM_Msg_v2.bufData , LDM_Msg_v2.bufLen , (uint8_t)LDM_Msg_v2.SigRep[0] , false , false );  // this calls directly the LDH send function          
        }
      else if((millis() > LDM_Msg_v2.dueTime) && (rtxFlag != 2 ) && (LDH_RX_Mode == 0) ){
        //   if(LoRa_debug) 
        debugA("LoRa_Device_Handler-MSG_IN: EXEC LoRa packet transmission MsgSqNbr =%d - dueTime=%ld .... \r", LDM_Msg_v2.MsgSqNbr , LDM_Msg_v2.dueTime  ); 
        LDH_sendNativeToLora_noCAD(LDM_Msg_v2.bufData , LDM_Msg_v2.bufLen , (uint8_t)LDM_Msg_v2.SigRep[0] , true , false );  // this calls directly the LDH send function          
        }                
      else {
        // packet scheduled for later time transmission...  re-enqueue transmission request
        if( xQueueSend( xQueue_in, (void *)&LDM_Msg_v2, ( TickType_t ) 10 ) != pdPASS ) {
          if(LoRa_debug) debugA("=====LDH_QUEUE_IN===> LoRa_Device_Handler: IPC TX request re-enqueueing failed: MsgSqNbr=%d....\r", LDM_Msg_v2.MsgSqNbr );
          // flag lost packet due to IPC overflow
          IPC_lost_msgs++ ;
          } 
        else {
          if(LoRa_debug) debugA("=====LDH_QUEUE_IN===> LoRa_Device_Handler: IPC TX request re-enqueueing success: MsgSqNbr=%d....\r", LDM_Msg_v2.MsgSqNbr );
          };            
        };
      };   // end of if(LDM_Msg_v2.MsgType == 4 )  
            
    }       // end of xQueueReceive(xQueue_in....
  //      else{
  //        if(LoRa_debug) debugA("LoRa_Device_Handler-MSG_RX-2:   NO INPUT MESSAGE AVAILABLE\r");
  //        };
           
  // TX path end of transmission management
  if (transmittedFlag == true) {    // atomic flag from ISR indicates a packet transmission ended
    RED_LED_disable(); 
    //  if(LoRa_debug) debugA("\r========TX path=========> LoRa_Device_Handler: New packet trasmitted: transmittedFlag = %d - receivedFlag = %d - enableInterrupt = %d \r", transmittedFlag,receivedFlag,enableInterrupt ); 
    //  restore APRS LoRa RX/TX saved configuration BCN_APRS_cfg to be able to restart LoRa APRS app dinamically
    if(( LDH_RX_Mode == 0) && ! BCN_VectorCmp(BCN_LDH_cfg , BCN_APRS_cfg)){  // rx is in APRS mode so need to restore settings if changed
      BCN_LDH_cfg = BCN_APRS_cfg ;  // set BCN_LDH_cfg
      float LoraFreq_nominal =  BCN_LDH_cfg.LoraFreq * ( 1.0 + (float)(BCN_LDH_cfg.LoraFreqCorr)/10000000.0 );   
      if ( LDH_SetLoraConfig( LoraFreq_nominal, BCN_LDH_cfg.LoraBw  ,BCN_LDH_cfg.LoraSf , BCN_LDH_cfg.LoraCodingRate ,  BCN_LDH_cfg.LoraPower , BCN_LDH_cfg.LoraSync ,  BCN_LDH_cfg.LoraPreambleLen   , "" )){
        // debugA("LoRa_Device_Handler_loop: LDH_SetLoraConfig success: LoraFreq=%8.4f - LoraBw=%8.4f - LoraSf=%d - LoraCodingRate=%d  - LoraPower=%d - LoraSync=%02X - LoraPreambleLen=%d \r", BCN_LDH_cfg.LoraFreq, BCN_LDH_cfg.LoraBw  ,BCN_LDH_cfg.LoraSf , BCN_LDH_cfg.LoraCodingRate ,  BCN_LDH_cfg.LoraPower , BCN_LDH_cfg.LoraSync ,  BCN_LDH_cfg.LoraPreambleLen ); 
        };
      };
    if(xSemaphoreTake(LoRa_Device_Handler_mutex_v,  ( TickType_t ) 2 ) ){ 
      enableInterrupt = false;  // disable the interrupt service routine 
      transmittedFlag = false;  // reset transmittedFlag
      rtxFlag = 1;                // flag that we are entering receive mode
      receivedFlag = false;
      enableInterrupt = true;
      xSemaphoreGive(LoRa_Device_Handler_mutex_v);
      }
    else {
      return;
      };

    if(LoraDeviceType == 0){  // first generation lora chips
      int state = lora_1g.startReceive();   // start reception mode
      }
    else{
      int state = lora_2g.startReceive();   // start reception mode
    };
    if (state == RADIOLIB_ERR_NONE) {
      // if(LoRa_debug) debugA("success!\r");
      }     
    else {
      if(LoRa_debug) debugA("LoRa_Device_Handler: lora_x.startReceive() FAILED, code %d \r ",state );
      };
    // we're ready to receive more packets, enable interrupt service routine
    }; // end of if (transmittedFlag == true) 
    
  // RX path new packet received management  711
  if (receivedFlag == true) {   // atomic flag from ISR indicates a new received packet is available to be retrived 
    // if(LoRa_debug)debugA("======= RX path 2 713  ==========> LoRa_Device_Handler: New packet received: transmittedFlag = %d - receivedFlag = %d - enableInterrupt = %d \r", transmittedFlag,receivedFlag,enableInterrupt ); 
    if(xSemaphoreTake(LoRa_Device_Handler_mutex_v,  ( TickType_t ) 2 ) ){ 
      enableInterrupt = false;   // disable the interrupt service routine while processing the received data
      receivedFlag = false;      // reset receivedFlag
      xSemaphoreGive(LoRa_Device_Handler_mutex_v);
      }
    else {
      return;
      };

    //   uint8_t LDH_Buffer[RADIOLIB_SX127X_MAX_PACKET_LENGTH]; 
    for (int i = 0; i < 255; i++){LDH_Buffer[i]=0;};      // read in received packet
    // put module to standby while reading data  
    int16_t rx_state;
    if(LoraDeviceType == 0){  // first generation lora chips
      lora_1g.standby();
      //   uint16_t rx_state = lora_1g.readData(LDH_Buffer,RADIOLIB_SX127X_MAX_PACKET_LENGTH);
      rx_state = lora_1g.readData(LDH_Buffer,0);  // by MF 20220130     
      }
    else{
      lora_2g.standby();
      //   uint16_t rx_state = lora_2g.readData(LDH_Buffer,RADIOLIB_SX127X_MAX_PACKET_LENGTH);
      rx_state = lora_2g.readData(LDH_Buffer,0);  // by MF 20220130         
      };

    int packetSize =  RADIOLIB_SX127X_MAX_PACKET_LENGTH -2;      // calculate packet length
    while (LDH_Buffer[packetSize] == 0x00 ) { packetSize-- ; }; packetSize++ ;  // zero fill remaining packet content
    if(LoRa_debug) debugA("======= RX path ==========> LoRa_Device_Handler: New packet received: length= %d - rx_state=%d\r", packetSize , rx_state); 

    // following by MF 20220126 OE compatibility
    if ((rx_state == RADIOLIB_ERR_NONE) || ((rx_state == RADIOLIB_ERR_CRC_MISMATCH) )) {  // packet was successfully received but may be errored
      // analyze now received packet and decide what to do with it....
      if(LoRa_debug){ int i=0; while(i < packetSize ){if(! (i%16))debugAl("\n\r")  ; debugAl("%02X ", LDH_Buffer[i]); i++;   }; debugAl("\n\r");};
      bool is_CRC_Errored = false ;
      if((rx_state == RADIOLIB_ERR_CRC_MISMATCH) ) is_CRC_Errored = true ;          
      if(LoRa_debug)debugA("LoRa_Device_Handler: perform received packet report analyses\r");
      // perform received packet report analyses
      float l_snr , rssi ;
      if(LoraDeviceType == 0){  // first generation lora chips
        l_snr = lora_1g.getSNR();
        rssi = lora_1g.getRSSI();
        frequencyError = lora_1g.getFrequencyError();
        lora_frequencyError = frequencyError;
        float ppm_now = frequencyError/LoRaFreqNow ;
        ppm_runtime= (3.0*ppm_runtime + ppm_now )/4.0;
        if(LoRa_debug) debugA("\r===-------=====> LoRaFreqNow=%12.5f - frequencyError=%8.5f - ppm_now=%8.5f  ==> ppm_runtime=%8.5f\r",LoRaFreqNow, frequencyError, ppm_now , ppm_runtime);
        }
      else{
        l_snr = lora_2g.getSNR();
        rssi = lora_2g.getRSSI();
        frequencyError = 0.0;
//        lora_frequencyError = frequencyError;        // replaced 20230507
        frequencyError = lora_2g.getFrequencyError();
        lora_frequencyError = frequencyError;
        float ppm_now = frequencyError/LoRaFreqNow ;
        ppm_runtime= (3.0*ppm_runtime + ppm_now )/4.0;
        if(LoRa_debug) debugA("\r===-------=====> LoRaFreqNow=%12.5f - frequencyError=%8.5f - ppm_now=%8.5f  ==> ppm_runtime=%8.5f\r",LoRaFreqNow, frequencyError, ppm_now , ppm_runtime);
        };    

      lora_snr = l_snr ;
      lora_rssi = (l_snr < 0 ? rssi + l_snr : rssi);
      if(l_snr < 0){
        if( LoRa_ENL == 0.0){LoRa_ENL = rssi;} else { LoRa_ENL = (4*LoRa_ENL + rssi)/5;};
        };
      // we are ready to receive new packets
      receivedFlag == false; // clear receivedFlag
      uint16_t state ;  
      if(LoraDeviceType == 0){  // first generation lora chips
        state = lora_1g.startReceive();
        }
      else{
        state = lora_2g.startReceive();
        } ;
      if (state == RADIOLIB_ERR_NONE) {
        // if(LoRa_debug) debugA("success!\r");
        }     
      else {
        if(LoRa_debug) debugA("LoRa_Device_Handler: lora_x.startReceive() FAILED, code %d \r ",state );
        };  
      enableInterrupt = true;          // enable interrupt service routine

      // let's now assemble RX repeort related to the received packet
      String signalReport ;
      if(LoraDeviceType == 1){  // second  generation lora chips
        signalReport = String(" (") + 
//        String(LoRa_DutyCycle)  +  String(" ") + 
        String(NodeName.c_str()) + String(" ") + 
        String(l_snr < 0 ? int(rssi + l_snr) : int(rssi)) + String(" ") + 
        String(int(l_snr)) + String(" ") +
        String(int(frequencyError)) + String(" ") + 
//        String(cpu_temp) + 
        String(")") ;
        }
      else{           // first generation chips
        signalReport = String(" (") + 
//        String(LoRa_DutyCycle) + 
//        String(" ") + 
        String(NodeName.c_str()) + 
        String(" ") + 
        String(l_snr < 0 ? int(rssi + l_snr) : int(rssi)) + 
        String(" ") + 
        String(int(l_snr)) + 
        String(int(frequencyError)) + String(" ") + 
//        String(" 0 ") +
//        String(" ") + 
//        String(cpu_temp) + 
        String(")") ;
        };

      if(is_CRC_Errored){  // flag in clear that this packet has been received as CRC_Errored
        signalReport = signalReport + String(" ") + String("CRC_Errored") ;
        };
                    
      String my_spot="1609180492|4038.45N|01423.34E|IQ8SOX-10|-127.00|-11.50|-1023.00"; // example
      // my_spot= String(l_snr < 0 ? rssi + l_snr : rssi) + "|" + String(l_snr) + "|" + String(frequencyError)  ;
      my_spot= String(l_snr < 0 ? rssi + l_snr : rssi) + "|" + String(l_snr)   ;

      // try to understand which type of packet it is
      LoRa_rx_packets++ ;  // BTW a packet has been received...  count it!
      
      bool is_AX25 = false ; bool is_OE_Style = false; bool is_native = false ;
      AX25::Payload payload(LDH_Buffer, packetSize);  // assume to be an AX.25 payload and check for validity
      if (payload.IsValid()) {  //  it is an AX25 APRS packet
        is_AX25 = true ;
        LoRa_rx_AX25_packets ++;
        if(LoRa_debug) debugA("LoRa_Device_Handler: found possible APRS AX25 payload (..may be errored..)!\r");
        }
      else if ( (LDH_Buffer[0] == '<') && (LDH_Buffer[1] == 0xFF) && (LDH_Buffer[2] == 0x01)  ) {     //  APRS_OE_Style LoRa packet
        is_OE_Style = true ;
        LoRa_rx_OEStyle_packets++;
        if(LoRa_debug) debugA("LoRa_Device_Handler: found possible APRS is_OE_Style payload (..may be errored..)!\r");
        }
      else {
        is_native = true ; 
        LoRa_rx_native_packets++;
        if(LoRa_debug) debugA("LoRa_Device_Handler: found possible native lora packet (..may be errored..)!\r");
        };
         
         
      // send received packet to APRS_service_handler or native_service_handler
      signalReport.toCharArray( LDM_Msg_v2.SigRep ,signalReport.length()+1 );  // fill Signaleport
      LDM_Msg_v2.ack = 1 ; // indicates a successfully received packet      
      memcpy(LDM_Msg_v2.bufData, LDH_Buffer, sizeof(LDH_Buffer[0])*packetSize);   
      LDM_Msg_v2.bufLen = packetSize ;  // fill RX packet size
      LDM_Msg_v2.bufData[packetSize+1]= '\0';
      // send LDM_Msg_v2 to APRS_Manager for higher level processing 
      if(LoRa_debug) debugA("LoRa_Device_Handler: sending packet via xQueue_APRS or xQueue_native ....(is_AX25=%d - is_OE_Style=%d - is_native=%d - is_CRC_Errored=%d)\r", is_AX25 , is_OE_Style,is_native, is_CRC_Errored );        
      // if (( is_AX25 || is_OE_Style) && (! is_CRC_Errored))  {
      if (( is_AX25 || is_OE_Style) )  {         
        // if(LoRa_debug) debugA("LoRa_Device_Handler: sending packet via xQueue_APRS....(is_AX25=%d - is_OE_Style=%d - is_native=%d - is_CRC_Errored=%d)\r", is_AX25 , is_OE_Style,is_native, is_CRC_Errored );
        // if(APRS_debug){ int i=0; while(i < LDM_Msg_v2.bufLen ){if(! (i%16))debugAl("\r")  ; debugAl("%02X ", LDM_Msg_v2.bufData[i]); i++;   }; debugAl("\r");};
        if( xQueueSend( xQueue_APRS, (void *)&LDM_Msg_v2, ( TickType_t ) 10 ) != pdPASS ) {
          if(LoRa_debug) debugA("LoRa_Device_Handler: unable to queue message toward xQueue_APRS....\r");
            // RX packet lost due to IPP fault
            IPC_lost_msgs++ ;
            };
          }
      else {  // native or CRC_errored packets
        if(LoRa_debug) debugA("LoRa_Device_Handler: sending packet via xQueue_native....(is_AX25=%d - is_OE_Style=%d - is_native=%d - is_CRC_Errored=%d )\r", is_AX25 , is_OE_Style,is_native, is_CRC_Errored );       
        if( xQueueSend( xQueue_native, (void *)&LDM_Msg_v2, ( TickType_t ) 10 ) != pdPASS ) {
          if(LoRa_debug) debugA("LoRa_Device_Handler: unable to queue message toward xQueue_native....\r");
          // RX packet lost due to IPP fault
          IPC_lost_msgs++ ;
          };
        };
      }   // end of if ((state == RADIOLIB_ERR_NONE) || ((state == RADIOLIB_ERR_CRC_MISMATCH)))
/*        
    else if ((state == RADIOLIB_ERR_CRC_MISMATCH)) {   // packet was received, but is CRC_Errored_
      String time_now =  Italy.dateTime(",Ymd_His ");
      //  if(LoRa_debug) 
      debugA("LoRa_Device_Handler: [SX12x8] CRC error! time_now=%s LDH_RX_Mode=%d \r", time_now.c_str(), LDH_RX_Mode);
      LoRa_CRC_errored_packets++;
      if(LDH_RX_Mode) LoRa_CRC_errored_packets_BCN++ ;
      } 
*/      
    else {                                 // some other error occurred
      if(LoRa_debug) debugA("LoRa_Device_Handler: [SX12x8] Failed, code=%d\r ", state); 
      LoRa_UMN_errored_packets++;
      }; // end of RX packet analalyses

    // just update statistics counters for the received packets 
    if ((rx_state == RADIOLIB_ERR_CRC_MISMATCH) ) {   // packet was received, but is CRC_Errored_
      String time_now =  Italy.dateTime(",Ymd_His ");
      // if(LoRa_debug) 
      debugA("LoRa_Device_Handler: [SX12x8] CRC_Errored_Packet received ! time_now=%s LDH_RX_Mode=%d rx_state=%d\r", time_now.c_str(), LDH_RX_Mode, rx_state);
      if (rx_state == RADIOLIB_ERR_CRC_MISMATCH){
        LoRa_CRC_errored_packets++;
        if(LDH_RX_Mode) LoRa_CRC_errored_packets_BCN++ ;
        };
         
      if(syslog_available) syslog.logf(LOG_INFO, "LoRa_Device_Handler: [SX12x8] CRC_Errored_Packet error! time_now=%s LDH_RX_Mode=%d - rx_state=%d - LoRa_CRC_errored_packets=%d - LoRa_CRC_errored_packets_BCN=%d ", time_now.c_str(), LDH_RX_Mode, rx_state , LoRa_CRC_errored_packets, LoRa_CRC_errored_packets_BCN  );
      }  
    };     // end of if (receivedFlag == true)  898

  // test for debug options
  // debugA("\r\n\r\nThis is a test of debugA.... at Lora_DevideHandler \r\r\n\r\n");
  // debugAl("\r\n\r\nThis is a test of debugAl.... at Lora_DevideHandler \r\r\n\r\n");
  // counter6++;
  // vTaskDelay(500);     // just release processor for a while...
  // }                    // end of task endless loop
} // end of LoRa_Device_Handler



// examples
//====================================
/*
      if( msg_queue != 0 ){
         // Receive a message on the queue.  Block for 10 ticks if a message is not immediately available.
         if (xQueueReceive(msg_queue, (void *)&rcv_msg, 0) == pdTRUE) {
         // pcRxedMessage now points to the struct AMessage variable posted by vATask.
         debugA("LoRa_Device_Handler-MSG_RX: SigRep=%s\r",rcv_msg.SigRep);
         debugA("LoRa_Device_Handler-MSG_RX: ack=%d\r", rcv_msg.ack);
         }
      else{
          if(LoRa_debug) debugA("LoRa_Device_Handler: NO MESSAGE AVAILABLE\r");
          };
        }; 

//--------------------------------------------------------------

    if( xQueue_in != 0 ){
       // Receive a message on the queue.  Block for 10 ticks if a message is not immediately available.
       if (xQueueReceive(xQueue_in, (void *)&LDM_Msg, 0) == pdTRUE) {
           // pcRxedMessage now points to the struct AMessage variable posted by vATask.
           debugA("LoRa_Device_Handler-MSG_RX-2: SigRep=%s\r",LDM_Msg.SigRep);
           debugA("LoRa_Device_Handler-MSG_RX-2: ack=%d\r", LDM_Msg.ack);
           if(LoRa_debug) debugA("LoRa_Device_Handler-MSG_RX-2:  bufLen =%d - bufData=%s - LoraFreq=%8.4f - LoraBw=%8.4f - LoraSf=%d - LoraCodingRate=%d\r",LDM_Msg.bufLen , (char *)LDM_Msg.bufData ,(float) LDM_Msg.LoraFreq, (float )LDM_Msg.LoraBw  , (byte )LDM_Msg.LoraSf , (byte)LDM_Msg.LoraCodingRate );
           }
       else{
          if(LoRa_debug) debugA("LoRa_Device_Handler-MSG_RX-2: NO MESSAGE AVAILABLE\r");
          };
        }; 

*/
//..................................................

/*
    LDM_Msg_TX_v2.ack = 0 ;
    // send LDM_Msg_TX_v2 to LDH to setup lora radio 
    if( xQueue_in != 0 ) {     
         if( xQueueSend( xQueue_in, (void *)&LDM_Msg_TX_v2, ( TickType_t ) 10 ) != pdPASS ) {
          if(LoRa_debug) debugA("Service::SetLoraConfig: unable to queue message toward LDH xQueue_in: failed LoRa device setup....\r");
          IPC_lost_msgs++ ;
          return(false);
          };
        }
    else{
        if(LoRa_debug) debugA("Service::SetLoraConfig: xQueue_in not available ....\r"); 
        return(false);                       
      }; 
 */



 
