// beacon_aux_funtions.cpp
/* native lora beacon payload struct
 
 data_type: 1 byte (uint8_t) : ex: $
 node_id: 1 byte ex 1 - 255 nodes in a dictionary
 sequence_number: 1 bytes  ( uint8_t)
 unix_time : 4 bytes (uint32_t)
 latitude: 4 bytes (int32_t)  ex. -33.905024  
 longitude: 4 bytes (int32_t)  ex. 151.26648  
 frequency: 4 bytes (unit32_t) ex 433.775000
 tx_power : 1 byte (int8_t) ex -127 : +128  
 working_conditions: 2 bytes (uint16_t) : <BW:4bits> <SF:4bits><CR:4bits><preamble:4bts>

 total_length= 22 bytes
 on_air_time at worst conditions: 25 secs


Reception report struct

  tx_report:  22 bytes
  node_name:   6 bytes  ( from node lookup table: mnemonics )
  rx_rssi:     1 byte ( real rssi +128 :  ex:  rssi = -134  -->  rx_rssi = -6  ; rssi= -30 --> rx_rsi = +98 )
  rx_snr:      1 byte ( -127 : +128 )
  sig_status:  1 byte ( 0= nosig  1= CRC_error 2: pakt_ok )

Total payload size= 31 bytes


Transmission schedule:

Single link mode:
   minute odd: offset in secs  from endpoint A to endpoint B  
   minute even: offset in secs  from endpoint B to endpoint A  
Multi link mode
   minute<==>node_id 

Minute schedule
 0   start
 1   SF=12 / BW=7.8   onair 26 sec  minrssi -150
 30  SF=12 / BW=15.6  on air 13 sec minrssi -147
 45  SF=12 / BW=31.25  on air 7 sec minrssi -144
 52  SF=10 / BW=31.25  on air 2 sec minrssi -139
 54  SF=9  / BW=31.25  on air <1 sec minrssi -136
 55  SF=8  / BW=31.25  on air <1 sec minrssi -134
 56  SF=7  / BW=31.25  on air <1 sec minrssi -130
 57  SF=6  / BW=31.25  on air <1 sec minrssi -126   4pkt
 58  SF=6  / BW=62,5  on air <1 sec minrssi -122   5pkt
 59  SF=6  / BW=125  on air <1 sec minrssi -119  5pkt
 

 Reception map example

  sec    minrssi  wkg_con tx_pwr rx_rssi  rx_snr Rx_CRC_err
   1      -150                   -139     6,00     ok
   30     -147                   -130     3,00     ok
   45     -144                   -122    -2,00     ok
   52     -139                   -118    -6,00     ok
   54     -136                   -118    -10,00    ok
   55     -134                   -118    -16,00    ok
   56     -130                    -       -      CRC_err
   57     -126                    -       -      CRC_err
   58     -122                    -       -      nosig
   59     -119                    -       -      nosig

*/

#include "beacon_aux_functions.h"
extern LoraPrs::Service loraPrsService;  // this is the manager ol the LORA_APRS application


// define current beacon schedule configuration
float BWindx[16] = {300.0, 7.8, 10.4, 15.6, 20.8, 31.25, 41.7, 62.5, 125.0, 250.0, 500.0, 0.0, 0.0, 0.0, 0.0, 0.0 };
uint8_t SFindx[16] = {1, 6, 7, 8, 9, 10, 11, 12, 0, 0, 0, 0, 0, 0, 0, 0};
uint8_t CRindx[16] = {1, 5, 6, 7, 8, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0};
uint8_t PrLenindx[16] = {1 , 8, 9, 10, 11, 12, 13, 14, 15, 16, 17, 18, 19, 20, 20, 20};

/*
  <option value="0">halfMinute</option>
  <option value="1">OnMinute</option>
  <option value="2">OnMinuteModulo_2</option>
  <option value="3">OnMinuteModulo_3</option> 
  <option value="4">OnMinuteModulo_4</option> 
  <option value="5">OnMinuteModulo_5</option> 
  <option value="6">OnMinuteModulo_6</option> 
  <option value="7">OnMinuteModulo_10</option> 
  <option value="8">OnMinuteModulo_15</option> 
  <option value="9">OnMinuteModulo_30</option> 
  <option value="10">OnMinuteModulo_60</option> 
  <option value="11">OnMinuteModulo_120</option>   
*/
uint16_t BCN_TimeSlot[16] = {30, 60, 120, 180, 240, 300, 360, 600, 900, 1800, 3600, 7200, 10800, 14400, 18000, 21600};


BeaconRunTimeConfig BCN_cfg_single ;

  BeaconRunTimeConfig BCN_cfg[9] ={
   {0,  433.775,  7.8,   11, 8, 0x34, 10, 0, 15},
   {54, 433.775,  15.6,  12, 8, 0x34, 10, 0, 15},
   {82, 433.775, 31.25,  12, 8, 0x34, 10, 0, 15},
   {96, 433.775, 31.25,   9, 8, 0x34, 10, 0, 15},
   {100, 433.775, 31.25,   7, 8, 0x34, 10, 0, 15},
   {102, 433.775, 31.25,   6, 8, 0x34, 10, 0, 15},
   {104, 433.775, 62.5,   12, 8, 0x34, 10, 0, 15},
   {112, 433.775, 125.0,  12, 8, 0x34, 10, 0, 15},
   {116, 433.775, 125.0,   6, 8, 0x34, 10, 0, 15}
   };

/*
  BeaconRunTimeConfig BCN_cfg[9] ={
   {0,  433.775,  7.8,   12, 8, 0x34, 22, 0, 15},
   {27, 433.775,  15.6,  12, 8, 0x34, 22, 0, 15},
   {41, 433.775, 31.25,  12, 8, 0x34, 22, 0, 15},
   {48, 433.775, 31.25,   9, 8, 0x34, 22, 0, 15},
   {50, 433.775, 31.25,   7, 8, 0x34, 22, 0, 15},
   {51, 433.775, 31.25,   6, 8, 0x34, 22, 0, 15},
   {52, 433.775, 62.5,   12, 8, 0x34, 22, 0, 15},
   {56, 433.775, 125.0,  12, 8, 0x34, 22, 0, 15},
   {58, 433.775, 125.0,   6, 8, 0x34, 22, 0, 15}
   };
*/



