// ArduinoJson - arduinojson.org 
// https://github.com/bblanchon/ArduinoJson/blob/6.x/examples/JsonConfigFile/JsonConfigFile.ino
// Copyright Benoit Blanchon 2014-2021
// MIT License
// https://arduinojson.org/v6/example/config/


#include "JsonConfigFile.h"

//fs::FS localFs = LittleFS;

bool DisplayConfig_serial(void );

  
const char *filename = "/WebConf.conf";  // <- SD library uses 8.3 filenames

bool loadConfigurationFromFlash(const char *filename) {  // Loads the configuration from a file
  
  Serial.printf("loadConfigurationFromFlash: using %s config file\r\n",filename);
  File file = LittleFS.open(filename);    // Open file for reading
  if(!file){
     Serial.printf("Failed to open %s file for reading\r\n",filename);
     return false;
     }; 
  // Allocate a temporary JsonDocument
  // Don't forget to change the capacity to match your requirements.
  // Use arduinojson.org/v6/assistant to compute the capacity.
  StaticJsonDocument<4096> doc;

  // Deserialize the JSON document
  DeserializationError error = deserializeJson(doc, file);
  if (error) {
    Serial.print("Failed to Dserialize Json configuration file, using default configuration\r\n");
    return false;
    };

  // Copy values from the JsonDocument to the Config
 // config.AprsPort = doc["AprsPort"] | 2731;
 // strlcpy(config.hostname,                  // <- destination
 //         doc["hostname"] | "example.com",  // <- source
 //         sizeof(config.hostname));         // <- destination's capacity
 // config.AprsHost = doc["AprsHost"] | "example.com" ;
 
  ESP_Config.IP[0] = doc["IP[0]"] ;
  ESP_Config.IP[1] = doc["IP[1]"] ;
  ESP_Config.IP[2] = doc["IP[2]"] ;
  ESP_Config.IP[3] = doc["IP[3]"] ;
  ESP_Config.Netmask[0] = doc["Netmask[0]"] ;
  ESP_Config.Netmask[1] = doc["Netmask[1]"] ;
  ESP_Config.Netmask[2] = doc["Netmask[2]"] ;
  ESP_Config.Netmask[3] = doc["Netmask[3]"] ;
  ESP_Config.Gateway[0] = doc["Gateway[0]"] ;
  ESP_Config.Gateway[1] = doc["Gateway[1]"] ;
  ESP_Config.Gateway[2] = doc["Gateway[2]"] ;
  ESP_Config.Gateway[3] = doc["Gateway[3]"] ;
  ESP_Config.Dns[0] = doc["Dns[0]"] ;
  ESP_Config.Dns[1] = doc["Dns[1]"] ;
  ESP_Config.Dns[2] = doc["Dns[2]"] ;
  ESP_Config.Dns[3] = doc["Dns[3]"] ;

  ESP_Config.dhcp = doc["dhcp"] | 1;
  ESP_Config.ntpServerName = doc["ntpServerName"] | "";
  ESP_Config.Update_Time_Via_NTP_Every = doc["Update_Time_Via_NTP_Every"] ;
  ESP_Config.timezone = doc["timezone"] ;
  ESP_Config.daylight = doc["daylight"] ;
  ESP_Config.DeviceName  = doc["DeviceName"]| ""  ;
  ESP_Config.DeviceId = doc["DeviceId"]| ""  ;
  ESP_Config.cpu_type = doc["cpu_type"]| ""  ;
  ESP_Config.gps_debug = doc["gps_debug"] ;
  ESP_Config.LoRa_debug = doc["LoRa_debug"] ;
  ESP_Config.APRS_debug = doc["APRS_debug"] ;
  
  ESP_Config.RTC_debug = doc["RTC_debug"] ;
  ESP_Config.ezTime_debug = doc["ezTime_debug"] ; 
  ESP_Config.pps_debug = doc["pps_debug"] ;
  ESP_Config.PE_debug = doc["PE_debug"] ;
  ESP_Config.BT_KISS_Mode = doc["BT_KISS_Mode"] ; 
  ESP_Config.Serial_KISS_Mode =  doc["Serial_KISS_Mode"] ;
  ESP_Config.TCP_KISS_Mode =  doc["TCP_KISS_Mode"] ;
  
  ESP_Config.Tracker_Mode =  doc["Tracker_Mode"] ;
  ESP_Config.IoT_enable =  doc["IoT_enable"] ;
  ESP_Config.ps_debug =  doc["ps_debug"] ;
  
  ESP_Config.V5_ena = doc["V5_ena"] ;
  ESP_Config.V24_1_ena = doc["V24_1_ena"] ;
  ESP_Config.V24_2_ena = doc["V24_2_ena"] ;
  ESP_Config.V24_3_ena = doc["V24_3_ena"] ;
 
  ESP_Config.mqtt_enable = doc["mqtt_enable"] ;
  ESP_Config.WebConfig_debug = doc["WebConfig_debug"] ;
  ESP_Config.standalone = doc["standalone"] ;
  ESP_Config.syslog_enable = doc["syslog_enable"] ;
  ESP_Config.RebootTag = doc["RebootTag"] ;

  ESP_Config.no_gps =  doc["no_gps"] ;
  ESP_Config.iGate_Mode = doc["iGate_Mode"] ;
  ESP_Config.Beacon_Mode = doc["Beacon_Mode"] ;
  
  ESP_Config.Admin_Mode = doc["Admin_Mode"] ;
  ESP_Config.Reboot_Now = doc["Reboot_Now"] ;
  ESP_Config.EnableRfToIs = doc["EnableRfToIs"] ;
  ESP_Config.EnableIsToRf = doc["EnableIsToRf"] ;
  ESP_Config.EnableRepeater = doc["EnableRepeater"] ; 

  ESP_Config.ssid = doc["ssid"]| ""  ;
  ESP_Config.password = doc["password"]| ""  ;

  ESP_Config.AprsHost = doc["AprsHost"]| "rotate.aprs2.net"  ;
  ESP_Config.AprsPort = doc["AprsPort"] | 14580 ;
  ESP_Config.AprsLogin = doc["AprsLogin"] | "" ;
  ESP_Config.AprsPass = doc["AprsPass"] | "" ;
  ESP_Config.AprsFilter = doc["AprsFilter"]| ""  ;
  ESP_Config.AprsRawBeaconIGATE = doc["AprsRawBeaconIGATE"] | "" ;
  ESP_Config.AprsRawBeaconTRACK = doc["AprsRawBeaconTRACK"] | "" ;
  ESP_Config.AprsRawBeaconPeriodSecondsIGATE = doc["AprsRawBeaconPeriodSecondsIGATE"] ;
  ESP_Config.AprsRawBeaconPeriodSecondsTRACK = doc["AprsRawBeaconPeriodSecondsTRACK"] ; 
  ESP_Config.LoraFreq = doc["LoraFreq"] | "" ;
  ESP_Config.LoraBw = doc["LoraBw"] | "";
  ESP_Config.LoraSf = doc["LoraSf"] ; 
  ESP_Config.LoraCodingRate = doc["LoraCodingRate"] ;
  ESP_Config.LoraSync = doc["LoraSync"] ;
  ESP_Config.LoraPower = doc["LoraPower"] ;
  ESP_Config.LoraFreqCorr = doc["LoraFreqCorr"] ;
  ESP_Config.LoraPreambleLen = doc["LoraPreambleLen"] ; 
  ESP_Config.EncapType = doc["EncapType"] ; 
  ESP_Config.wifi_enable = doc["wifi_enable"] ; 
  
  ESP_Config.Loc_Lat = doc["Loc_Lat"] | "" ;
  ESP_Config.Loc_Lon = doc["Loc_Lon"] | ""  ;
  ESP_Config.AprsLoggerAddr = doc["AprsLoggerAddr"] | "" ; 
  ESP_Config.AprsLoggerPort = doc["AprsLoggerPort"] ;
  ESP_Config.IoT_Host = doc["IoT_Host"] | "" ;
  ESP_Config.IoT_Port = doc["IoT_Port"] ; 
  ESP_Config.IoT_Login = doc["IoT_Login"] | "" ; 
  ESP_Config.IoT_Key = doc["IoT_Key"] | "" ;
  ESP_Config.feed_1 = doc["feed_1"] | "" ;
  ESP_Config.feed_2 = doc["feed_2"] | "" ; 
  ESP_Config.feed_3 = doc["feed_3"] | "" ;
  ESP_Config.feed_4 = doc["feed_4"] | "" ; 
  ESP_Config.feed_5 = doc["feed_5"] | "" ;
  ESP_Config.feed_6 = doc["feed_6"] | "" ; 
//  ESP_Config.SW_Revision  = doc["SW_Revision"] | "" ;

    ESP_Config.i2c_sda = doc["i2c_sda"] ;
    ESP_Config.i2c_scl = doc["i2c_scl"] ;
    ESP_Config.spi_sck = doc["spi_sck"] ;
    ESP_Config.spi_miso = doc["spi_miso"] ;
    ESP_Config.spi_mosi = doc["spi_mosi"] ;
    ESP_Config.oled_addr = doc["oled_addr"] ;
    ESP_Config.oled_rst = doc["oled_rst"] ;
    ESP_Config.oled_orient = doc["oled_orient"] ;
    ESP_Config.lora_cs = doc["lora_cs"] ;
    ESP_Config.lora_rst = doc["lora_rst"] ;
    ESP_Config.lora_dio = doc["lora_dio"] ;
    ESP_Config.gps_rx = doc["gps_rx"] ;
    ESP_Config.gps_tx = doc["gps_tx"] ;
    ESP_Config.PayloadStyle = doc["PayloadStyle"] ;
    ESP_Config.RepeaterOperation = doc["RepeaterOperation"] ;
    ESP_Config.LocationCompression = doc["LocationCompression"] ;
    ESP_Config.BlackList = doc["BlackList"] ;

    ESP_Config.AgileBeaconing = doc["AgileBeaconing"] ;

  

// Beacon app parameters
  ESP_Config.BeaconId  = doc["BeaconId"]  ;
  ESP_Config.BeaconSeqNbr  = doc["BeaconSeqNbr"]  ;
  ESP_Config.BeaconLocation  = doc["BeaconLocation"]  ;
  ESP_Config.BeaconEngineType  = doc["BeaconEngineType"]  ;
  ESP_Config.BeaconRun  = doc["BeaconRun"]  ;
  ESP_Config.BCN_TimeSlotOffset  = doc["BCN_TimeSlotOffset"]  ;
  ESP_Config.BCN_TimeSlotSync  = doc["BCN_TimeSlotSync"]  ;
  ESP_Config.BeaconStatsCollector_IP[0] = doc["BeaconStatsCollector_IP[0]"] ;
  ESP_Config.BeaconStatsCollector_IP[1] = doc["BeaconStatsCollector_IP[1]"] ;
  ESP_Config.BeaconStatsCollector_IP[2] = doc["BeaconStatsCollector_IP[2]"] ;
  ESP_Config.BeaconStatsCollector_IP[3] = doc["BeaconStatsCollector_IP[3]"] ;

  ESP_Config.BeaconStatsCollector_Port  = doc["BeaconStatsCollector_Port"] ;
  ESP_Config.BeaconQRALocator  = doc["BeaconQRALocator"] | "" ;
  ESP_Config.BCN_LoRa_Vector  = doc["BCN_LoRa_Vector"] | "" ;

  ESP_Config.BeaconUnixTime  = doc["BeaconUnixTime"] | "" ;
  ESP_Config.BeaconFreq  = doc["BeaconFreq"] | "" ;
  ESP_Config.BeaconPower  = doc["BeaconPower"] | "" ;
  ESP_Config.BeaconWorkConditions  = doc["BeaconWorkConditions"] | "" ;

  // Close the file (Curiously, File's destructor doesn't close the file)
  file.close();
  return true ;
}


