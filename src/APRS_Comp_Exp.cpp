/*
 APRS_Compress/APRS_Expand
 by I8FUC 2023
*/

#include <APRS-Decoder.h>
#include <Arduino.h>
#include <TimeLib.h>
#include <TinyGPS++.h>
#include <LoraEncoder.h>
#include "BCN_Structs.h"

extern volatile byte LocationCompression ;
extern byte BeaconId ;
extern bool BeaconSeqNbr;
extern bool BeaconPower ;   //
extern bool BeaconWorkConditions ; //

extern bool LoRa_debug ;
extern uint8_t APRS_MsgSN ;
extern BeaconRunTimeConfig BCN_APRS_cfg  ;

extern float BWindx[]  ; 
extern uint8_t SFindx[] ;
extern uint8_t CRindx[] ;
extern uint8_t PrLenindx[] ;


String  APRS_WCDecode( byte *rxBuf_BCN, const int bufferSize); 
int  APRS_WCEncode( byte outBuf_BCN[]);  


String APRS_Compress(const String & message) ;
String APRS_Expand(const String & message) ;


char *ax25_base91enc(char *s, uint8_t n, uint32_t v);
String createDateString(time_t t);
String createTimeString(time_t t);
String padding(unsigned int number, unsigned int width);

bool send_update = true ;
bool gps_loc_update = true ;
//static int      speed_zero_sent = 0;
int speed_nodes ;

/*
void setup() {
  Serial.begin(115200);
  delay(500);
  Serial.println("APRS_Comp Vr. 1.0");
  delay(500);
  padding(15,4);
  APRS_Compress("IQ8SO-10>APLS01,WIDE1-1,qAR,IZ8FPF-11:!4038.68N/01424.57E&LoRa-OE, 125/SF12 - rssi:-113- snr:-17.75 ");

}

void loop() {
  Serial.printf("\r\n\r\n=====> APRS_Comp_loop input message:  %s\r\n","IQ8SO-10>APLS01,WIDE1-1,qAR,IZ8FPF-11:!4038.68N/01424.57E&LoRa-OE, 125/SF12 - rssi:-113- snr:-17.75 ");

  String cMessage = APRS_Compress("IQ8SO-10>APLS01,WIDE1-1,qAR,IZ8FPF-11:!4038.68N/01424.57E&LoRa-OE, 125/SF12 - rssi:-113- snr:-17.75 ");
  Serial.printf(        "=====> APRS_Comp_loop CompreMessage:  %s\r\n", cMessage.c_str());

  String newDecMessage = APRS_Expand (cMessage); 
  Serial.printf(        "=====> APRS_Comp_loop ExpndedMessage: %s\r\n", newDecMessage.c_str());

delay(10000);
}
*/

/// FUNCTIONS ///


bool isNumeric(String str) {
    unsigned int stringLength = str.length();
    for(unsigned int i = 0; i < stringLength; ++i) {
        if (isDigit(str.charAt(i))) {
            return true ;
        }
    }
        return false ;
}