bool LoRa_Set_BCN( struct BeaconRunTimeConfig &BCN_vector){
//  SetLoraConfig(float loraFreq, float bw, byte sf, byte cr, int pwr, byte sync, byte prlen , String lora_mod_type);
  if(LoRa_debug) 
  debugA("LoRa_Set_BCN checkpoint....\r");
//  SetLoraConfig(BCN_vector.LoraFreq, BCN_vector.LoraBw,BCN_vector.LoraSf, BCN_vector.LoraCodingRate, BCN_vector.LoraPower, BCN_vector.LoraSync, BCN_vector.LoraPreambleLen, "" );
  return true ;
}

bool BCN_VectorCmp(  struct BeaconRunTimeConfig &BCN_vector_a , struct BeaconRunTimeConfig &BCN_vector_b ) {
    String vec_a, vec_b;
    vec_a=String(BCN_vector_a.BCN_SeqNbr)+" "+String(BCN_vector_a.LoraFreq)+" "+String(BCN_vector_a.LoraBw)+" "+String(BCN_vector_a.LoraSf)+" "+String(BCN_vector_a.LoraCodingRate)+" "+String(BCN_vector_a.LoraSync)+" "+String(BCN_vector_a.LoraPower)+" "+String(BCN_vector_a.LoraFreqCorr)+" "+String(BCN_vector_a.LoraPreambleLen) ; 
    vec_b=String(BCN_vector_b.BCN_SeqNbr)+" "+String(BCN_vector_b.LoraFreq)+" "+String(BCN_vector_b.LoraBw)+" "+String(BCN_vector_b.LoraSf)+" "+String(BCN_vector_b.LoraCodingRate)+" "+String(BCN_vector_b.LoraSync)+" "+String(BCN_vector_b.LoraPower)+" "+String(BCN_vector_b.LoraFreqCorr)+" "+String(BCN_vector_b.LoraPreambleLen) ; 
//    debugA("BCN_VectorCmp: vec_a=[%s] - vec_b=[%s]\r", vec_a.c_str(), vec_b.c_str());
    if(vec_a == vec_b) return (true);
    return (false);
}


void textToVector( String &LoraText ,   struct BeaconRunTimeConfig &BCN_vector ) {
    char *strings[10]; char *ptr = NULL; byte index = 0; char array_loc[40] = "" ;
//  LoraText= "0,433.775,7.8,11,8,0x34,10,0,15";
    LoraText.replace(" ","");
    LoraText.toCharArray( array_loc ,LoraText.length()+1 );
    ptr = strtok(array_loc, ",");  // takes a list of delimiters
    while(ptr != NULL){
       strings[index] = ptr;
       index++;
       ptr = strtok(NULL, ",");  // takes a list of delimiters
       }; 
   for(int n = 0; n < index; n++){ if(LoRa_debug) debugA("textToVector ==============> %s \r",strings[n]); } ;

   BCN_vector.BCN_SeqNbr = atoi(strings[0]); 
   BCN_vector.LoraFreq = atof(strings[1]);
   BCN_vector.LoraBw = atof(strings[2]);
   BCN_vector.LoraSf = atoi(strings[3]);   
   BCN_vector.LoraCodingRate = atoi(strings[4]);
   sscanf(strings[5], "%x", &(BCN_vector.LoraSync));    
   BCN_vector.LoraPower =atoi(strings[6]); 
   BCN_vector.LoraFreqCorr = atoi(strings[7]); 
   BCN_vector.LoraPreambleLen = atoi(strings[8]); 
   if(BCN_vector.LoraPreambleLen < 8 ) BCN_vector.LoraPreambleLen = 8;
   if(LoRa_debug) debugA("-->  BCN_vector ====>   BCN_SeqNbr=%d - LoraFreq=%08.4f - LoraBw=%08.4f - LoraSf=%d - LoraCodingRate=%d - LoraSync=%02X - LoraPower=%d - LoraFreqCorr=%d - LoraPreambleLen=%d \r", BCN_vector.BCN_SeqNbr, BCN_vector.LoraFreq, BCN_vector.LoraBw, BCN_vector.LoraSf, BCN_vector.LoraCodingRate, BCN_vector.LoraSync, BCN_vector.LoraPower, BCN_vector.LoraFreqCorr, BCN_vector.LoraPreambleLen ); 
   return;
}


int  BeaconEncode(  struct BeaconRunTimeConfig &BCN_vector,  byte outBuf_BCN[]){   // (float loraFreq, float bw, byte sf, byte cr, int pwr, byte sync, byte prlen){
    byte mydata[40];
    if(LoRa_debug) debugA("BeaconEncode: starting....\r");
    uint8_t bwi=0; while( BWindx[bwi] != BCN_vector.LoraBw ){ bwi++;};
    uint8_t sfi=0; while( SFindx[sfi] != BCN_vector.LoraSf ){ sfi++;};
    uint8_t cri=0; while( CRindx[cri] != BCN_vector.LoraCodingRate ){ cri++;};
    uint8_t pri=0; while( PrLenindx[pri] != BCN_vector.LoraPreambleLen ){ pri++;};
    int pkt_len=0;
    LoraEncoder encoder(mydata);
    encoder.writeUint8('$'); 
    encoder.writeUint8(BeaconId); 
 //   BeaconFmt = 0; BeaconFmt = BeaconWorkConditions | BeaconPower | BeaconFreq  | BeaconLocation | BeaconUnixTime | BeaconSeqNbr;
    if(LoRa_debug)  
    debugA("BeaconEncode: ===============================> BeaconFmt=0x%02X ==> BeaconWorkConditions=%d BeaconPower=%d BeaconFreq=%d BeaconLocation=%d BeaconUnixTime=%d BeaconSeqNbr=%d \r" , BeaconWorkConditions , BeaconPower , BeaconFreq  , BeaconLocation , BeaconUnixTime , BeaconSeqNbr);
//    encoder.writeUint8(BeaconFmt);  
    pkt_len = 2;   
    if(BeaconSeqNbr){ encoder.writeUint8(BCN_MsgSN); BCN_MsgSN++ ; pkt_len = pkt_len +1; };
    if(BeaconUnixTime){ encoder.writeUnixtime(UTC.now()); pkt_len = pkt_len +4;};        // ex. 1468075322  4bytes
    if(BeaconLocation){ encoder.writeLatLng(homelat,homelon); pkt_len = pkt_len +8; };  // ex. -33.905024, 151.26648  8 bytes
    uint32_t freq_int = int (BCN_vector.LoraFreq*1e6 );
    if(BeaconFreq){ encoder.writeUnixtime(freq_int); pkt_len = pkt_len +4;};        // ex. 1468075322  4bytes
    if(BeaconPower){ encoder.writeUint8(BCN_vector.LoraPower + 128 ); pkt_len = pkt_len +1; };
    uint16_t workConditions=  bwi * 4096 + sfi *256 + cri *16 + pri ; 
    if(LoRa_debug)debugA("BeaconEncode: unixtime=%d - BCN_MsgSN=%d ===>  bw=%d - sf=%d - cr=%d - prlen=%d  ==> workConditions=0x%02X\r",UTC.now() ,BCN_MsgSN,bwi,sfi,cri,pri,workConditions);
    if(BeaconWorkConditions){ encoder.writeUint16(workConditions); pkt_len = pkt_len +2;}; // 2 bytes
    if(LoRa_debug) debugA("BeaconEncode: payload="); int i=0; while(i < pkt_len ){if(! (i%16))debugAl("\r")  ; debugAl("%02X ", mydata[i]); i++;   }; debugAl("\r");
    mydata[pkt_len + 1] = '\0';  // fix result length
    if(LoRa_debug) debugA("BeaconEncode: complete (pkt_len=%d)\r", pkt_len);
    memcpy(outBuf_BCN, mydata, pkt_len); 
    return ( pkt_len ); 
}


