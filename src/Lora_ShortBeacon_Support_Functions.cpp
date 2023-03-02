// Lora_ShortBeacon_Support_Functions.cpp
#include "Lora_ShortBeacon_Support_Functions.h"

// Native LoRa packets discovery in our implementation will be done based on the failing check for a valid AX25 payload:
// this empirical method will allow to maintain compatibility of our AX25 APRS implementation and other LoRa applications
// using other non AX25 based trasmission modes.
//
// A first application for the native transmission method is to implement a "short beaconing app" for propagation tests
//
// In this "short beaconing app" Native LoRa packets are marked by a leading "$" character followed by the following payload
//  callsign[6] QTH_Locator[6] dbm_power[2] ==> $IQ8SO EF34je27 i.e. native packet from IQ8SO in EF34je transmitting with 27 dbm power
// other LoRa parameters will be deducted from currenti LoRa subsystem setup
//
// A second native app is the support of the "OE style LoRa APRS" implementation according the https://github.com/peterus implementation
// and in particular https://github.com/lora-aprs/APRS-Decoder-Lib   for the APRS messages content 
// and https://github.com/lora-aprs/LoRa_APRS_iGate/blob/master/lib/LoRa_APRS/LoRa_APRS.cpp for the LoRa_APRS message  identification
//
// In this second "OE style LoRa APRS app" Native LoRa packets are marked by a leading "<" character  followed by "0xff 0x01" and  the 
// APRS message contents as per the first document above
// other LoRa parameters will be deducted from current LoRa subsystem setup


void handle_nativeLoRaPacket( byte *rxBuf, int rxBufIndex, float rssi, float lora_snr, float frequencyError){
  rxBuf[rxBufIndex + 1] = '\0'; 
  int i=0;
  if(rxBuf[0] == '<'){ 
    debugA("handle_nativeLoRaPacket: Found OE_APRS_Packet: payload = [%s]\r", (char *)rxBuf );
    }   
  else if(rxBuf[0] == '$'){ 
    debugA("handle_nativeLoRaPacket: Found ShortBeaconPacket: payload = ");
    i=0; while(i < rxBufIndex ){if(! (i%16))debugAl("\r")  ; debugAl("%02X ", rxBuf[i]); i++;   }; debugAl("\r");
    debugA("handle_nativeLoRaPacket: ===> rssi=%8.2f - lora_snr=%8.2f - freq_err=%8.2f\r", rssi, lora_snr, frequencyError);    
    String textPayload = "PROVA";
    String signalReport = "";
    String my_spot = String(rssi,2) + String(" ") + String(lora_snr,2) + String(" ") +String(frequencyError,2);
    send_native_log_msg(textPayload, signalReport, my_spot);   // send to spot log server tag 1
    }
  else if(rxBuf[0] == '#'){ 
    debugA("handle_nativeLoRaPacket: Found Magic packet: payload = ");
    i=0; while(i < rxBufIndex ){if(! (i%16))debugAl("\r")  ; debugAl("%02X ", rxBuf[i]); i++;   }; debugAl("\r");
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
    debugA("handle_nativeLoRaPacket: Found Unknown packet type: payload = ");
    i=0; while(i < rxBufIndex ){if(! (i%16))debugAl("\r")  ; debugAl("%02X ", rxBuf[i]); i++;   }; debugAl("\r");
    debugA("handle_nativeLoRaPacket: ===> rssi=%8.2f - lora_snr=%8.2f - freq_err=%8.2f\r", rssi, lora_snr, frequencyError);    
    };   
  return;
}


