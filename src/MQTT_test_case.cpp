/*

per interagire tramite MQTT

http://www.hivemq.com/demos/websocket-client/

funzioni implementate
  - heartbeat from device
  - remote reboot command to device 

funzioni VPN da integrare ma pesano troppo...


per la VPN da sariserver 
cd cd /sec/LOCAL_SAVESET/MQTT_VPN/mqtt_vpn
./mqtt_vpn -i mq0 -a 10.0.1.1 -b tcp://broker.hivemq.com:1883 -k "auguri" -d -u micfuc2020 -p Papera_123

da un altra finestra 

ping 10.0.1.2
*/

#include "MQTT_test_case.h"

extern uint32_t HMQ_last_connect_trial ; 
extern int mqtt_unavailable  ;
extern volatile int32_t time_jitter ;

String get_device_id(void);


extern String LastFunction ;
extern uint32_t activity_time ;
extern volatile float ppm_runtime;
extern String RX_Last_Spot_level ;
extern String RX_Last_Spot_payload ;
extern String RX_Last_Path_Report ;
extern volatile uint32_t IPC_lost_msgs ;
extern volatile uint32_t LoRa_lost_packets ;
extern volatile uint32_t LoRa_rx_packets ;
extern volatile uint32_t LoRa_tx_packets ;
extern volatile uint32_t AprsIS_rx_packets ;
extern volatile uint32_t AprsIS_tx_packets ;
extern volatile uint32_t AprsIS_relayed_packets ;
extern volatile uint32_t LoRa_rx_AX25_packets ;
extern volatile uint32_t LoRa_tx_AX25_packets ;
extern volatile uint32_t LoRa_rx_OEStyle_packets ;
extern volatile uint32_t LoRa_tx_OEStyle_packets ;
extern volatile uint32_t LoRa_rx_native_packets ;
extern volatile uint32_t LoRa_tx_native_packets ;
extern volatile uint32_t LoRa_ReSched_packets;
extern volatile uint32_t LoRa_CAD_errors;
extern volatile float ChanCong , LoRa_ENL;

extern volatile uint32_t AprsIS_dropped_packets ;
extern volatile uint32_t LoRa_CRC_errored_packets;
extern volatile uint32_t LoRa_CRC_errored_packets_BCN;

extern volatile uint32_t LoRa_UMN_errored_packets;

extern volatile uint32_t LoRa_OnAirTime ;
extern volatile float LoRa_DutyCycle  ;
extern volatile uint32_t LoRa_OnAirTime_Tot ;

extern String SW_Revision ; 

extern volatile float cpu_temp ;
extern String lat_now,lon_now;
extern volatile int op_status;

extern volatile int op_status;
extern volatile int32_t time_jitter ;



//String lora_payload ;
extern volatile bool EnableRfToIs;
extern volatile bool EnableIsToRf;
extern volatile bool EnableRepeater ;
extern volatile bool BT_KISS_Mode ;
extern volatile bool Serial_KISS_Mode ;
extern volatile bool TCP_KISS_Mode ;

extern uint32_t activity_start ;

extern  int wifi_unavailable ;

#include "ezTime_loc.h"
extern Timezone UTC;
extern time_t now;

#include <TinyGPS++.h>
extern TinyGPSPlus gps;

#include <ArduinoJson.h>
#include "JsonConfigFile.h"
StaticJsonDocument<4096> doc;




void loop_mqtt() {   // only for Adafruit MQTT server 
  if( wifi_unavailable ) return; // MQTT is only available if we have an upstream connection to internet
#ifdef AIO_MQTT_ENABLE  
  //=========================== Adafruit MQTT service loop =====================================================
  // Ensure the connection to the MQTT server is alive (this will make the first
  // connection and automatically reconnect when disconnected).  See the MQTT_connect
  // function definition further below.
  Adafruit_MQTT_connect();
  // this is our 'wait for incoming subscription packets' busy subloop
  Adafruit_MQTT_Subscribe *subscription;
  while ((subscription = mqtt.readSubscription(1000))) {
    if (subscription == &onoffbutton) {
      debugA("loop_mqtt: AIO_MQTT Got: %s r n", (char *)onoffbutton.lastread);
      if(syslog_available)  syslog.logf("loop_mqtt: AIO_MQTT Got: [%s]", (char *)onoffbutton.lastread);
    }
  }

  // Now we can publish stuff!
  if (millis() - AIO_last_time > 60000) {    // publish every 60 secs
     debugA("loop_mqtt: AIO_MQTT Publishing photocell val %d...", AIO_cntr);
     if (! photocell.publish(AIO_cntr++)) {
        debugA("Failed\r\n");
        if(syslog_available)  syslog.logf("loop_mqtt: AIO_MQTT Failed Publishing photocell val [%d]", AIO_cntr );
        } 
     else {
       if(syslog_available)  syslog.logf("loop_mqtt: AIO_MQTT Success Publishing photocell val [%d]", AIO_cntr );    
       debugA("OK!\r\n");
       };
    AIO_last_time = millis();   
    };
  // ping the server to keep the mqtt connection alive
  if(! mqtt.ping()) { mqtt.disconnect(); } ;
#endif
}