String  BeaconDecode( byte *rxBuf_BCN, const int packetSize){   
    // (float loraFreq, float bw, byte sf, byte cr, int pwr, byte sync, byte prlen){
    // encoded data in rxBuf_BCN / packetSize 
    // decoded payload in String textpayload

    rxBuf_BCN[packetSize+1] = '\0';  // fix input buffer length
    String xxx= String((char *)rxBuf_BCN);  // iput buffer as a string
    String textPayload = ""; int x ;
    int BCN_ptr =3 ;

    uint32_t unixtime=0; if (BeaconUnixTime) { for (x=0; x<4 ; x++){ unixtime |= +(rxBuf_BCN[BCN_ptr + x] << (x * 8)); }; BCN_ptr = BCN_ptr + 4 ; };
    uint32_t lat_int=0;  if (BeaconLocation) { for (x=0; x<4 ; x++){ lat_int |= +(rxBuf_BCN[BCN_ptr + x] << (x * 8)); }; BCN_ptr = BCN_ptr + 4 ; };
    uint32_t lon_int=0;  if (BeaconLocation) { for (x=0; x<4 ; x++){ lon_int |= +(rxBuf_BCN[BCN_ptr + x] << (x * 8)); }; BCN_ptr = BCN_ptr + 4 ; };
    double lat_x = lat_int/1e6; 
    double lon_x = lon_int/1e6;
    
    uint32_t freq_int=0; if (BeaconFreq) { for (x=0; x<4 ; x++){ freq_int |= +(rxBuf_BCN[BCN_ptr + x] << (x * 8)); }; BCN_ptr = BCN_ptr + 4 ; };
    float freq_x = freq_int/1000000.0;
    
    int8_t pwr_int = 0; if (BeaconPower) { pwr_int = (uint8_t )rxBuf_BCN[BCN_ptr ]  - 128 ;  BCN_ptr++ ; }; 

    uint16_t wc_int = 0 ; if (BeaconWorkConditions) {wc_int = (rxBuf_BCN[BCN_ptr +1 ] << 8) + rxBuf_BCN[BCN_ptr ]; } ;
    char sz[32];
    sprintf( sz, "0x%02X", wc_int ); String wc_str = String((char *)sz); 
    textPayload = "Id=" + String(rxBuf_BCN[1]) + " SeqNbr=" + String(rxBuf_BCN[2]) + " time=" + String((uint32_t)unixtime) + " lat=" + String((float)lat_x) + " lon=" + String((float)lon_x) + " freq=" + String(freq_x,3) + " pwr=" + String(pwr_int) + " wc=" + wc_str ; 
    if(LoRa_debug) debugA("BeaconDecode: ======= unixtime=%ld   lat_int=%ld  lon_int=%ld  freq_int=%ld   pwr_int=%d   wc_str=%s ==================>  rxBuf_BCN-String=[%s] - payload=[%s]\r", unixtime,lat_int,lon_int, freq_int,pwr_int ,wc_str.c_str(), xxx.c_str(), textPayload.c_str()); 
    return (textPayload);
} 



time_t nextRuntime_single() {
  time_t t = UTC.now();  // time now
  if(LoRa_debug) debugA("nextRuntime:  time_now=%ld --> ", t);
  int slice_length = 60*BCN_TimeSlotSync ;
  t= int((t+slice_length)/slice_length)*slice_length ;
  if(LoRa_debug) debugA("  nextRuntime=%ld - slice_length=%d \r", t, slice_length);   
  return t;
}


time_t nextRuntime_short() {
  time_t t = UTC.now();  // time now
  if(LoRa_debug) debugA("nextRuntime:  time_now=%ld --> ", t);
  int slice_length = 60*BCN_TimeSlotSync ;
  t= int((t+slice_length)/slice_length)*slice_length ;
  if(LoRa_debug) debugA("  nextRuntime=%ld - slice_length=%d \r", t, slice_length);  
  return t;
}

time_t nextRuntime_long() {
  time_t t = UTC.now();  // time now
  if(LoRa_debug) debugA("nextRuntime:  time_now=%ld --> ", t);
  int slice_length = 60*BCN_TimeSlotSync ;
  t= int((t+slice_length)/slice_length)*slice_length ;
  if(LoRa_debug) debugA("  nextRuntime=%ld - slice_length=%d \r", t, slice_length);   
  return t;
}

//========================================================================

