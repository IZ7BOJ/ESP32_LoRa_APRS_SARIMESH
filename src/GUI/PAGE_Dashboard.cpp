// PAGE_Dashboard.cpp
#include "PAGE_Dashboard.h"

//
// FILL WITH INFOMATION
// 

void send_dashboard_values_html () {
  char TEMP[32],TEMP1[32],TEMP2[32],TEMP3[32], TEMP4[32], TEMP5[32], TEMP6[32];
  
  sprintf(TEMP,"%d",LoRa_OnAirTime);
  String OnAirTime=TEMP;
  sprintf(TEMP1,"%5.2f",cpu_temp) ;
  String CpuTemp = TEMP1;  
  uint32_t uptime = esp_timer_get_time()/1000000 ;
  sprintf(TEMP2,"%d",uptime) ;
  String Up_Time = TEMP2;  
  sprintf(TEMP3,"%5.2f",ppm_runtime) ;
  String LoRa_FreqJitter = TEMP3; 
  sprintf(TEMP4,"%5.2f",LoRa_DutyCycle) ;
  String DutyCycle=TEMP4;
  sprintf(TEMP5,"%5.2f",ChanCong) ;
  String ChanCongStr=TEMP5;
  sprintf(TEMP6,"%5.2f",LoRa_ENL) ;
  String LoRa_ENLStr=TEMP6;
  // String device_date=UTC.dateTime(now(), F("l, d/n/Y H:i:s.v T")) ;
  String device_date=UTC.dateTime(ezt::now(), F("d/n/Y H:i:s T")) ;
  if(GPS_TimeDate != "" ) device_date = GPS_TimeDate ;  // if we have a GPS originated timedate just display it
  if(CPU_TYPE == "TTGO"){                               // TTGO must use gps timedate in any case
    if(GPS_TimeDate == "" ) {
      device_date = String("Not Jet Available");
      }
    else{  
      device_date = GPS_TimeDate ;
    };
  }; 
  

  if(device_date.indexOf("1970") != -1) { device_date="Not_Jet_Available";  };
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
  if(ESP_Config.BT_KISS_Mode) {device_type = "BT_KISS Mode";};
  
  String op_status_clear="Not_synchronized";
  if(op_status == 1 ) {op_status_clear = "RTC_disciplined";};
  if(op_status == 2 ) {op_status_clear = "NTP_disciplined";};
  if(op_status == 3 ) {op_status_clear = "GPS_disciplined";};
  String wifi_upstream = "UP" ; if(wifi_unavailable   > 0 ) wifi_upstream = "DOWN";
  String mqtt_upstream = "UP" ; if(mqtt_unavailable   > 0 ) mqtt_upstream = "DOWN";
  String aprs_upstream = "DOWN" ; if(aprsis_available  ) aprs_upstream = "UP";
  String syslog_upstream = "DOWN" ; if(syslog_available  ) syslog_upstream = "UP";

  String dashboardvalues ="";
  dashboardvalues += "x_LoRa_rx_packets|" + String(LoRa_rx_packets) +  "|div\n";
  dashboardvalues += "x_LoRa_tx_packets|" + String(LoRa_tx_packets) +  "|div\n";
  dashboardvalues += "x_LoRa_rx_AX25_packets|" + String(LoRa_rx_AX25_packets) +  "|div\n";
  dashboardvalues += "x_LoRa_tx_AX25_packets|" + String(LoRa_tx_AX25_packets) +  "|div\n";
  dashboardvalues += "x_LoRa_rx_OEStyle_packets|" + String(LoRa_rx_OEStyle_packets) +  "|div\n";
  dashboardvalues += "x_LoRa_tx_OEStyle_packets|" + String(LoRa_tx_OEStyle_packets) +  "|div\n";
  dashboardvalues += "x_LoRa_rx_native_packets|" + String(LoRa_rx_native_packets) +  "|div\n";
  dashboardvalues += "x_LoRa_tx_native_packets|" + String(LoRa_tx_native_packets) +  "|div\n";
  dashboardvalues += "x_LoRa_lost_packets|" + String(LoRa_lost_packets) +  "|div\n";

  if(!BT_KISS_Mode && !Serial_KISS_Mode && !TCP_KISS_Mode){   // this is for APRS
    dashboardvalues += "x_AprsIS_rx_packets|" + String(AprsIS_rx_packets) +  "|div\n";
    dashboardvalues += "x_AprsIS_tx_packets|" + String(AprsIS_tx_packets) +  "|div\n";
    dashboardvalues += "x_AprsIS_dropped_packets|" + String(AprsIS_dropped_packets) +  "|div\n";
    dashboardvalues += "x_AprsIS_relayed_packets|" + String(AprsIS_relayed_packets) +  "|div\n";
    }
  else {  // this is for KISS
     dashboardvalues += "x_Kiss_rx_packets|" + String(Kiss_rx_packets) +  "|div\n";
     dashboardvalues += "x_Kiss_tx_packets|" + String(Kiss_tx_packets) +  "|div\n";    
    };  

  dashboardvalues += "x_IPC_lost_msgs|" + String(IPC_lost_msgs) +  "|div\n";
  dashboardvalues += "x_LoRa_OnAirTime|" + OnAirTime +  "|div\n";
  dashboardvalues += "x_LoRa_DutyCycle|" + DutyCycle +  "|div\n";
  dashboardvalues += "x_ChanCong|" + ChanCongStr +  "|div\n";
  dashboardvalues += "x_LoRa_ENL|" + LoRa_ENLStr +  "|div\n";
  dashboardvalues += "x_LoRa_FreqJitter|" + LoRa_FreqJitter +  "|div\n";
  dashboardvalues += "x_cpu_temp|" + CpuTemp +  "|div\n";
  dashboardvalues += "x_Processor_UpTime|" + Up_Time +  "|div\n";
  dashboardvalues += "x_device_date|" + device_date +  "|div\n";  
  dashboardvalues += "x_device_type|" + device_type +  "|div\n";  
  dashboardvalues += "x_op_status|" + op_status_clear +  "|div\n";  
  dashboardvalues += "x_time_jitter|" + String(time_jitter) +  "|div\n";  
  dashboardvalues += "x_wifi_upstream|" + String(wifi_upstream) +  "|div\n";  
  dashboardvalues += "x_aprs_upstream|" + String(aprs_upstream) +  "|div\n";  
  dashboardvalues += "x_mqtt_upstream|" + String(mqtt_upstream) +  "|div\n";  
  dashboardvalues += "x_syslog_upstream|" + String(syslog_upstream) +  "|div\n";  
  dashboardvalues += "x_gps_pos|" + gps_pos +  "|div\n";  
  dashboardvalues += "x_gps_sats|" + gps_sats +  "|div\n";  
  //  String my_new_spot = (String *)RX_Last_Spot_level.replace('|',' ');
  dashboardvalues += "x_RX_Last_Spot_level|" + RX_Last_Spot_level +  "|div\n";  
  dashboardvalues += "x_RX_Last_Spot_payload|" + RX_Last_Spot_payload +  "|div\n";  
  dashboardvalues += "x_RX_Last_Path_Report|" + RX_Last_Path_Report +  "|div\n";  
  dashboardvalues += "x_LoRa_CRC_errored_packets|" + String(LoRa_CRC_errored_packets) + String(" / ") + String(LoRa_CRC_errored_packets_BCN)+  "|div\n";
  dashboardvalues += "x_LoRa_UMN_errored_packets|" + String(LoRa_UMN_errored_packets) +  "|div\n";
  dashboardvalues += "x_LoRa_ReSched_packets|" + String(LoRa_ReSched_packets) +  "|div\n";
  dashboardvalues += "x_LoRa_CAD_errors|" + String(LoRa_CAD_errors) +  "|div\n";

  web_server.send ( 200, "text/plain", dashboardvalues);
  if(WebConfig_debug)  debugA("%s",__FUNCTION__); 
}