void sendPeriodicShortBeacon_noCAD(){
  debugA("sendPeriodicShortBeacon_noCAD: passing checkpoint...\r ");
  long currentMs = millis();
  byte LoRa_buf[102];

  // if (previousShortBeaconMs == 0 || currentMs - previousShortBeaconMs >= aprsShortBeaconPeriodSeconds  * 1000 || requirePeriodicBeacon ) {
  requirePeriodicShortBeacon = false;
  //   sprintf((char *)LoRa_buf, "$IQ8SO EF34je27");
  sprintf((char *)LoRa_buf, "$IQ8SO");

  uint8_t EncapType =  0; // native packet no encapsulation
  //   LDH_sendNativeToLora(LoRa_buf, sizeof(LoRa_buf, EncapType, true ));
  //   LDH_sendNativeToLora_noCAD(LoRa_buf, 15, EncapType , true );
  LDH_sendNativeToLora_noCAD(LoRa_buf, 8, EncapType , true , false );
  previousShortBeaconMs = currentMs;
  return ;
}

void sendPeriodicShortBeacon(){
  long currentMs = millis();
  byte LoRa_buf[102];

  uint8_t EncapType  = 0; // native payload no encapsulation
  if (previousShortBeaconMs == 0 || currentMs - previousShortBeaconMs >= aprsShortBeaconPeriodSeconds  * 1000 || requirePeriodicBeacon ) {
    requirePeriodicShortBeacon = false;
    sprintf((char *)LoRa_buf, "$IQ8SO EF34je27");
    //   LDH_sendNativeToLora(LoRa_buf, sizeof(LoRa_buf), EncapType , true );
    LDH_sendNativeToLora(LoRa_buf, 15, EncapType, true , false  ); 
    previousShortBeaconMs = currentMs;
    return ;
    };
}


void sendNativeUdpMsg(char * Msg) {  //data will be sent to the statistics server
  uint8_t buffer[520] = "hello world\r";
  if(APRS_debug) debugA("sendNativeUdpMsg: starting....(%s - %d - [%s])\r", udpAddress.c_str(), udpPort, Msg );
  strcpy((char *)buffer,Msg);
  //send hello world to server
  if(udp.beginPacket(udpAddress.c_str(), udpPort) == 1) {
    udp.write(buffer,strlen(Msg));
    if(udp.endPacket() == 1) { 
      if(APRS_debug) debugA("sendNativeUdpMsg: endPacket ok\r");
      }
    else {
      if(APRS_debug) debugA("sendNativeUdpMsg: endPacket fail\r"); 
      };
    }
  else{ 
    if(APRS_debug) debugA("sendNativeUdpMsg: beginPacket fail\r");
    };  
  memset(buffer, 0, 520);
  //processing incoming packet, must be called before reading the buffer
  /*  
  udp.parsePacket();
  //receive response from server, it will be HELLO WORLD
  if(udp.read(buffer, 520) > 0){
    debugA("Server to client: ");
    debugA((char *)buffer);
  }
  */
}


// create log messages with a suitable format; allows to use geolocation with sarimesh tools
// also creates and write a record o local FRAM log