void BeaconSequencer_tuning(void){
    int delta_time;
    time_t t = UTC.now();  // time now
//  if(LoRa_debug) 
    debugA("\r---------%ld----------> BeaconSequencer_tuning: starting... BCN_SeqNbr=%d ", t, BCN_SeqNbr);
//  if(LoRa_debug) 
    debugA("==> hour=%d - minutes=%d - seconds=%d - msec=%d\r", hour(), minute(), second(),ms());
    // first of all set loRa working conditions and next sequence step
    int present_BCN_SeqNbr = BCN_SeqNbr;  // save present BCN sequence number 
    BCN_SeqNbr++;

//  Setup Lora according to the tuning setup BCN_LoRa_Vector
//    BeaconRunTimeConfig BCN_cfg_tuning = {0,  433.775,  7.8,   11, 8, 0x34, 10, 0, 15} ;

    BeaconRunTimeConfig BCN_cfg_tuning  ; // use basic LoRa configuration as in the BCN_cfg_single configuration vector
    textToVector( BCN_LoRa_Vector , BCN_cfg_tuning); // populate BCN_cfg_tuning from beacon text vector

//  if(LoRa_debug) 
    debugA("BeaconSequencer_tuning: BCN_LoRa_Vector=%s \r", BCN_LoRa_Vector.c_str() ); 
    LoRa_Set_BCN( BCN_cfg_tuning );   // setup LoRa as per the nominal setup configuration 
    
    byte BCN_Payload[40]; int BCN_PayloadLen = 0 ;
    BCN_PayloadLen = BeaconEncode( BCN_cfg_tuning,(byte *)BCN_Payload );   // create present spot binary payload 
    
    uint8_t EncapType = 0; // native packets no encapsulation  
//  BeaconEngineType is irrelevant in this mode 
//  BeaconId is just reported in the sptos but not cared for timing: oll the native time slot is allocated 
//  to transmit a sequence of very shortpackets with varying ppm values in order to check the RX frequency 
//  allignement on the other side of the link
    // 0,  433.775,  10.4,   12, 7, 0x34, 22, 0, 8  ogni 3 minuti mx 4 nodi 80 sec ogn 180 per traffico aprs

    uint32_t r = 6000 ; // offset of the start of spot  sequence just 2 secs after the start of the timeslot
    uint32_t schedTimeEnterRXBeaconMode = GetNextSlice(BCN_TimeSlot[BCN_TimeSlotSync], BCN_TimeSlotOffset);
    uint32_t TX_schedTime = schedTimeEnterRXBeaconMode ;
    uint32_t schedTimeExitRXBeaconMode = schedTimeEnterRXBeaconMode + 100000;  // the full spot sequence must last at worst 100 secs

    SchedEnterRX_BeaconMode((byte *)BCN_Payload, BCN_PayloadLen, EncapType, schedTimeEnterRXBeaconMode , BCN_TimeSlot[BCN_TimeSlotSync] * 1000, BCN_cfg_tuning ) ;
    int delta_ppm = -9 ;  // start at -6 ppm and loop till +6 ppm of frequency offset
    float LoraFreq_nominal = BCN_cfg_tuning.LoraFreq;

    if( BeaconEngineType == 0){   // txOnly
       while( r < 90000 ){   // txOnly variable ppm freq offset until 10 secs before end of allocated time slot
          debugA("Scheduling packet with ppm_offset = %d\r", delta_ppm );
          TX_schedTime = schedTimeEnterRXBeaconMode + r ; 
          BCN_PayloadLen = 0 ;
          BCN_cfg_tuning.LoraFreq = LoraFreq_nominal * ( 1.0 + (float)(delta_ppm)/1000000.0 );        
          BCN_PayloadLen = BeaconEncode( BCN_cfg_tuning,(byte *)BCN_Payload );   // create present spot binary payload 
          debugA("=====> %8.5f - %8.5f - %d\r\n", BCN_cfg_tuning.LoraFreq, BCN_cfg_tuning.LoraBw, BCN_cfg_tuning.LoraFreqCorr );
          loraPrsService.SchedNativeToLora_noCAD((byte *)BCN_Payload, BCN_PayloadLen, EncapType, TX_schedTime  , BCN_TimeSlot[BCN_TimeSlotSync] * 1000, BCN_cfg_tuning, false ) ;
          r = r + 13000 ; delta_ppm = delta_ppm + 3 ;
          };
       BCN_cfg_tuning.LoraFreq = LoraFreq_nominal ;
      };  // end of txOnly
    
    SchedExitRX_BeaconMode((byte *)BCN_Payload, BCN_PayloadLen, EncapType, schedTimeExitRXBeaconMode , BCN_TimeSlot[BCN_TimeSlotSync] * 1000, BCN_cfg_tuning ) ;     
}

void BeaconSequencer_single( uint32_t phase_length ){
    uint32_t phase_start=500;  // milliseconds for starting a possible TX phase
    
    time_t t = UTC.now();  // time now
    if(LoRa_debug) debugA("\r---------%ld----------> BeaconSequencer_single: starting: phase_start=%d - phase_length=%d - BCN_SeqNbr=%d ", t, phase_start, phase_length, BCN_SeqNbr);
    if(LoRa_debug) debugA("==> hour=%d - minutes=%d - seconds=%d - msec=%d\r", hour(), minute(), second(),ms());
    // first of all set loRa working conditions and next sequence step
    int present_BCN_SeqNbr = BCN_SeqNbr;  // save present BCN sequence number 
    BCN_SeqNbr++;

//  Setup Lora according to the singlePhase setup BCN_LoRa_Vector
//  BeaconRunTimeConfig BCN_cfg_single = {0,  433.775,  7.8,   11, 8, 0x34, 10, 0, 15} ;

    BeaconRunTimeConfig BCN_cfg_single  ;
    textToVector( BCN_LoRa_Vector , BCN_cfg_single);

    if(LoRa_debug) debugA("BeaconSequencer_single: BCN_LoRa_Vector=%s \r", BCN_LoRa_Vector.c_str() ); 
    LoRa_Set_BCN( BCN_cfg_single );   // setup LoRa as per RX 
    
    byte BCN_Payload[40]; int BCN_PayloadLen = 0 ;
    BCN_PayloadLen = BeaconEncode( BCN_cfg_single,(byte *)BCN_Payload );   // create present spot binary payload 
    
    uint8_t EncapType = 0; // native packets no encapsulation  
    // 0,  433.775,  10.4,   12, 7, 0x34, 22, 0, 8  ogni 3 minuti mx 4 nodi 80 sec ogn 180 per traffico aprs
    uint32_t r ;  // milliseconds from phase start to begin TX part if requested
    if (phase_length == 100){
//       r = phase_start + ( BeaconId -1) * phase_length * 1000;
       r = phase_start + ( BeaconId -1) *23000;   // max BeaconId = 4; range 1-4
       }
    else{
       r = phase_start ;
       };   
    uint32_t schedTimeEnterRXBeaconMode = GetNextSlice(BCN_TimeSlot[BCN_TimeSlotSync], BCN_TimeSlotOffset);
    uint32_t TX_schedTime = schedTimeEnterRXBeaconMode + r ;
    uint32_t schedTimeExitRXBeaconMode = schedTimeEnterRXBeaconMode + phase_length*1000;
    uint32_t schedTimeSendPeriodicBeacon = schedTimeExitRXBeaconMode + 1000 + (BeaconId - 1)*18000;

    SchedEnterRX_BeaconMode((byte *)BCN_Payload, BCN_PayloadLen, EncapType, schedTimeEnterRXBeaconMode , BCN_TimeSlot[BCN_TimeSlotSync] * 1000, BCN_cfg_single ) ;
    if( BeaconEngineType == 0){   // txOnly that is TX part is requested 
        loraPrsService.SchedNativeToLora_noCAD((byte *)BCN_Payload, BCN_PayloadLen, EncapType, TX_schedTime  , BCN_TimeSlot[BCN_TimeSlotSync] * 1000, BCN_cfg_single, false ) ;
        };
    SchedExitRX_BeaconMode((byte *)BCN_Payload, BCN_PayloadLen, EncapType, schedTimeExitRXBeaconMode , BCN_TimeSlot[BCN_TimeSlotSync] * 1000, BCN_cfg_single ) ;
    SchedSendPeriodicBeacon((byte *)BCN_Payload, BCN_PayloadLen, EncapType, schedTimeSendPeriodicBeacon , BCN_TimeSlot[BCN_TimeSlotSync] * 1000, BCN_cfg_single ) ;

}