String APRS_Compress(const String & message) {   // compress the message contained in the input string producing an output string
//  Serial.printf("\r\nAPRS_Compress: +++> input message= %s\r\n", message.c_str());
	APRSMessage msg;
	msg.decode(message);
//	Serial.printf("\r\n====> msg.toString=%s\r\n",msg.toString().c_str());
 
  String msg_raw_body = msg.getRawBody() ;
//  Serial.printf("--> RawBody=%s\r\n",msg.getRawBody().c_str());
  String body_type =  msg_raw_body.substring(0, 1);  //  Serial.println (body_type);
  String Loc_Lat =  msg_raw_body.substring(1, 9); // Serial.println (Loc_Lat);
  String symbol_table =  msg_raw_body.substring(9, 10); // Serial.println (symbol_table);
  String Loc_Lon =  msg_raw_body.substring(10, 19); // Serial.println (Loc_Lon);
  String symbol_code =  msg_raw_body.substring(19, 20) ;  // Serial.println (symbol_code);
  String sym_comp =  msg_raw_body.substring(1, 2) ;  // Serial.println ;

  if ( ! LocationCompression ) {
      Serial.printf ("APRS_Compress >>>>  LocationCompression is disabled on this node....\r\n" );
      return(message) ;
  }

  Serial.printf ("APRS_Compress >>>> sym_comp =%s\r\n", sym_comp.c_str());
  if( ! isNumeric (sym_comp) ) {
      Serial.printf ("APRS_Compress >>>>  message is ALREADY compressed....\r\n" );
      return(message) ;
  }
  Serial.printf ("APRS_Compress >>>>  message NOT compressed  ... compressing  ....\r\n" );

  String comment =  msg_raw_body.substring(20 ); // Serial.println (comment);
  String location= body_type + Loc_Lat + symbol_table  + Loc_Lon + symbol_code  ;
//  Serial.printf("----------------------=====> original location = %s  --> len=%d \r\n", location.c_str(), strlen(location.c_str()));

  // Serial.println ("");
  // translate APRS position in decimal position (float)
  String mylat_int = Loc_Lat.substring(0, 2) ; String  mylat_frac =Loc_Lat.substring(2, 7) ; float mylat_norm = mylat_int.toFloat() + mylat_frac.toFloat() /60.0 ;
  if ( Loc_Lat.substring(7) == "S") {mylat_norm = - mylat_norm ;};
  String mylon_int = Loc_Lon.substring(0, 3) ; String  mylon_frac =Loc_Lon.substring(3, 8) ; float mylon_norm = mylon_int.toFloat() + mylon_frac.toFloat() /60.0 ;
  if ( Loc_Lon.substring(8) == "W") {mylon_norm = - mylon_norm ;};  
//  Serial.printf("===> mylat_norm=%10.7f  mylon_norm=%10.7f\r\n", mylat_norm , mylon_norm);
  float course_norm = 0.0 ;
  float speed_norm = 0.0 ;
  uint32_t aprs_lat, aprs_lon;
  aprs_lat = 900000000 - mylat_norm * 10000000;
  aprs_lat = aprs_lat / 26 - aprs_lat / 2710 + aprs_lat / 15384615;
  aprs_lon = 900000000 + mylon_norm * 10000000 / 2;
  aprs_lon = aprs_lon / 26 - aprs_lon / 2710 + aprs_lon / 15384615;

  String Ns, Ew, helper;
  if(mylat_norm < 0) { Ns = "S"; } else { Ns = "N"; }
  if(mylat_norm < 0) { mylat_norm= -mylat_norm; }

  if(mylon_norm < 0) { Ew = "W"; } else { Ew = "E"; }
  if(mylon_norm < 0) { mylon_norm= -mylon_norm; }

  String infoField = "!";    // Data Type ID 
//  infoField += "/" ;         // info_field symbol_table_identifier ( table selector)
  infoField += "L" ;         // info_field symbol_table_identifier ( table selector)
                           
  char helper_base91[] = {"0000\0"};
  int i;
  ax25_base91enc(helper_base91, 4, aprs_lat);
  for (i=0; i<4; i++) {
    infoField += helper_base91[i];
    }
  ax25_base91enc(helper_base91, 4, aprs_lon);
  for (i=0; i<4; i++) {
    infoField += helper_base91[i];
    }
//  infoField += "[";  // info_field_symbol // info_field symbol_code  ( i.e. house icon )
  infoField += "&";  // info_field_symbol // info_field symbol_code  ( i.e. house icon )


  ax25_base91enc(helper_base91, 1, (uint32_t) course_norm/4 );
  infoField += helper_base91[0];
  ax25_base91enc(helper_base91, 1, (uint32_t) (log1p(speed_norm)/0.07696));
  infoField += helper_base91[0];
  infoField += "\x47";    // Compression Type (T) Byte = \b001 00 110 = 38; 38 + 33 = 72 = \x47 = G
  String clocation = infoField ;
//  Serial.printf("+++++++++++++++++++++++++++++=====> compressed location = %s  --> len=%d\r\n",clocation.c_str() , strlen(clocation.c_str()));
  infoField += comment ;
//  Serial.printf("=====> compressed  infoField= %s\r\n",infoField.c_str());
  msg.getBody()->setData(infoField);
//  Serial.printf("-----> compressed_msg_body=%s\r\n" ,msg.toString().c_str());
  String data_out = msg.encode();
//  Serial.printf("\r\n=====> output compressed_msg_encoded= %s\r\n",data_out.c_str());

  String msg_out="Success\r\n\r\n\r\n";
  return(data_out.c_str()) ;  
};





