// PAGE_Statistics.cpp
#include "PAGE_Statistics.h"


void send_stats_values_html () {
  char TEMP[32],TEMP1[32],TEMP2[32];
  sprintf(TEMP,"%d",LoRa_OnAirTime);
  String OnAirTime=TEMP;
  sprintf(TEMP1,"%5.2f",cpu_temp) ;
  String CpuTemp = TEMP1;  
  uint32_t uptime = esp_timer_get_time()/1000000 ;
  sprintf(TEMP2,"%d",uptime) ;
  String Up_Time = TEMP2;  
  //  String device_date=UTC.dateTime(now(), F("l, d/n/Y H:i:s.v T")) ;
  String device_date=UTC.dateTime(ezt::now(), F("d/n/Y H:i:s T")) ;
  if(device_date.indexOf("1970") != -1) { device_date="Not_Jet_Available";  };
	String statsvalues ="";
	statsvalues += "x_LoRa_rx_packets|" + String(LoRa_rx_packets) +  "|div\n";
  statsvalues += "x_LoRa_tx_packets|" + String(LoRa_tx_packets) +  "|div\n";
  statsvalues += "x_LoRa_rx_AX25_packets|" + String(LoRa_rx_AX25_packets) +  "|div\n";
  statsvalues += "x_LoRa_tx_AX25_packets|" + String(LoRa_tx_AX25_packets) +  "|div\n";
  statsvalues += "x_LoRa_lost_packets|" + String(LoRa_lost_packets) +  "|div\n";
  statsvalues += "x_AprsIS_rx_packets|" + String(AprsIS_rx_packets) +  "|div\n";
  statsvalues += "x_AprsIS_tx_packets|" + String(AprsIS_tx_packets) +  "|div\n";
  statsvalues += "x_AprsIS_dropped_packets|" + String(AprsIS_dropped_packets) +  "|div\n";
  statsvalues += "x_LoRa_OnAirTime|" + OnAirTime +  "|div\n";
  statsvalues += "x_cpu_temp|" + CpuTemp +  "|div\n";
  statsvalues += "x_Processor_UpTime|" + Up_Time +  "|div\n";
  statsvalues += "x_device_date|" + device_date +  "|div\n";  
  statsvalues += "x_LoRa_CRC_errored_packets|" + String(LoRa_CRC_errored_packets) +  "|div\n";

	web_server.send ( 200, "text/plain", statsvalues);
  if(WebConfig_debug)  debugA("%s",__FUNCTION__); 
}