void BeaconSequencer_short(void){
    int delta_time; int phase=0 ;
    time_t t = UTC.now();  // time now
    debugA("\r---------%ld----------> BeaconSequencer_short: starting... BCN_SeqNbr=%d ", t, BCN_SeqNbr);
    debugA("==> hour=%d - minutes=%d - seconds=%d - msec=%d\r", hour(), minute(), second(),ms());
    // first of all set loRa working conditions and next sequence step
    int present_BCN_SeqNbr = BCN_SeqNbr;  // save preset BCN sequence number 
/*    
    if (BCN_SeqNbr == 58) {phase=8 ; BCN_SeqNbr = 1000  ; delta_time = 2; }; // send 1 pkt at BW=125Khz SF=12   - 1,3sec on air CR=4:5 / 1,7 CR=4:8
    if (BCN_SeqNbr == 56) {phase=7 ; BCN_SeqNbr = 58 ; delta_time = 2; };   // send 1 pkt at BW=62.5 Khz SF=12 - 2,6sec on air CR=4:5 / 3,3 CR=4:8
    if (BCN_SeqNbr == 52) {phase=6 ; BCN_SeqNbr = 56 ; delta_time = 4; };   // send 1 pkt at BW=62.5 Khz SF=12 - 2,6sec on air CR=4:5 / 3,3 CR=4:8
    if (BCN_SeqNbr == 51) {phase=5 ; BCN_SeqNbr = 52 ; delta_time = 1; };   // send 1 pkt at BW=31.25Khz SF=6  - 0,2sec on air CR=4:5 / 0,2  CR=4:8
    if (BCN_SeqNbr == 50) {phase=4 ; BCN_SeqNbr = 51 ; delta_time = 1; };   // send 1 pkt at BW=31.25Khz SF=7  - 0,3sec on air CR=4:5 / 0,4  CR=4:8
    if (BCN_SeqNbr == 48) {phase=3 ; BCN_SeqNbr = 50 ; delta_time = 2; };   // send 1 pkt at BW=31.25Khz SF=9  - 0,8sec on air CR=4:5 / 0,95  CR=4:8
    if (BCN_SeqNbr == 41) {phase=2 ; BCN_SeqNbr = 48 ; delta_time = 7; };   // send 1 pkt at BW=31.25Khz SF=12 - 5,1sec on air CR=4:5 / 6,6 CR=4:8
    if (BCN_SeqNbr == 27) {phase=1 ; BCN_SeqNbr = 41 ; delta_time = 14; };   // send 1 pkt at BW=15.6Khz SF=12  - 10,3sec on air CR=4:5 / 13,2 CR=4:8
    if (BCN_SeqNbr == 0)  {phase=0 ; BCN_SeqNbr = 27 ; delta_time = 27; };   // send 1 pkt at BW=7.8Khz SF=12   - 20,1sec on air CR=4:5 / 26,3 CR=4:8
    if (BCN_SeqNbr == 1000)  {phase=8 ; BCN_SeqNbr = 0 ;};
*/

    if (BCN_SeqNbr == 116) {phase=8 ; BCN_SeqNbr = 1000  ; delta_time = 4; }; // send 1 pkt at BW=125Khz SF=12   - 1,3sec on air CR=4:5 / 1,7 CR=4:8
    if (BCN_SeqNbr == 112) {phase=7 ; BCN_SeqNbr = 116 ; delta_time = 4; };   // send 1 pkt at BW=62.5 Khz SF=12 - 2,6sec on air CR=4:5 / 3,3 CR=4:8
    if (BCN_SeqNbr == 104) {phase=6 ; BCN_SeqNbr = 112 ; delta_time = 8; };   // send 1 pkt at BW=62.5 Khz SF=12 - 2,6sec on air CR=4:5 / 3,3 CR=4:8
    if (BCN_SeqNbr == 102) {phase=5 ; BCN_SeqNbr = 104 ; delta_time = 2; };   // send 1 pkt at BW=31.25Khz SF=6  - 0,2sec on air CR=4:5 / 0,2  CR=4:8
    if (BCN_SeqNbr == 100) {phase=4 ; BCN_SeqNbr = 102 ; delta_time = 2; };   // send 1 pkt at BW=31.25Khz SF=7  - 0,3sec on air CR=4:5 / 0,4  CR=4:8
    if (BCN_SeqNbr == 96) {phase=3 ; BCN_SeqNbr = 100 ; delta_time = 4; };   // send 1 pkt at BW=31.25Khz SF=9  - 0,8sec on air CR=4:5 / 0,95  CR=4:8
    if (BCN_SeqNbr == 82) {phase=2 ; BCN_SeqNbr = 96 ; delta_time = 14; };   // send 1 pkt at BW=31.25Khz SF=12 - 5,1sec on air CR=4:5 / 6,6 CR=4:8
    if (BCN_SeqNbr == 54) {phase=1 ; BCN_SeqNbr = 82 ; delta_time = 28; };   // send 1 pkt at BW=15.6Khz SF=12  - 10,3sec on air CR=4:5 / 13,2 CR=4:8
    if (BCN_SeqNbr == 0)  {phase=0 ; BCN_SeqNbr = 54 ; delta_time = 54; };   // send 1 pkt at BW=7.8Khz SF=12   - 20,1sec on air CR=4:5 / 26,3 CR=4:8
    if (BCN_SeqNbr == 1000)  {phase=8 ; BCN_SeqNbr = 0 ;};

    setEvent( BeaconSequencer_short, UTC.now() + delta_time );

    if( ! phase ) {
//    get_slot_infos();  
      };
    LoRa_Set_BCN( BCN_cfg[phase]);   // setup LoRa as per RX 
    byte BCN_Payload[40]; int BCN_PayloadLen = 0 ;
    BCN_PayloadLen = BeaconEncode( BCN_cfg[phase], (byte *)BCN_Payload  );   // create present spot binary payload 

    if (phase <=1 ) {
       vTaskDelay(pdMS_TO_TICKS(1000)); // wait 1000 msec
       }
    else {
       vTaskDelay(pdMS_TO_TICKS(300)); 
      };
      
   uint8_t EncapType = 0;  // native packets no encapsulation
   if( BeaconEngineType == 0){   // txOnly
//    sendPeriodicShortBeacon_noCAD(); // do TX
//    LDH_sendNativeToLora_noCAD((uint8_t *)BCN_Payload.c_str(), BCN_Payload.length(), EncapType, true ) ;// send our beacon binary message
    LDH_sendNativeToLora_noCAD((byte *)BCN_Payload , BCN_PayloadLen, EncapType, true , false ) ;// send our beacon binary message
    };
}