String APRS_Expand(const String & message) {   // de-compress the message contained in the input string producing an output string
//  Serial.printf("\r\nAPRS_Expand:  +++> input message= %s\r\n", message.c_str());
	APRSMessage msg;
	msg.decode(message);
//	Serial.println(msg.toString());

  String msg_raw_body = msg.getRawBody() ;
//  Serial.printf("--> RawBody=%s\r\n",msg.getRawBody().c_str());

  // we have to extract compress location and decompress it, then re-create uncompressed message to give back
  String cLocation = msg_raw_body.substring(0, 13);  
//  Serial.printf (">>>> cLocation=%s\r\n", cLocation.c_str());
  String comment =  msg_raw_body.substring(14); 
//  Serial.printf (">>>> comment=%s\r\n\r\n\r\n",comment.c_str());

  String cl_symtable = msg_raw_body.substring(0,1);  // Serial.println (cl_symtable);
  String cl_symcode = msg_raw_body.substring(1,2); // Serial.println (cl_symcode);

  Serial.printf (">>>> cl_symcode =%s  cl_symtable=%s \r\n", cl_symcode.c_str() , cl_symtable.c_str() );
  if ( isNumeric (cl_symcode) ) {    
      Serial.printf (">>>>  message is NOT compressed....\r\n" );
      return(message) ;
  }
  Serial.printf (">>>>  message is compressed  ... expanding ....\r\n" );

  String cl_lat = msg_raw_body.substring(2,6); // Serial.println (cl_lat);
  String cl_lon = msg_raw_body.substring(6,10); // Serial.println (cl_lon);
  String cl_course = msg_raw_body.substring(10,11); // Serial.println (cl_course);
  String cl_speed = msg_raw_body.substring(11,12); // Serial.println (cl_speed);
  String cl_ctype = msg_raw_body.substring(12,13); // Serial.println (cl_ctype);

  // Lat = 90 - ((y1-33) x 913 + (y2-33) x 912 + (y3-33) x 91 + y4-33) / 380926
  // Long = -180 + ((x1-33) x 913 + (x2-33) x 912 + (x3-33) x 91 + x4-33) / 190463
  int y1 = int(cl_lat[0]); int y2 = int(cl_lat[1]); int y3 = int(cl_lat[2]); int y4 = int(cl_lat[3]); 
  float Lat = 90.0 - ((y1-33) * 91*91*91 + (y2-33) * 91*91 + (y3-33) * 91.0 + y4-33) / 380926.0 ;
  int x1 = int(cl_lon[0]); int x2 = int(cl_lon[1]); int x3 = int(cl_lon[2]); int x4 = int(cl_lon[3]); 
  float Lon = -180.0 + ((x1-33) * 91*91*91 + (x2-33) * 91*91 + (x3-33) * 91.0 + x4-33) / 190463.0 ;
//  printf("-----------> Lat=%f , Lon=%f\r\n\r\n", Lat, Lon  );
  char TEMP[16];
//  if(Lat < 0) { sprintf(TEMP,"%10.7fS",Lat) ; } else { sprintf(TEMP,"%10.7fN",Lat) ;}
  if(Lat < 0) { 
    sprintf(TEMP,"%02d%05.2fS",int(Lat) , (Lat - int(Lat)) * 60) ; 
    } 
  else { 
    sprintf(TEMP,"%02d%05.2fN",int(Lat) , (Lat - int(Lat)) * 60) ; 
    } ;
  String sLat = TEMP;

//  if(Lon < 0) { sprintf(TEMP,"%011.7fW",Lon) ; } else { sprintf(TEMP,"%011.7fE",Lon) ;}
  if(Lon < 0) { 
    sprintf(TEMP,"%03d%05.2fW",int(Lon) , (Lon - int(Lon)) * 60) ; 
    } 
  else { 
    sprintf(TEMP,"%03d%05.2fE",int(Lon) , (Lon - int(Lon)) * 60) ; 
    } ;
  String sLon = TEMP;

  String infoField = "!";    // Data Type ID 
  infoField += sLat ;  
  infoField += "/" ;         // info_field symbol_table_identifier ( table selector)
                             // info_field symbol_code  ( i.e. house icon )
  infoField += sLon ;  
  infoField += "&" ;    
  infoField += comment ;
//  Serial.printf("=====> expanded  infoField= %s\r\n",infoField.c_str());
  msg.getBody()->setData(infoField);

  String data_out = msg.encode();
  Serial.printf("=====> expanded_msg_encoded= %s\r\n",data_out.c_str());

  String msg_out="Success";
  return(data_out.c_str()) ;
};



char *ax25_base91enc(char *s, uint8_t n, uint32_t v){
  /* Creates a Base-91 representation of the value in v in the string */
  /* pointed to by s, n-characters long. String length should be n+1. */
//  Serial.printf("input_char=%s - length=%d - value=%d --> ", s , n , v);
  for(s += n, *s = '\0'; n; n--){
    *(--s) = v % 91 + 33;
    v /= 91;
    } ;
//  Serial.printf("result=%s\r\n", s);
  return(s);
}

String createDateString(time_t t) {
  return String(padding(year(t), 4) + "-" + padding(month(t), 2) + "-" + padding(day(t), 2));
}