// Function to connect and reconnect as necessary to the Adafruit IO MQTT server.
// Should be called in the loop function and it will take care if connecting.
void Adafruit_MQTT_connect() {
  int8_t ret;
#ifdef AIO_MQTT_ENABLE  
  // Stop if already connected.
  if (mqtt.connected()) {
    return;
  }

  debugA("Adafruit_MQTT_connect: Connecting to MQTT... ");
  int i=5 ;
  while ((ret = mqtt.connect()) != 0 && i-- > 0  && ! wifi_unavailable ) { // connect will return 0 for connected
       debugA("%s",mqtt.connectErrorString(ret).c_str());
       debugA("Adafruit_MQTT_connect: Retrying MQTT connection to Adafruit MQTT Broker...\r");
       mqtt.disconnect();
       vTaskDelay(pdMS_TO_TICKS(50));  // wait 50 msec seconds and retray max 4 times then abandon
  }
  debugA("Adafruit_MQTT_connect: MQTT Connected!\r");
#endif  
}


void HMQ_setupMQTT() {
#ifdef HMQ_MQTT_ENABLE    
  debugA("HMQ_setupMQTT: setup HMQ_setupMQTT ... \r");
  mqttClient.setServer(HMQ_mqttServer, HMQ_mqttPort);
  mqttClient.setCallback(HMQ_callback); // set the callback function
  debugA("HMQ_setupMQTT: setup HMQ_setupMQTT successfully completed \r");
  mqtt_unavailable = 1 ;
#endif  
}


bool HMQ_reconnect() {
#ifdef HMQ_MQTT_ENABLE    
  if ( millis() - HMQ_last_connect_trial < 32000 ) return (false);
  HMQ_last_connect_trial = millis();
  debugA("Connecting to HMQ MQTT Broker.....\r");
  if(!mqttClient.connected() ) {
      debugA("HMQ_reconnect: Retrying connection to HMQ MQTT Broker.. \r" );
      String clientId = "ESP32Client-";
      clientId += String(random(0xffff), HEX);
      if (mqttClient.connect(clientId.c_str())) {
        mqtt_unavailable = 0;
        debugA("HMQ_reconnect: Connected to HMQ MQTT Broker\r");
        String SubPath = "/micfuc2020/" + SyslogDeviceName + "/command" ;
//      mqttClient.subscribe("/micfuc2020/command"); // subscribe to topic   
        mqttClient.subscribe(SubPath.c_str()); // subscribe to topic  
        return(true);  
       };
  } ; // end of while
  mqtt_unavailable = 1 ;
  return(false);
#endif  
   return true ;

}


void HMQ_callback(char* topic, byte* payload, unsigned int length) {
#ifdef HMQ_MQTT_ENABLE 
  payload[length]='\0';
  syslog.logf("HMQ_callback: Got: [%s]", (char *)payload);
  debugAl("HMQ_callback: Rec_Message: ");
  for (int i = 0; i < length; i++) {
    debugAl("%c",(char)payload[i]);
    };
  debugAl("\r\n");
    
  // process incoming request
  String remote_command = String((char *)payload);
  HMQ_pong();                             // send back heartbeat replay     
  if(remote_command.indexOf("reboot") >= 0) {       // reboot command found
    debugA("===> Remote Reboot command found ...");
    RebootTag = 123 ;
    fram.write8(2015,RebootTag);
    vTaskDelay(pdMS_TO_TICKS(1000));
    RebootTag = 0;
    RebootTag = fram.read8(2015);
    if( RebootTag == 123) debugA("===> Reboot tag correctly written to FRAM ...");
    write_event_to_log("====== system reboot by MQTT Remote Master ========");
    syslog.logf(LOG_INFO, "system reboot by remote_command; LastFunction=%s", LastFunction.c_str());    
    debugA("===> Rebooting...");
    vTaskDelay(pdMS_TO_TICKS(2000));

    pinMode(14, OUTPUT);      // force device reset
    digitalWrite(14, LOW);
    }
  else if(remote_command.indexOf("dashboard") >= 0) {       // display dashboard command found
    HMQ_dashboard();       
    };
  
#endif  
  
}