void BeaconSequencer_long(void){

}


void setupBeaconMode(){
   debugA("setupBeaconMode: starting Beacon_Mode...\r");
   vTaskDelay(pdMS_TO_TICKS(10000));
   int i=0;

   if(BeaconRun == 0){  // single phase run
     debugA("--> BeaconRun == 0   BCN_SeqNbr=%d - LoraFreq=%08.4f - LoraBw=%08.4f - LoraSf=%d - LoraCodingRate=%d - LoraSync=%02X - LoraPower=%d - LoraFreqCorr=%d - LoraPreambleLen=%d \r", BCN_cfg[i].BCN_SeqNbr, BCN_cfg[i].LoraFreq, BCN_cfg[i].LoraBw, BCN_cfg[i].LoraSf, BCN_cfg[i].LoraCodingRate, BCN_cfg[i].LoraSync, BCN_cfg[i].LoraPower, BCN_cfg[i].LoraFreqCorr, BCN_cfg[i].LoraPreambleLen );   
     BeaconSequencer_single( 10 );
     }
   else if(BeaconRun == 1){  // single phase run
     debugA("--> BeaconRun == 1   BCN_SeqNbr=%d - LoraFreq=%08.4f - LoraBw=%08.4f - LoraSf=%d - LoraCodingRate=%d - LoraSync=%02X - LoraPower=%d - LoraFreqCorr=%d - LoraPreambleLen=%d \r", BCN_cfg[i].BCN_SeqNbr, BCN_cfg[i].LoraFreq, BCN_cfg[i].LoraBw, BCN_cfg[i].LoraSf, BCN_cfg[i].LoraCodingRate, BCN_cfg[i].LoraSync, BCN_cfg[i].LoraPower, BCN_cfg[i].LoraFreqCorr, BCN_cfg[i].LoraPreambleLen );   
     BeaconSequencer_single( 18 );
     }
   else if(BeaconRun == 2){  // single phase run
     debugA("--> BeaconRun == 2   BCN_SeqNbr=%d - LoraFreq=%08.4f - LoraBw=%08.4f - LoraSf=%d - LoraCodingRate=%d - LoraSync=%02X - LoraPower=%d - LoraFreqCorr=%d - LoraPreambleLen=%d \r", BCN_cfg[i].BCN_SeqNbr, BCN_cfg[i].LoraFreq, BCN_cfg[i].LoraBw, BCN_cfg[i].LoraSf, BCN_cfg[i].LoraCodingRate, BCN_cfg[i].LoraSync, BCN_cfg[i].LoraPower, BCN_cfg[i].LoraFreqCorr, BCN_cfg[i].LoraPreambleLen );   
     BeaconSequencer_single( 23 );
     }
   else if(BeaconRun == 3){  // single phase run
     debugA("--> BeaconRun == 3   BCN_SeqNbr=%d - LoraFreq=%08.4f - LoraBw=%08.4f - LoraSf=%d - LoraCodingRate=%d - LoraSync=%02X - LoraPower=%d - LoraFreqCorr=%d - LoraPreambleLen=%d \r", BCN_cfg[i].BCN_SeqNbr, BCN_cfg[i].LoraFreq, BCN_cfg[i].LoraBw, BCN_cfg[i].LoraSf, BCN_cfg[i].LoraCodingRate, BCN_cfg[i].LoraSync, BCN_cfg[i].LoraPower, BCN_cfg[i].LoraFreqCorr, BCN_cfg[i].LoraPreambleLen );   
     BeaconSequencer_single( 100 );
     }          
   else if(BeaconRun == 4){
     while( i < 9){
       debugA("-->  BeaconRun == 4   BCN_SeqNbr=%d - LoraFreq=%08.4f - LoraBw=%08.4f - LoraSf=%d - LoraCodingRate=%d - LoraSync=%02X - LoraPower=%d - LoraFreqCorr=%d - LoraPreambleLen=%d \r", BCN_cfg[i].BCN_SeqNbr, BCN_cfg[i].LoraFreq, BCN_cfg[i].LoraBw, BCN_cfg[i].LoraSf, BCN_cfg[i].LoraCodingRate, BCN_cfg[i].LoraSync, BCN_cfg[i].LoraPower, BCN_cfg[i].LoraFreqCorr, BCN_cfg[i].LoraPreambleLen ); 
       LoRa_Set_BCN( BCN_cfg[i]);
       i++;
       };
     }
   else if(BeaconRun == 5){
     while( i < 9){
       debugA("--> BeaconRun == 5   BCN_SeqNbr=%d - LoraFreq=%08.4f - LoraBw=%08.4f - LoraSf=%d - LoraCodingRate=%d - LoraSync=%02X - LoraPower=%d - LoraFreqCorr=%d - LoraPreambleLen=%d \r", BCN_cfg[i].BCN_SeqNbr, BCN_cfg[i].LoraFreq, BCN_cfg[i].LoraBw, BCN_cfg[i].LoraSf, BCN_cfg[i].LoraCodingRate, BCN_cfg[i].LoraSync, BCN_cfg[i].LoraPower, BCN_cfg[i].LoraFreqCorr, BCN_cfg[i].LoraPreambleLen ); 
       LoRa_Set_BCN( BCN_cfg[i]);
       i++;
       };
     }   
   else if(BeaconRun == 6){  // tuningMode run
     debugA("--> BeaconRun == 6   BCN_SeqNbr=%d - LoraFreq=%08.4f - LoraBw=%08.4f - LoraSf=%d - LoraCodingRate=%d - LoraSync=%02X - LoraPower=%d - LoraFreqCorr=%d - LoraPreambleLen=%d \r", BCN_cfg[i].BCN_SeqNbr, BCN_cfg[i].LoraFreq, BCN_cfg[i].LoraBw, BCN_cfg[i].LoraSf, BCN_cfg[i].LoraCodingRate, BCN_cfg[i].LoraSync, BCN_cfg[i].LoraPower, BCN_cfg[i].LoraFreqCorr, BCN_cfg[i].LoraPreambleLen );   
     BeaconSequencer_tuning();
     };     
}