void saveConfigurationToFlash(const char *filename ) {  // Saves the configuration to a file
  LittleFS.remove(filename);   // Delete existing file, otherwise the configuration is appended to the file
  File file = LittleFS.open(filename, FILE_WRITE);  // Open file for writing
  if (!file) {
    Serial.print("Failed to create file\r\n");
    return;
  }

  // Allocate a temporary JsonDocument
  // Don't forget to change the capacity to match your requirements.
  // Use arduinojson.org/assistant to compute the capacity.
  StaticJsonDocument<4096> doc;

  // Set the values in the documens
  doc["IP[0]"] = ESP_Config.IP[0];
  doc["IP[1]"] = ESP_Config.IP[1];
  doc["IP[2]"] = ESP_Config.IP[2];
  doc["IP[3]"] = ESP_Config.IP[3];
  doc["Netmask[0]"] = ESP_Config.Netmask[0];
  doc["Netmask[1]"] = ESP_Config.Netmask[1];
  doc["Netmask[2]"] = ESP_Config.Netmask[2];
  doc["Netmask[3]"] = ESP_Config.Netmask[3];
  doc["Gateway[0]"] = ESP_Config.Gateway[0];
  doc["Gateway[1]"] = ESP_Config.Gateway[1];
  doc["Gateway[2]"] = ESP_Config.Gateway[2];
  doc["Gateway[3]"] = ESP_Config.Gateway[3];
  doc["Dns[0]"] = ESP_Config.Dns[0]; 
  doc["Dns[1]"] = ESP_Config.Dns[1];
  doc["Dns[2]"] = ESP_Config.Dns[2];
  doc["Dns[3]"] = ESP_Config.Dns[3];
 
  doc["ssid"] = ESP_Config.ssid  ;
  doc["password"] = ESP_Config.password ;
  doc["dhcp"] = ESP_Config.dhcp;
  doc["ntpServerName"] = ESP_Config.ntpServerName.c_str();
  doc["Update_Time_Via_NTP_Every"] = ESP_Config.Update_Time_Via_NTP_Every;
  doc["timezone"] = ESP_Config.timezone;
  doc["daylight"] = ESP_Config.daylight;
  doc["DeviceName"] = ESP_Config.DeviceName.c_str();
  doc["DeviceId"] = ESP_Config.DeviceId.c_str();
  doc["cpu_type"] = ESP_Config.cpu_type.c_str();
  doc["gps_debug"] = ESP_Config.gps_debug;
  doc["LoRa_debug"] = ESP_Config.LoRa_debug;
  doc["APRS_debug"] = ESP_Config.APRS_debug;
  
  doc["RTC_debug"] = ESP_Config.RTC_debug;
  doc["ezTime_debug"] = ESP_Config.ezTime_debug;
  doc["pps_debug"] = ESP_Config.pps_debug;
  doc["PE_debug"] = ESP_Config.PE_debug;
  doc["BT_KISS_Mode"] = ESP_Config.BT_KISS_Mode;
  doc["Serial_KISS_Mode"] = ESP_Config.Serial_KISS_Mode; 
  doc["TCP_KISS_Mode"] = ESP_Config.TCP_KISS_Mode; 
  
  doc["Tracker_Mode"] = ESP_Config.Tracker_Mode; 
  doc["IoT_enable"] = ESP_Config.IoT_enable; 
  doc["mqtt_enable"] = ESP_Config.mqtt_enable;
  doc["WebConfig_debug"] = ESP_Config.WebConfig_debug;
  doc["standalone"] = ESP_Config.standalone;
  doc["syslog_enable"] = ESP_Config.syslog_enable;
  doc["RebootTag"] = ESP_Config.RebootTag;
  doc["ps_debug"] = ESP_Config.ps_debug;


  doc["V5_ena"] = ESP_Config.V5_ena;
  doc["V24_1_ena"] = ESP_Config.V24_1_ena;
  doc["V24_2_ena"] = ESP_Config.V24_2_ena;
  doc["V24_3_ena"] = ESP_Config.V24_3_ena;

  doc["no_gps"] = ESP_Config.no_gps; 
  doc["iGate_Mode"] = ESP_Config.iGate_Mode;
  doc["Beacon_Mode"] = ESP_Config.Beacon_Mode;
  
  doc["Admin_Mode"] = ESP_Config.Admin_Mode;
  doc["Reboot_Now"] = ESP_Config.Reboot_Now;
  doc["EnableRfToIs"] = ESP_Config.EnableRfToIs;
  doc["EnableIsToRf"] = ESP_Config.EnableIsToRf;
  doc["EnableRepeater"] = ESP_Config.EnableRepeater;
  doc["AprsHost"] = ESP_Config.AprsHost.c_str();
  doc["AprsPort"] = ESP_Config.AprsPort;
  doc["AprsLogin"] = ESP_Config.AprsLogin.c_str() ;
  doc["AprsPass"] = ESP_Config.AprsPass.c_str() ;
  doc["AprsFilter"] = ESP_Config.AprsFilter.c_str() ;
  doc["AprsRawBeaconIGATE"] = ESP_Config.AprsRawBeaconIGATE.c_str();
  doc["AprsRawBeaconTRACK"] = ESP_Config.AprsRawBeaconTRACK.c_str() ;
  doc["AprsRawBeaconPeriodSecondsIGATE"] = ESP_Config.AprsRawBeaconPeriodSecondsIGATE ;
  doc["AprsRawBeaconPeriodSecondsTRACK"] = ESP_Config.AprsRawBeaconPeriodSecondsTRACK ;
  doc["LoraFreq"] = ESP_Config.LoraFreq.c_str() ;
  doc["LoraBw"] = ESP_Config.LoraBw.c_str() ;
  doc["LoraSf"] = ESP_Config.LoraSf ;
  doc["LoraCodingRate"] = ESP_Config.LoraCodingRate ;
  doc["LoraSync"] = ESP_Config.LoraSync ;
  doc["LoraPower"] = ESP_Config.LoraPower ;
  doc["LoraFreqCorr"] = ESP_Config.LoraFreqCorr ;
  doc["LoraPreambleLen"] = ESP_Config.LoraPreambleLen ;
  doc["EncapType"] = ESP_Config.EncapType ;
  doc["wifi_enable"] = ESP_Config.wifi_enable ;
  
  doc["Loc_Lat"] = ESP_Config.Loc_Lat.c_str() ;
  doc["Loc_Lon"] = ESP_Config.Loc_Lon.c_str() ;
  doc["AprsLoggerAddr"] = ESP_Config.AprsLoggerAddr.c_str()  ;
  doc["AprsLoggerPort"] = ESP_Config.AprsLoggerPort ; 
  doc["IoT_Host"] = ESP_Config.IoT_Host.c_str()  ;
  doc["IoT_Port"] = ESP_Config.IoT_Port ;
  doc["IoT_Login"] = ESP_Config.IoT_Login.c_str()  ;
  doc["IoT_Key"] = ESP_Config.IoT_Key.c_str()  ;
  doc["feed_1"] = ESP_Config.feed_1.c_str()  ;
  doc["feed_2"] = ESP_Config.feed_2.c_str()  ;
  doc["feed_3"] = ESP_Config.feed_3.c_str()  ;
  doc["feed_4"] = ESP_Config.feed_4.c_str()  ;
  doc["feed_5"] = ESP_Config.feed_5.c_str()  ;
  doc["feed_6"] = ESP_Config.feed_6.c_str()  ;
//  doc["SW_Revision"] = ESP_Config.SW_Revision.c_str() ;

   doc["i2c_sda"] = ESP_Config.i2c_sda ; 
   doc["i2c_scl"] = ESP_Config.i2c_scl ;
   doc["spi_sck"] = ESP_Config.spi_sck ;
   doc["spi_miso"] = ESP_Config.spi_miso ;
   doc["spi_mosi"] = ESP_Config.spi_mosi ;
   doc["oled_addr"] = ESP_Config.oled_addr ; 
   doc["oled_rst"] = ESP_Config.oled_rst ; 
   doc["oled_orient"] = ESP_Config.oled_orient ; 
   doc["lora_cs"] = ESP_Config.lora_cs ; 
   doc["lora_rst"] = ESP_Config.lora_rst ;
   doc["lora_dio"] = ESP_Config.lora_dio ; 
   doc["gps_rx"] = ESP_Config.gps_rx ;
   doc["gps_tx"] = ESP_Config.gps_tx ;
   doc["PayloadStyle"] = ESP_Config.PayloadStyle ;
   doc["RepeaterOperation"] = ESP_Config.RepeaterOperation ;
   doc["LocationCompression"] = ESP_Config.LocationCompression ;
   doc["BlackList"] = ESP_Config.BlackList ;

   doc["AgileBeaconing"] = ESP_Config.AgileBeaconing ;

   
    ESP_Config.PayloadStyle = doc["PayloadStyle"] ;
    ESP_Config.RepeaterOperation = doc["RepeaterOperation"] ;
    
// Beacon app parameters
  doc["BeaconId"] = ESP_Config.BeaconId ;
  doc["BeaconSeqNbr"] = ESP_Config.BeaconSeqNbr ;
  doc["BeaconLocation"] = ESP_Config.BeaconLocation ;
  doc["BeaconEngineType"] = ESP_Config.BeaconEngineType ;
  doc["BeaconRun"] = ESP_Config.BeaconRun ;
  doc["BCN_TimeSlotOffset"] = ESP_Config.BCN_TimeSlotOffset ;
  doc["BCN_TimeSlotSync"] = ESP_Config.BCN_TimeSlotSync ;
  doc["BeaconStatsCollector_Port"] = ESP_Config.BeaconStatsCollector_Port ;
  doc["BeaconQRALocator"] = ESP_Config.BeaconQRALocator.c_str() ;
  doc["BCN_LoRa_Vector"] = ESP_Config.BCN_LoRa_Vector.c_str() ;
  doc["BeaconStatsCollector_IP[0]"] = ESP_Config.BeaconStatsCollector_IP[0]; 
  doc["BeaconStatsCollector_IP[1]"] = ESP_Config.BeaconStatsCollector_IP[1]; 
  doc["BeaconStatsCollector_IP[2]"] = ESP_Config.BeaconStatsCollector_IP[2]; 
  doc["BeaconStatsCollector_IP[3]"] = ESP_Config.BeaconStatsCollector_IP[3]; 

  doc["BeaconUnixTime"] = ESP_Config.BeaconUnixTime ;
  doc["BeaconFreq"] = ESP_Config.BeaconFreq ;
  doc["BeaconPower"] = ESP_Config.BeaconPower ;
  doc["BeaconWorkConditions"] = ESP_Config.BeaconWorkConditions ;


  // Serialize JSON to file
  if (serializeJson(doc, file) == 0) {
    Serial.print("Failed to write to file\r\n");
  }

  // Close the file
  file.close();
  Serial.print("Saved Configuration to flash file\r\n");

  return ;
}

// Prints the content of a file to the Serial
void printFile(const char *filename) {
  Serial.printf("try opening %s\r\n",filename );

  // Open file for reading
  File x_file = LittleFS.open(filename);
  if (!x_file) {
    Serial.print("Failed to read file\r\n");
    return;
  }

  // Extract each characters by one by one
  while (x_file.available()) {
    Serial.printf("%s",(char)x_file.read());
  }
  Serial.printf("\r\n");

  // Close the file
  x_file.close();
}

void setup_LittleFS_Config( void ) {

    // Should load default config if run for the first time
    Serial.print("Loading LittleFS configuration...\r\n");
    loadConfigurationFromFlash( filename );

    // Create configuration file and save present configuration 
    Serial.print("Saving LittleFS configuration...\r\n");
    saveConfigurationToFlash(filename);

    // Dump config file
    Serial.print("Print LittleFS config file...\r\n");
    printFile(filename);

    DisplayConfig_serial();
}