// MQTT loop to publish periodically something.....

void HMQ_loop_MQTT() {
#ifdef HMQ_MQTT_ENABLE  
  if( ! wifi_unavailable &&  ! Admin_Mode  ) {  // upstream  internet conection is required for MQTT operation
    //    debugA("HMQ_loop_MQTT: try running HMQ_loop_MQTT mqttClient.loop() ... \r\n");
    if (!mqttClient.connected()){ 
      if( HMQ_reconnect() )  mqttClient.loop();
      }
    else{
      mqttClient.loop();      
      };  
    };
/*    
    if (millis() - HMQ_last_time > 60000) {  // publish every 60 secs
       String PubVal = "Heartbeat from device " + SyslogDeviceName + " [" + String(HMQ_cntr)+ "]" ;
       if ( mqttClient.publish("/micfuc2020/replay", PubVal.c_str())){
          debugAl("HMQ_loop_MQTT: HMQ_MQTT %s device heartbeat: >>[%d]\r\n", SyslogDeviceName.c_str(), HMQ_cntr++);
          if(syslog_available ){
             bool ret=syslog.logf("HMQ_loop_MQTT: HMQ_MQTT %s device heartbeat:>> [%d]", SyslogDeviceName.c_str(), HMQ_cntr);
             if (! ret) debugAl("HMQ_loop_MQTT: syslog.logf FAILED\r\n\r\n ");
             };
          }
       else{
          debugAl("HMQ_loop_MQTT: HMQ_MQTT FAILED  Publishing replay [%d]\r\n", HMQ_cntr++);
          if(syslog_available){
              bool ret=syslog.logf("HMQ_loop_MQTT: HMQ_MQTT Success Publishing replay  [%d]", HMQ_cntr);
             if (! ret) debugAl("HMQ_loop_MQTT: syslog.logf FAILED\r\n\r\n ");
             };
        }; 
       HMQ_last_time = millis();
       } ; // end of publish every 60 secs
*/
#endif
}


void HMQ_dashboard(void) {
#ifdef HMQ_MQTT_ENABLE
       String PubPath = "/micfuc2020/" + SyslogDeviceName + "/replay" ;
       String PubVal = (String)msg_dashboard() ;
       
//     if ( mqttClient.publish("/micfuc2020/replay", PubVal.c_str())){
       mqttClient.setBufferSize(4096);
       if ( mqttClient.publish(PubPath.c_str(), PubVal.c_str())){
        
          debugAl("HMQ_dashboard: HMQ_MQTT %s : >>[%d]\r\n", SyslogDeviceName.c_str(), HMQ_cntr++);
          if( syslog_available ){
             bool ret=syslog.logf("HMQ_dashboard: HMQ_MQTT %s:>> [%d]", SyslogDeviceName.c_str(), HMQ_cntr);
             if (! ret) debugAl("HMQ_dashboard: syslog.logf FAILED\r\n\r\n ");
             };
          }
       else{
          debugAl("HMQ_dashboard: HMQ_MQTT FAILED  Publishing replay [%d]\r\n", HMQ_cntr++);
          if(syslog_available){
             bool ret=syslog.logf("HMQ_dashboard: HMQ_MQTT Success Publishing replay  [%d]", HMQ_cntr);
             if (! ret) debugAl("HMQ_dashboard: syslog.logf FAILED\r\n\r\n ");
             };
        };
#endif       
}       