// manages LoRa Native_packets service startup and loop in a single main loop function to sync with other processes...

bool loop_Lora_native() {
    if(! Native_Service_flag || ! Beacon_Mode  || ( op_status == 0)  ) return (false) ;   // native packet apps are unavailable or still under initialization... just return...
         
    if(Native_Service_initialized) {   // service is correctly initialized.. juts go loop component of the service
       native_loop();   // performs native and native park quee scanning and perfor native transmission tasks
       }
    else if (APRS_Service_initialized){
      debugA("loop_Lora_native: starting native packet services....\r"); 
      setupBeaconMode();     
      Native_Service_initialized = true ;
      debugA("loop_Lora_native: native packet services up&running....\r");       
      };
       
    return(true);
}


void LoRaSer_demo(void) {
    byte mydata[40]; int mydata_len;
  
    debugA( "main_encoder Test start...\r" );
    LoraEncoder encoder(mydata);
    encoder.writeUnixtime(1468075322);
    encoder.writeLatLng(-33.905024, 151.26648);

    debugA("time=%ld lat=%08.4f lon=%08.4f\r", 1468075322, -33.905024, 151.26648 );
    int i=0; while(i < mydata_len ){if(! (i%16))debugAl("\r")  ; debugAl("%02X ", mydata[i]); i++;   }; debugAl("\r\r");

}


void LoRaBeacon_msg(void) {
  LoraMessage message;

  message
    .addUnixtime(1467632413)
    .addLatLng(-33.905052, 151.26641);

//  lora_send_bytes(message.getBytes(), message.getLength());
//  delete message;
}


void test_beacon(){
    requirePeriodicBeacon = true;
} 



bool SchedEnterRX_BeaconMode(byte *txBuf, int txBufIndex, uint8_t Encap , uint32_t schedTime , uint32_t repTime , struct BeaconRunTimeConfig &BCN_vector) {
      LDH_Message_v2 LDM_Msg_v2 ;
  //  if(LoRa_debug)debugA("SchedEnterRX_BeaconMode: ===>>>>>>  IPC request to park packet for later transmission\r");
      // send message to Beacon Sequencer xQueue_native_park to enter RX_Beacon_Mode
      LDM_Msg_v2.MsgType = 5 ;      // 3=transmit pkt (auto CAD selection) 4=transmit pkt no CAD 5=enter RX_Beacon_Mode 6=exit RX_Beacon_Mpode
      LDM_Msg_v2.MsgSqNbr = MsgStdSeqNbr++ ;   // use MsgStdSeqNbr to tag this message and autoincrement
      LDM_Msg_v2.Lv = BCN_vector ;
      LDM_Msg_v2.ReschedTime = repTime ;
      LDM_Msg_v2.dueTime = schedTime ;      
      LDM_Msg_v2.ack = 0 ;          // indicates a query packet      
      memcpy(LDM_Msg_v2.bufData, txBuf, txBufIndex );  // fill buffer with packet content to transmit
      LDM_Msg_v2.bufLen = txBufIndex ;                 // fill with packet size
      LDM_Msg_v2.SigRep[0] = (uint8_t) Encap ;         // we use this field here to signal packet encap for statistics     
      // send LDM_Msg_v2 to parking queue to perform delayed packet transmission
      if(LoRa_debug) debugA("SchedEnterRX_BeaconMode: enqueue  packet to xQueue_nativepark....\r" );
//    if(LoRa_debug){ int i=0; while(i < LDM_Msg_v2.bufLen ){if(! (i%16))debugAl("\r")  ; debugAl("%02X ", LDM_Msg_v2.bufData[i]); i++;   }; debugAl("\r");};

      int messagesWaiting ; int emptySpaces ;      
      if( xQueueSend( xQueue_native_park, (void *)&LDM_Msg_v2, ( TickType_t ) 10 ) != pdPASS ) {
         if(LoRa_debug) debugA("SchedEnterRX_BeaconMode: unable to enqueue message toward xQueue_native_park....\r");
         // TX packet lost due to IPC fault
         IPC_lost_msgs++ ;
         return(false);
         }; 
      messagesWaiting = uxQueueMessagesWaiting(xQueue_native_park);
      emptySpaces = uxQueueSpacesAvailable(xQueue_native_park);      
      if(LoRa_debug) debugA("SchedEnterRX_BeaconMode: on exit ===>>>>>>  IPC xQueue_native_park:  messagesWaiting=%d - emptySpaces=%d - LoRa_ReSched_packets=%d\r", messagesWaiting, emptySpaces, LoRa_ReSched_packets + 1);                   
      LoRa_ReSched_packets++;
      return(true);
}  // end of SchedEnterRX_BeaconMode