String createTimeString(time_t t) {
  return String(padding(hour(t), 2) + ":" + padding(minute(t), 2) + ":" + padding(second(t), 2));
}


String padding(unsigned int number, unsigned int width) {
  String result;
  String num(number);
  if (num.length() > width) {
    width = num.length();
  }
  for (unsigned int i = 0; i < width - num.length(); i++) {
    result.concat('0');
  }
  result.concat(num);
//  Serial.printf("input=%d - width=%d --> result=%s\r",number, width, result.c_str());
  return result;
}


int  APRS_WCEncode( byte outBuf_BCN[]){   // (float loraFreq, float bw, byte sf, byte cr, int pwr, byte sync, byte prlen){
    byte mydata[40];
    if(LoRa_debug) debugA("APRS_WCEncode: starting....\r");
    uint8_t bwi=0; while( BWindx[bwi] != BCN_APRS_cfg.LoraBw ){ bwi++;};
    uint8_t sfi=0; while( SFindx[sfi] != BCN_APRS_cfg.LoraSf ){ sfi++;};
    uint8_t cri=0; while( CRindx[cri] != BCN_APRS_cfg.LoraCodingRate ){ cri++;};
    uint8_t pri=0; while( PrLenindx[pri] != BCN_APRS_cfg.LoraPreambleLen ){ pri++;};
    int pkt_len=0;
    LoraEncoder encoder(mydata);
    encoder.writeUint8('$'); 
    encoder.writeUint8(BeaconId); 
    if(LoRa_debug)  
    debugA("APRS_WCEncode: ================================> BeaconWorkConditions=%d BeaconPower=%d BeaconSeqNbr=%d \r" , BeaconWorkConditions , BeaconPower ,  BeaconSeqNbr);
    pkt_len = 2;   
    encoder.writeUint8(APRS_MsgSN); APRS_MsgSN++ ; pkt_len = pkt_len +1; 
    encoder.writeUint8(BCN_APRS_cfg.LoraPower + 128 ); pkt_len = pkt_len +1; 
    uint16_t workConditions=  bwi * 4096 + sfi *256 + cri *16 + pri ; 
    if(LoRa_debug)debugA("APRS_WCEncode: APRS_MsgSN=%d ===>  bw=%d - sf=%d - cr=%d - prlen=%d  ==> workConditions=0x%02X\r",APRS_MsgSN,bwi,sfi,cri,pri,workConditions);
    encoder.writeUint16(workConditions); pkt_len = pkt_len +2; // 2 bytes
    if(LoRa_debug) debugA("APRS_WCEncode: payload="); int i=0; while(i < pkt_len ){if(! (i%16))debugAl("\r")  ; debugAl("%02X ", mydata[i]); i++;   }; debugAl("\r");
    mydata[pkt_len + 1] = '\0';  // fix result length
    if(LoRa_debug) debugA("APRS_WCEncode: complete (pkt_len=%d)\r", pkt_len);
    memcpy(outBuf_BCN, mydata, pkt_len); 
    return ( pkt_len ); 
}


String  APRS_WCDecode( byte *rxBuf_BCN, const int bufferSize){   
    // (float loraFreq, float bw, byte sf, byte cr, int pwr, byte sync, byte prlen){
    // encoded data in rxBuf_BCN / packetSize 
    // decoded payload in String textpayload

    rxBuf_BCN[bufferSize+1] = '\0';  // fix input buffer length
    String xxx= String((char *)rxBuf_BCN);  // iput buffer as a string
    String textPayload = ""; int x ;
    int BCN_ptr =3 ;    
    int8_t pwr_int = 0; pwr_int = (uint8_t )rxBuf_BCN[BCN_ptr ]  - 128 ;  BCN_ptr++ ; 
    uint16_t wc_int = 0 ; wc_int = (rxBuf_BCN[BCN_ptr +1 ] << 8) + rxBuf_BCN[BCN_ptr ]; 
    char sz[32];
    sprintf( sz, "0x%02X", wc_int ); String wc_str = String((char *)sz); 
    textPayload = "Id=" + String(rxBuf_BCN[1]) + " SeqNbr=" + String(rxBuf_BCN[2]) +  " pwr=" + String(pwr_int) + " wc=" + wc_str ; 
    if(LoRa_debug) debugA("APRS_WCDecode: =======   pwr_int=%d   wc_str=%s ==================>  rxBuf_BCN-String=[%s] - payload=[%s]\r", pwr_int ,wc_str.c_str(), xxx.c_str(), textPayload.c_str()); 
    return (textPayload);
} 