bool send_native_log_msg(String textPayload , String signalReport, String my_spot){   // sends spots to aour private server via UDP
  char Msg[1024]; char Msg1[512];
  // sample textPayload=[Id=5 SeqNbr=112 time=1636552079 lat=40.64 lat=14.41 freq=433.77 pwr=22 wc=0x412A]
  // sample signalReport=[ (2.64 I8[0mFUC-7 -40.00 12.00 0  52.70)] 
  // debugA("send_native_log_msg: create log messages ===> textPayload=[%s] - signalReport=[%s] - my_spot=[%s] \r", textPayload.c_str(), signalReport.c_str(),my_spot.c_str());
  time_t ora =  ezt::now();
  bool local_is_CRC_Errored = false ;
  int posCRC_Errored = signalReport.indexOf("CRC_Errored"); 
  if(posCRC_Errored > 0) {
    local_is_CRC_Errored = true;
    sprintf(Msg,"3,%d",ora);  // for remote server
    sprintf(Msg1,"3,%d",ora);   // for local log in FRAM         
    }
  else{
    sprintf(Msg,"1,%d",ora);  // for remote server
    sprintf(Msg1,"1,%d",ora);   // for local log in FRAM       
    }

  String message = Msg ;    // ex. 0,1628957071,
  String spot_time = Msg1 ; // ex. 1628957071    
  // String time_now =  Italy.dateTime(",Y-m-d~TH:i:s~Z ");
  String time_now =  Italy.dateTime(",Ymd_His ");
  time_now.toCharArray(Msg,time_now.length() ); 
  message = message + Msg + "," ;        // ex. 0,1628957071,2021-08-14T18:04:31Z,
  // debugA("Setup timezone Italy time: " + Italy.dateTime("l, d/n/Y H:i:s.v T\r"));
  int space_index = textPayload.indexOf(' '); int eq_index = textPayload.indexOf('=');
  String TX_Id = textPayload.substring(eq_index +1 , space_index);
  String Res = textPayload.substring(space_index+1); 
  space_index =  Res.indexOf(' '); eq_index = Res.indexOf('=');
  String TX_SeqNbr = Res.substring(eq_index +1 , space_index); 
  Res = Res.substring(space_index+1);
  space_index =  Res.indexOf(' '); eq_index = Res.indexOf('=');
  String TX_time = Res.substring(eq_index +1 , space_index);
  Res = Res.substring(space_index+1);
  space_index =  Res.indexOf(' '); eq_index = Res.indexOf('=');
  String TX_lat = Res.substring(eq_index +1 , space_index);
  Res = Res.substring(space_index+1);
  space_index =  Res.indexOf(' '); eq_index = Res.indexOf('=');
  String TX_lon = Res.substring(eq_index +1 , space_index);
  Res = Res.substring(space_index+1);
  space_index =  Res.indexOf(' '); eq_index = Res.indexOf('=');
  String TX_freq = Res.substring(eq_index +1 , space_index);      
  Res = Res.substring(space_index+1);
  space_index =  Res.indexOf(' '); eq_index = Res.indexOf('=');
  String TX_pwr = Res.substring(eq_index +1 , space_index);
  Res = Res.substring(space_index+1);
  space_index =  Res.indexOf(' '); eq_index = Res.indexOf('=');
  String TX_wc = Res.substring(eq_index +1 , space_index);   
  //  String first_node_rep , last_node_rep , last_node ;String DutyCycleNow ;
  // extract first and last reception reports from incoming packet if LoRa packet
  int first_nodeIndex = signalReport.indexOf('(');
  int first_node_endIndex = signalReport.indexOf(')');
  String first_node_rep = signalReport.substring(first_nodeIndex +1 , first_node_endIndex);
  space_index =  first_node_rep.indexOf(' ');
  String RX_Cong = first_node_rep.substring(0 , space_index);    
  Res = first_node_rep.substring(space_index + 1);
  space_index =  Res.indexOf(' '); 
  String RX_Name = Res.substring(0 , space_index);
  Res = Res.substring(space_index + 1);
  space_index =  Res.indexOf(' '); 
  String RX_RSSI = Res.substring(0 , space_index);
  Res = Res.substring(space_index + 1);
  space_index =  Res.indexOf(' '); 
  String RX_SNR = Res.substring(0 , space_index);      
  if(APRS_debug) debugA("\r\n+++++++> send_native_log_msg: textPayload=[%s] - signalReport=[%s] - my_spot=[%s] - first_node_rep=[%s]\r\n", textPayload.c_str() , signalReport.c_str(), my_spot.c_str() ,  first_node_rep.c_str() );
  // unsigned long distancemTo = (unsigned long)TinyGPSPlus::distanceBetween( mylat_norm, mylon_norm, homelat, homelon);  // distance to present  device position or static position
  // debugA("===== DELTA DISTANCE ====[%f,%f]===========> distancemTo = %lu\r", mylat_norm, mylon_norm, distancemTo);
  // old_Lat = mylat_norm ; old_Lon = mylon_norm ;
  String sender_call = String("ID=") + TX_Id;
  if(APRS_debug){
    debugAl("====> TX_Id     %s\r\n" , TX_Id.c_str() );
    debugAl("====> TX_SeqNbr %s\r\n" , TX_SeqNbr.c_str() );
    debugAl("====> TX_time   %s\r\n" , TX_time.c_str() );
    debugAl("====> TX_lat    %s\r\n" , TX_lat.c_str() );
    debugAl("====> TX_lon    %s\r\n" , TX_lon.c_str() );
    debugAl("====> TX_freq   %s\r\n" , TX_freq.c_str() );
    debugAl("====> TX_pwr    %s\r\n" , TX_pwr.c_str() );
    debugAl("====> TX_wc     %s\r\n" , TX_wc.c_str() );
    debugAl("====> fn_rep    %s\r\n" , first_node_rep.c_str() );
    debugAl("====> RX_Cong   %s\r\n" , RX_Cong.c_str() );
    debugAl("====> RX_Name   %s\r\n" , RX_Name.c_str() );
    debugAl("====> RX_RSSI   %s\r\n" , RX_RSSI.c_str() );
    debugAl("====> RX_SNR    %s\r\n" , RX_SNR.c_str() );
    float rx_rssi = RX_RSSI.toFloat();
    float rx_snr  = RX_SNR.toFloat();
    String RX_ENL = "--";
    if( rx_snr < 0.0 ){
      RX_ENL = String (rx_rssi + rx_snr);
      };        
    debugAl("====> RX_ENL     %s\r\n", RX_ENL.c_str() );      
    };
           
    /*
    // we just accept only packets including a valid position 
    if(( mylat_norm == 0.0) || ( mylon_norm == 0.0)) {
    if(APRS_debug) debugA("====> Discarding packet after policy checks\r");
      return (false);
      };
    */
    float mylat_norm = TX_lat.toFloat();
    float mylon_norm = TX_lon.toFloat();
    String com = textPayload ;
    // extract working conditions from wc
    int bwi=1, sfi=1, cri=1, pri=1 ;   
    sscanf(TX_wc.c_str(), "0x%01d%01d%01d%01d", &bwi, &sfi, &cri, &pri);
    // debugA ("================ TX_wc=[%s]======================>>>> bwi=%d - sfi=%d - cri=%d - pri=%d \r", TX_wc.c_str(), bwi, sfi, cri, pri );
    String LoraBw = String(BWindx[bwi],2);
    String LoraSf = String(SFindx[sfi]) ;
    String LoraCodingRate = String(CRindx[cri]) ;
    String LoraPreambleLen = String(PrLenindx[pri]) ;
    int CRC_errored_packets_APRS = (LoRa_CRC_errored_packets - LoRa_CRC_errored_packets_BCN);
    String BCN_Status = String(" ID=") + String(TX_Id) +String(" SeqNbr=") + String(TX_SeqNbr) +String(" time=") + String(TX_time) + String(" freq=") + String(TX_freq) +String(" pwr=") + String(TX_pwr) + String(" BW=") + String(LoraBw) + String(" SF=") + String(LoraSf) + String(" CR=") + String(LoraCodingRate) + String(" pl=") + String(LoraPreambleLen) + String(" CRC_err=") + String(LoRa_CRC_errored_packets_BCN) + String(" CRC_err_APRS=") + String(CRC_errored_packets_APRS)   + String(" nat_pkt=") + String(LoRa_rx_native_packets);    
    // debugA ("================ TX_wc=[%s]======================>>>> bwi=%d sfi=%d cri=%d pri=%d  LoraBw=%8.2f LoraSf=%d LoraCodingRate=%d  LoraPreambleLen=%d\r", TX_wc.c_str(), bwi, sfi, cri,pri, BWindx[bwi] , SFindx[sfi], CRindx[cri], PrLenindx[pri] );
    // debugA ("================ TX_wc=[%s]======================>>>>   BCN_Status = [%s]", TX_wc.c_str() ,BCN_Status.c_str());
    // OK finaly we was able to say that a new LoRa packet has been processed correctly... send log message to remote sarimesh server
    // message = message + sender_call + "," + sender_call + ",,,," + sender_call + ",/#," + String(mylat_norm,7) + "," + String(mylon_norm,7) + ",,,,,,,,,," + com + signalReport  ;
    message = message + sender_call + "," + sender_call + ",,,," + sender_call + ",/#," + String(mylat_norm,7) + "," + String(mylon_norm,7) + ",,,,,,,,,," + BCN_Status + signalReport  ;
    // send syslog message
    if(syslog_available) syslog.logf(LOG_INFO, "%s", message.c_str());
    // set display content and flag display event and sound buzzer 
    ui3_payload = String("ID=") + String(TX_Id) +String("  SeqNbr=") + String(TX_SeqNbr) ; 
    ui4_payload = String("freq=") + String(TX_freq) +String("  pwr=") + String(TX_pwr)  ;
    ui5_payload = String("BW=") + String(LoraBw) + String(" SF=") + String(LoraSf) + String(" CR=") + String(LoraCodingRate) + String(" pl=") + String(LoraPreambleLen) ;
    display_event = 1 ;
    soundBuzzer();

    /*       
    lora_payload = textPayload;   
    // set dashboard variables
    RX_Last_Spot_payload = textPayload ; RX_Last_Spot_payload.replace("|"," ");
    RX_Last_Spot_level = signalReport; 
    if((first_node_rep != last_node_rep) || ( first_node_rep.indexOf("FromInt") > 0 )){
      RX_Last_Path_Report = first_node_rep + "<br>--> " + last_node_rep + "<br>---->" + signalReport ; 
      }
    else {
      RX_Last_Path_Report = first_node_rep + "<br>--> " + "   --  " + "<br>---->" + signalReport ; 
      };
    */
      
    // send new spot to remote sarimesh server
    message = message + " \n\r";
    message.toCharArray(Msg,message.length()); 
    sendNativeUdpMsg(Msg);                                 

    // write new spot to FRAM
    String my_local_spot;
    /*       
    if((first_node_rep != last_node_rep) || ( first_node_rep.indexOf("FromInt") > 0 )){
      my_local_spot= String(spot_time) + "|" + String( mylat_norm,5) + "|" + String(mylon_norm,5)+ "|" + String(sender_call) + "|" + String(my_spot) + "|" +  String(last_node);
      }
    else {
      my_local_spot= String(spot_time) + "|" + String( mylat_norm,5) + "|" + String(mylon_norm,5)+ "|" + String(sender_call) + "|" + String(my_spot) + "|" +  "--------";
      };
    */

    my_spot.replace(" (","");
    int my_spot_init = my_spot.indexOf(" ");  int my_spot_end = my_spot.lastIndexOf(" ");
    my_spot = my_spot.substring(my_spot_init + 1 , my_spot_end - my_spot_init +1 );       
    // my_spot_end = my_spot.lastIndexOf(" ");  my_spot = my_spot.substring(0 , my_spot_end + 1 ); 
    if(local_is_CRC_Errored) { my_local_spot = "1|" ; } else { my_local_spot = "0|" ; };   
    // my_local_spot= String( mylat_norm,5) + "|" + String(mylon_norm,5)+ "|" + String(sender_call) + "|" + String(my_spot) + "|" +  "-NATIVE-------+++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++";
    my_local_spot= my_local_spot + String(sender_call) + "|" + String(my_spot) + "|" +  "----+++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++";
    my_local_spot =  my_local_spot.substring(0, 52);  // clip any local spot to only the first 63 bytes
    // debugA("====> %s\r\r\r", my_local_spot.c_str() );
    write_spot_to_log(my_local_spot);                             
    return (true);              
}