bool SchedExitRX_BeaconMode(byte *txBuf, int txBufIndex, uint8_t Encap , uint32_t schedTime , uint32_t repTime , struct BeaconRunTimeConfig &BCN_vector) {
      LDH_Message_v2 LDM_Msg_v2 ;
      if(LoRa_debug) debugA("SchedExitRX_BeaconMode: =====>>>>>>  IPC request to park packet for later transmission\r");
      // send message to Beacon Sequencer xQueue_native_park to enter RX_Beacon_Mode
      LDM_Msg_v2.MsgType = 6 ;      // 3=transmit pkt (auto CAD selection) 4=transmit pkt no CAD 5=enter RX_Beacon_Mode 6=exit RX_Beacon_Mpode
      LDM_Msg_v2.MsgSqNbr = MsgStdSeqNbr++ ;   // use MsgStdSeqNbr to tag this message and autoincrement
      LDM_Msg_v2.Lv = BCN_vector ;
      LDM_Msg_v2.ReschedTime = repTime ;
      LDM_Msg_v2.dueTime = schedTime ;      
      LDM_Msg_v2.ack = 0 ;          // indicates a query packet      
      memcpy(LDM_Msg_v2.bufData, txBuf, txBufIndex );  // fill buffer with packet content to transmit
      LDM_Msg_v2.bufLen = txBufIndex ;                 // fill with packet size
      LDM_Msg_v2.SigRep[0] = (uint8_t) Encap ;         // we use this field here to signal packet encap for statistics     
      // send LDM_Msg_v2 to parking queue to perform delayed packet transmission
      if(LoRa_debug) debugA("SchedExitRX_BeaconMode: enqueue  packet to xQueue_nativepark....\r" );
//    if(LoRa_debug){ int i=0; while(i < LDM_Msg_v2.bufLen ){if(! (i%16))debugAl("\r")  ; debugAl("%02X ", LDM_Msg_v2.bufData[i]); i++;   }; debugAl("\r");};

      int messagesWaiting ; int emptySpaces ;      
      if( xQueueSend( xQueue_native_park, (void *)&LDM_Msg_v2, ( TickType_t ) 10 ) != pdPASS ) {
         if(LoRa_debug) debugA("SchedExitRX_BeaconMode: unable to enqueue message toward xQueue_native_park....\r");
         // TX packet lost due to IPC fault
         IPC_lost_msgs++ ;
         return(false);
         }; 
      messagesWaiting = uxQueueMessagesWaiting(xQueue_native_park);
      emptySpaces = uxQueueSpacesAvailable(xQueue_native_park);      
      if(LoRa_debug) debugA("SchedExitRX_BeaconMode: on exit ===>>>>>>  IPC xQueue_native_park:  messagesWaiting=%d - emptySpaces=%d - LoRa_ReSched_packets=%d\r", messagesWaiting, emptySpaces, LoRa_ReSched_packets + 1);                   
      LoRa_ReSched_packets++;
      return(true);
}  // end of SchedEnterRX_BeaconMode

bool SchedSendPeriodicBeacon(byte *txBuf, int txBufIndex, uint8_t Encap , uint32_t schedTime , uint32_t repTime , struct BeaconRunTimeConfig &BCN_vector) {
      LDH_Message_v2 LDM_Msg_v2 ;
      if(LoRa_debug) debugA("SchedSendPeriodicBeacon: =====>>>>>>  IPC request to park packet for later transmission\r");
      // send message to Beacon Sequencer xQueue_native_park to enter RX_Beacon_Mode
      LDM_Msg_v2.MsgType = 7 ;      // 3=transmit pkt (auto CAD selection) 4=transmit pkt no CAD 5=enter RX_Beacon_Mode 6=exit RX_Beacon_Mpode  7=SendAPRSPeriodicBeacon
      LDM_Msg_v2.MsgSqNbr = MsgStdSeqNbr++ ;   // use MsgStdSeqNbr to tag this message and autoincrement
      LDM_Msg_v2.Lv = BCN_vector ;
      LDM_Msg_v2.ReschedTime = repTime ;
      LDM_Msg_v2.dueTime = schedTime ;      
      LDM_Msg_v2.ack = 0 ;          // indicates a query packet      
      memcpy(LDM_Msg_v2.bufData, txBuf, txBufIndex );  // fill buffer with packet content to transmit
      LDM_Msg_v2.bufLen = txBufIndex ;                 // fill with packet size
      LDM_Msg_v2.SigRep[0] = (uint8_t) Encap ;         // we use this field here to signal packet encap for statistics     
      // send LDM_Msg_v2 to parking queue to perform delayed packet transmission
      if(LoRa_debug) debugA("SchedSendPeriodicBeacon: enqueue  packet to xQueue_nativepark....\r" );
//    if(LoRa_debug){ int i=0; while(i < LDM_Msg_v2.bufLen ){if(! (i%16))debugAl("\r")  ; debugAl("%02X ", LDM_Msg_v2.bufData[i]); i++;   }; debugAl("\r");};

      int messagesWaiting ; int emptySpaces ;      
      if( xQueueSend( xQueue_native_park, (void *)&LDM_Msg_v2, ( TickType_t ) 10 ) != pdPASS ) {
         if(LoRa_debug) debugA("SchedSendPeriodicBeacon: unable to enqueue message toward xQueue_native_park....\r");
         // TX packet lost due to IPC fault
         IPC_lost_msgs++ ;
         return(false);
         }; 
      messagesWaiting = uxQueueMessagesWaiting(xQueue_native_park);
      emptySpaces = uxQueueSpacesAvailable(xQueue_native_park);      
      if(LoRa_debug) debugA("SchedSendPeriodicBeacon: on exit ===>>>>>>  IPC xQueue_native_park:  messagesWaiting=%d - emptySpaces=%d - LoRa_ReSched_packets=%d\r", messagesWaiting, emptySpaces, LoRa_ReSched_packets + 1);                   
      LoRa_ReSched_packets++;
      return(true);
}  // end of SchedSendPeriodicBeacon