String  msg_dashboard(void){              // create a json formatted stats dump for transfer via mqtt
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

  String op_status_clear="Not_synchronized";
  if(op_status == 1 ) {op_status_clear = "RTC_disciplined";};
  if(op_status == 2 ) {op_status_clear = "NTP_disciplined";};
  if(op_status == 3 ) {op_status_clear = "GPS_disciplined";};
  String wifi_upstream = "UP" ; if(wifi_unavailable   > 0 ) wifi_upstream = "DOWN";
  String mqtt_upstream = "UP" ; if(mqtt_unavailable   > 0 ) mqtt_upstream = "DOWN";


  
  // use https://jsonformatter.org/json-pretty-print   to display results
  // https://jsonformatter.curiousconcept.com/#https://jsonformatter.curiousconcept.com/# alternativa
  // Allocate a temporary JsonDocument
  // Don't forget to change the capacity to match your requirements.
  // Use arduinojson.org/assistant to compute the capacity.
  StaticJsonDocument<1024> doc;

  // Set the values in the documens
  doc["Name of Device"] = ESP_Config.DeviceName;
  doc["Device Id"] = get_device_id();
  doc["CPU_Type"] = String(ESP_Config.cpu_type);
  doc["SW_version"] = SW_Revision.c_str();
  
  doc["Device Date/Time"] = device_date.c_str();
  doc["Operation Status"] = op_status_clear.c_str();
  doc["Clock Jitter(msec)"] = String(time_jitter);
  doc["WiFi Upstream"] = wifi_upstream.c_str();
  doc["MQTT Upstream"] = mqtt_upstream.c_str();
  doc["GPS Fix"] = gps_pos.c_str();
  doc["GPS Sats"] = gps_sats.c_str();
  doc["Last_RX_Pkt_Report"] = RX_Last_Spot_level.c_str();
  doc["Last_RX_Pkt_payload"] = RX_Last_Spot_payload.c_str();
  doc["Last_RX_Path"] = RX_Last_Path_Report.c_str();
  doc["LoRa_rx_packets"] = LoRa_rx_packets;
  doc["LoRa_tx_packets"] = LoRa_tx_packets;
  doc["LoRa_rx_AX25_packets"] = LoRa_rx_AX25_packets;
  doc["LoRa_tx_AX25_packets"] = LoRa_tx_AX25_packets;

  doc["LoRa_rx_OEStyle_packets"] = LoRa_rx_OEStyle_packets;
  doc["LoRa_tx_OEStyle_packets"] = LoRa_tx_OEStyle_packets;
  doc["LoRa_rx_native_packets"] = LoRa_rx_native_packets;
  doc["LoRa_tx_native_packets"] = LoRa_tx_native_packets;
  doc["LoRa_lost_packets"] = LoRa_lost_packets;
  doc["LoRa_CRC_errored_packets"] = LoRa_CRC_errored_packets;
  doc["LoRa_CRC_errored_packets_BCN"] = LoRa_CRC_errored_packets_BCN;
  doc["LoRa_UMN_errored_packets"] = LoRa_UMN_errored_packets;
  doc["LoRa_CAD_errors"] = LoRa_CAD_errors;
  doc["LoRa_ReSched_packets"] = LoRa_ReSched_packets;
  doc["AprsIS_rx_packets"] = AprsIS_rx_packets;
  doc["AprsIS_tx_packets"] = AprsIS_tx_packets;
  doc["AprsIS_dropped_packets"] = AprsIS_dropped_packets;
  doc["AprsIS_relayed_packets"] = AprsIS_relayed_packets;
  doc["IPC_lost_msgs"] = IPC_lost_msgs;
  doc["LoRa_OnAirTime(msec)"] = LoRa_OnAirTime;
  doc["LoRa_DutyCycle(percentage)"] = DutyCycle;
  doc["LoRa_ChanCong(percentage)"] = ChanCongStr;
  doc["LoRa_ENL(dbm)"] = LoRa_ENLStr;
  doc["LoRa_FreqJitter(ppm)"] = LoRa_FreqJitter;
  doc["CPU_Temperature C"] = cpu_temp;
  uint32_t proc_uptime= esp_timer_get_time()/1000000;
  doc["Processor_UpTime(sec)"] = proc_uptime ;

  // Serialize JSON to buffer
  byte msg_buf[1024];
  serializeJsonPretty(doc, msg_buf, 1024);  
  String msg_mqtt= (char*)msg_buf;
  return msg_mqtt ;
}



void HMQ_pong(void) {
#ifdef HMQ_MQTT_ENABLE
       String PubPath = "/micfuc2020/" + SyslogDeviceName + "/replay" ;
       String PubVal = "Heartbeat from device " + SyslogDeviceName + " [" + String(HMQ_cntr)+ "]" ;
       
//     if ( mqttClient.publish("/micfuc2020/replay", PubVal.c_str())){
       if ( mqttClient.publish(PubPath.c_str(), PubVal.c_str())){
          debugAl("HMQ_pong: HMQ_MQTT %s device heartbeat: >>[%d]\r\n", SyslogDeviceName.c_str(), HMQ_cntr++);
          if( syslog_available ){
             bool ret=syslog.logf("HMQ_pong: HMQ_MQTT %s device heartbeat:>> [%d] - LastFunction=%s", SyslogDeviceName.c_str(), HMQ_cntr, LastFunction.c_str());
             if (! ret) debugAl("HMQ_pong: syslog.logf FAILED\r\n\r\n ");
             };
          }
       else{
          debugAl("HMQ_pong: HMQ_MQTT FAILED  Publishing replay [%d]\r\n", HMQ_cntr++);
          if(syslog_available){
              bool ret=syslog.logf("HMQ_pong: HMQ_MQTT Success Publishing replay  [%d]", HMQ_cntr);
             if (! ret) debugAl("HMQ_pong: syslog.logf FAILED\r\n\r\n ");
             };
        };
#endif       
}        
