// global.cpp

#include "global.h"
#include "hw_def.h"


/*
**
** CONFIGURATION HANDLING
**
*/

void WriteConfig(void)
{
  char char_array[64 + 1]; 
  if( ! save_config_enable) { debugA("save_config disabled: aborting "); return;  };

#ifdef GET_CONFIG_FROM_FRAM 

if( has_FM24W256 ){  
  if( !(xSemaphoreTake(i2c_mutex_v, ( TickType_t ) 2) )){ debugA("WriteConfig: xSemaphoreTake FAILED: aborting "); return;  };
  Serial.println("Writing Config to FRAM");

  fram.write8(0,'C');
  fram.write8(1,'F');
  fram.write8(2,'G');
  fram.write8(16,ESP_Config.dhcp);
  fram.write8(17,ESP_Config.daylight);
  fram.write32(18, ESP_Config.Update_Time_Via_NTP_Every);
  fram.write32(22, ESP_Config.timezone);
  fram.write8(26,ESP_Config.LED_R);
  fram.write8(27,ESP_Config.LED_G);
  fram.write8(28,ESP_Config.LED_B);
  fram.write8(29,ESP_Config.BeaconId);
  fram.write8(30,ESP_Config.BeaconSeqNbr);
  fram.write8(31,ESP_Config.BeaconLocation);
  fram.write8(32,ESP_Config.IP[0]);
  fram.write8(33,ESP_Config.IP[1]);
  fram.write8(34,ESP_Config.IP[2]);
  fram.write8(35,ESP_Config.IP[3]);

  fram.write8(36,ESP_Config.Netmask[0]);
  fram.write8(37,ESP_Config.Netmask[1]);
  fram.write8(38,ESP_Config.Netmask[2]);
  fram.write8(39,ESP_Config.Netmask[3]);

  fram.write8(40,ESP_Config.Gateway[0]);
  fram.write8(41,ESP_Config.Gateway[1]);
  fram.write8(42,ESP_Config.Gateway[2]);
  fram.write8(43,ESP_Config.Gateway[3]);

  fram.write8(44,ESP_Config.Dns[0]);
  fram.write8(45,ESP_Config.Dns[1]);
  fram.write8(46,ESP_Config.Dns[2]);
  fram.write8(47,ESP_Config.Dns[3]);

  fram.write8(48,ESP_Config.BeaconEngineType);
  fram.write8(49,ESP_Config.BeaconRun);
  fram.write8(50,ESP_Config.BCN_TimeSlotOffset);
  fram.write8(51,ESP_Config.BCN_TimeSlotSync);
  fram.write8(52,ESP_Config.BeaconStatsCollector_IP[0]);
  fram.write8(53,ESP_Config.BeaconStatsCollector_IP[1]);
  fram.write8(54,ESP_Config.BeaconStatsCollector_IP[2]);
  fram.write8(55,ESP_Config.BeaconStatsCollector_IP[3]);
  fram.write8(56,lowByte(ESP_Config.BeaconStatsCollector_Port));
  fram.write8(57,highByte(ESP_Config.BeaconStatsCollector_Port));

  fram.write8(58,ESP_Config.BeaconUnixTime);
  fram.write8(59,ESP_Config.BeaconFreq);
  fram.write8(60,ESP_Config.BeaconPower);
  fram.write8(61,ESP_Config.BeaconWorkConditions);
  fram.write8(62,ESP_Config.APRS_debug);

  strcpy(char_array, ESP_Config.ssid.c_str()); fram.write(64 ,(uint8_t *)char_array, 32);
  strcpy(char_array, ESP_Config.password.c_str()); fram.write(96 ,(uint8_t *)char_array, 32);
  strcpy(char_array, ESP_Config.ntpServerName.c_str()); fram.write(128 ,(uint8_t *)char_array, 32);

  fram.write8(180,ESP_Config.AutoTurnOn);
  fram.write8(181,ESP_Config.AutoTurnOff);
  fram.write8(182,ESP_Config.TurnOnHour);
  fram.write8(183,ESP_Config.TurnOnMinute);
  fram.write8(184,ESP_Config.TurnOffHour);
  fram.write8(185,ESP_Config.TurnOffMinute);
//  fram.write8(2015,ESP_Config.RebootTag);
  
  fram.write8(200,ESP_Config.gps_debug);
  fram.write8(201,ESP_Config.LoRa_debug);
  
  fram.write8(202,ESP_Config.RTC_debug);
  fram.write8(203,ESP_Config.ezTime_debug);
  fram.write8(204,ESP_Config.pps_debug);
  fram.write8(205,ESP_Config.PE_debug);
  fram.write8(982,ESP_Config.BT_KISS_Mode);
  fram.write8(983,ESP_Config.LocationCompression);
  fram.write8(984,ESP_Config.AgileBeaconing);
  fram.write8(985,ESP_Config.BlackList);



  fram.write8(207,ESP_Config.Serial_KISS_Mode);
  
  fram.write8(208,ESP_Config.mqtt_enable);
  fram.write8(209,ESP_Config.WebConfig_debug);
  fram.write8(210,ESP_Config.standalone);
  fram.write8(974,ESP_Config.syslog_enable);
  
  fram.write8(211,ESP_Config.no_gps);  
  fram.write8(212,ESP_Config.iGate_Mode);
  fram.write8(213,ESP_Config.Admin_Mode);
  fram.write8(214,ESP_Config.Reboot_Now);

  fram.write8(215,ESP_Config.EnableRfToIs);
  fram.write8(216,ESP_Config.EnableIsToRf);
  fram.write8(217,ESP_Config.EnableRepeater);
  fram.write8(218,ESP_Config.Tracker_Mode);
  fram.write8(219,ESP_Config.IoT_enable);

  fram.write8(220,ESP_Config.LoraSf);
  fram.write8(221,ESP_Config.LoraCodingRate);
  fram.write8(222,ESP_Config.LoraSync);
  fram.write8(223,ESP_Config.Beacon_Mode);
  
  fram.write8(224,lowByte(ESP_Config.AprsPort));
  fram.write8(225,highByte(ESP_Config.AprsPort));

  fram.write8(226,lowByte(ESP_Config.AprsRawBeaconPeriodSecondsIGATE));
  fram.write8(227,highByte(ESP_Config.AprsRawBeaconPeriodSecondsIGATE));  

  fram.write8(228,lowByte(ESP_Config.AprsRawBeaconPeriodSecondsTRACK));
  fram.write8(229,highByte(ESP_Config.AprsRawBeaconPeriodSecondsTRACK));

  fram.write8(230,lowByte(ESP_Config.LoraPower));
  fram.write8(231,highByte(ESP_Config.LoraPower));

  fram.write8(232,lowByte(ESP_Config.LoraFreqCorr));
  fram.write8(233,highByte(ESP_Config.LoraFreqCorr));
  fram.write8(234,ESP_Config.LoraPreambleLen);
  fram.write8(235,ESP_Config.EncapType);


  strcpy(char_array, ESP_Config.Loc_Lat.c_str()); fram.write(236 ,(uint8_t *)char_array, 10);
  strcpy(char_array, ESP_Config.Loc_Lon.c_str()); fram.write(246 ,(uint8_t *)char_array, 10);

  strcpy(char_array, ESP_Config.AprsHost.c_str()); fram.write(256 ,(uint8_t *)char_array, 32);
  strcpy(char_array, ESP_Config.AprsLogin.c_str()); fram.write(288 ,(uint8_t *)char_array, 32);
  strcpy(char_array, ESP_Config.AprsPass.c_str()); fram.write(320 ,(uint8_t *)char_array, 32);
  strcpy(char_array, ESP_Config.AprsFilter.c_str()); fram.write(352 ,(uint8_t *)char_array, 32);
  strcpy(char_array, ESP_Config.AprsRawBeaconIGATE.c_str()); fram.write(384 ,(uint8_t *)char_array, 64);
  strcpy(char_array, ESP_Config.AprsRawBeaconTRACK.c_str()); fram.write(448 ,(uint8_t *)char_array, 64);


  strcpy(char_array, ESP_Config.DeviceName.c_str()); fram.write(512 ,(uint8_t *)char_array, 32);
  strcpy(char_array, ESP_Config.DeviceId.c_str()); fram.write(544 ,(uint8_t *)char_array, 32);

  strcpy(char_array, ESP_Config.LoraFreq.c_str()); fram.write(576 ,(uint8_t *)char_array, 16);
  strcpy(char_array, ESP_Config.LoraBw.c_str()); fram.write(592 ,(uint8_t *)char_array, 16);

  strcpy(char_array, ESP_Config.AprsLoggerAddr.c_str()); fram.write(608 ,(uint8_t *)char_array, 32);
  fram.write8(640,lowByte(ESP_Config.AprsLoggerPort));
  fram.write8(641,highByte(ESP_Config.AprsLoggerPort));
  
  fram.write8(642,lowByte(ESP_Config.IoT_Port));
  fram.write8(643,highByte(ESP_Config.IoT_Port));
  strcpy(char_array, ESP_Config.IoT_Host.c_str()); fram.write(644 ,(uint8_t *)char_array, 32);
  strcpy(char_array, ESP_Config.IoT_Login.c_str()); fram.write(676 ,(uint8_t *)char_array, 32);
  strcpy(char_array, ESP_Config.IoT_Key.c_str()); fram.write(708 ,(uint8_t *)char_array, 32);

  strcpy(char_array, ESP_Config.feed_1.c_str()); fram.write(740 ,(uint8_t *)char_array, 16);
  strcpy(char_array, ESP_Config.feed_2.c_str()); fram.write(756 ,(uint8_t *)char_array, 16);
  strcpy(char_array, ESP_Config.feed_3.c_str()); fram.write(772 ,(uint8_t *)char_array, 16);
  strcpy(char_array, ESP_Config.feed_4.c_str()); fram.write(788 ,(uint8_t *)char_array, 16);
  strcpy(char_array, ESP_Config.feed_5.c_str()); fram.write(804 ,(uint8_t *)char_array, 16);
  strcpy(char_array, ESP_Config.feed_6.c_str()); fram.write(820 ,(uint8_t *)char_array, 16);

  fram.write8(960,ESP_Config.i2c_sda);
  fram.write8(961,ESP_Config.i2c_scl);
  fram.write8(962,ESP_Config.spi_sck);
  fram.write8(963,ESP_Config.spi_miso);
  fram.write8(964,ESP_Config.spi_mosi);
  fram.write8(965,ESP_Config.oled_addr);
  fram.write8(966,ESP_Config.oled_rst);
  fram.write8(967,ESP_Config.oled_orient);
  fram.write8(968,ESP_Config.lora_cs);
  fram.write8(969,ESP_Config.lora_rst);
  fram.write8(970,ESP_Config.lora_dio);
  fram.write8(971,ESP_Config.gps_rx);
  fram.write8(972,ESP_Config.gps_tx);
  fram.write8(973,ESP_Config.PayloadStyle);
  fram.write8(975,ESP_Config.RepeaterOperation);

  fram.write8(976,ESP_Config.V5_ena);
  fram.write8(977,ESP_Config.V24_1_ena);
  fram.write8(978,ESP_Config.V24_2_ena);
  fram.write8(979,ESP_Config.V24_3_ena);
  fram.write8(980,ESP_Config.ps_debug);
  fram.write8(981,ESP_Config.wifi_enable);
  fram.write8(206,ESP_Config.TCP_KISS_Mode);

  
  strcpy(char_array, ESP_Config.BeaconQRALocator.c_str()); fram.write(836 ,(uint8_t *)char_array, 16);
  strcpy(char_array, ESP_Config.BCN_LoRa_Vector.c_str()); fram.write(852 ,(uint8_t *)char_array, 64);
  xSemaphoreGive(i2c_mutex_v);
  Serial.println("End of FRAM save configuration");
  }  // end of has_FM24W256
  saveConfigurationToFlash("/WebConf.conf");  // save to LittleFS FS just in case
//   printFile("/WebConf.conf"); 
#else
  saveConfigurationToFlash("/WebConf.conf");  // to LittleFS FS
//  printFile("/WebConf.conf");
#endif  
  // update local configuration dependent data
  UpdateLocalConfigData();
  if(WebConfig_debug) DisplayConfig(); // display configuration data
  return ;
}


bool ReadConfig(void )
{
  char str[64];

Serial.printf("ReadConfig: try reading configuration from FRAM or Flash...\r\n");

#ifdef GET_CONFIG_FROM_FRAM 
Serial.printf("ReadConfig: found request for getting  configuration from FRAM ...\r\n");

if( has_FM24W256 ){     
  Serial.println("Reading Device Configuration from FRAM");
  if (fram.read8(0) == 'C' && fram.read8(1) == 'F'  && fram.read8(2) == 'G' ) {
    Serial.println("Configuration Found!");
    ESP_Config.dhcp =   fram.read8(16);

    ESP_Config.daylight = fram.read8(17);
    ESP_Config.Update_Time_Via_NTP_Every = fram.read32(18);
    ESP_Config.timezone = fram.read32(22);

    ESP_Config.LED_R = fram.read8(26);
    ESP_Config.LED_G = fram.read8(27);
    ESP_Config.LED_B = fram.read8(28);
    ESP_Config.BeaconId = fram.read8(29);
    ESP_Config.BeaconSeqNbr = fram.read8(30);
    ESP_Config.BeaconLocation = fram.read8(31);
    ESP_Config.IP[0] = fram.read8(32);
    ESP_Config.IP[1] = fram.read8(33);
    ESP_Config.IP[2] = fram.read8(34);
    ESP_Config.IP[3] = fram.read8(35);
    ESP_Config.Netmask[0] = fram.read8(36);
    ESP_Config.Netmask[1] = fram.read8(37);
    ESP_Config.Netmask[2] = fram.read8(38);
    ESP_Config.Netmask[3] = fram.read8(39);
    ESP_Config.Gateway[0] = fram.read8(40);
    ESP_Config.Gateway[1] = fram.read8(41);
    ESP_Config.Gateway[2] = fram.read8(42);
    ESP_Config.Gateway[3] = fram.read8(43);
    ESP_Config.Dns[0] = fram.read8(44);
    ESP_Config.Dns[1] = fram.read8(45);
    ESP_Config.Dns[2] = fram.read8(46);
    ESP_Config.Dns[3] = fram.read8(47);

    ESP_Config.BeaconEngineType = fram.read8(48);
    ESP_Config.BeaconRun = fram.read8(49);
    ESP_Config.BCN_TimeSlotOffset = fram.read8(50);
    ESP_Config.BCN_TimeSlotSync = fram.read8(51);
    ESP_Config.BeaconStatsCollector_IP[0] = fram.read8(52);
    ESP_Config.BeaconStatsCollector_IP[1] = fram.read8(53);
    ESP_Config.BeaconStatsCollector_IP[2] = fram.read8(54);
    ESP_Config.BeaconStatsCollector_IP[3] = fram.read8(55);

    ESP_Config.BeaconUnixTime = fram.read8(58);
    ESP_Config.BeaconFreq = fram.read8(59);
    ESP_Config.BeaconPower = fram.read8(60);
    ESP_Config.BeaconWorkConditions = fram.read8(61);
    ESP_Config.APRS_debug = fram.read8(62);
    ESP_Config.RebootTag = fram.read8(2015);
    

    fram.read(64, (uint8_t *)str, 32); ESP_Config.ssid = (char *)str;
    fram.read(96, (uint8_t *)str, 32); ESP_Config.password = (char *)str;
    fram.read(128, (uint8_t *)str, 32); ESP_Config.ntpServerName = (char *)str;

    ESP_Config.AutoTurnOn = fram.read8(180);
    ESP_Config.AutoTurnOff = fram.read8(181);
    ESP_Config.TurnOnHour = fram.read8(182);
    ESP_Config.TurnOnMinute = fram.read8(183);
    ESP_Config.TurnOffHour = fram.read8(184);
    ESP_Config.TurnOffMinute = fram.read8(185);
    
    ESP_Config.gps_debug = fram.read8(200);
    ESP_Config.LoRa_debug = fram.read8(201);
   
    ESP_Config.RTC_debug = fram.read8(202);
    ESP_Config.ezTime_debug = fram.read8(203);
    ESP_Config.pps_debug = fram.read8(204);
    ESP_Config.PE_debug = fram.read8(205);
    ESP_Config.BT_KISS_Mode = fram.read8(982);
    ESP_Config.Serial_KISS_Mode = fram.read8(207);
    
    ESP_Config.mqtt_enable = fram.read8(208);
    ESP_Config.WebConfig_debug = fram.read8(209);
    ESP_Config.standalone = fram.read8(210);
    ESP_Config.syslog_enable = fram.read8(974);
    
    ESP_Config.no_gps = fram.read8(211);  
    ESP_Config.iGate_Mode = fram.read8(212);
    ESP_Config.Admin_Mode = fram.read8(213);
    ESP_Config.Reboot_Now = fram.read8(214);

    ESP_Config.EnableRfToIs = fram.read8(215);
    ESP_Config.EnableIsToRf = fram.read8(216);
    ESP_Config.EnableRepeater = fram.read8(217);
    ESP_Config.Tracker_Mode = fram.read8(218);
    ESP_Config.IoT_enable = fram.read8(219);


    ESP_Config.LoraSf = fram.read8(220);
    ESP_Config.LoraCodingRate = fram.read8(221);
    ESP_Config.LoraSync = fram.read8(222);
    ESP_Config.Beacon_Mode = fram.read8(223);

    uint8_t a[2] = {0, 0};
    a[0] = fram.read8(56);  a[1] = fram.read8(57); ESP_Config.BeaconStatsCollector_Port = word( a[1], a[0]) ;
    
    a[0] = fram.read8(224);  a[1] = fram.read8(225); ESP_Config.AprsPort = word( a[1], a[0]) ;
    a[0] = fram.read8(226);  a[1] = fram.read8(227); ESP_Config.AprsRawBeaconPeriodSecondsIGATE = word( a[1], a[0]) ;
    a[0] = fram.read8(228);  a[1] = fram.read8(229); ESP_Config.AprsRawBeaconPeriodSecondsTRACK = word( a[1], a[0]) ;
    a[0] = fram.read8(230);  a[1] = fram.read8(231); ESP_Config.LoraPower = word( a[1], a[0]) ; 
    if ( ESP_Config.LoraPower >= 32768 ) ESP_Config.LoraPower = ESP_Config.LoraPower - 65536;
    a[0] = fram.read8(232);  a[1] = fram.read8(233); ESP_Config.LoraFreqCorr = word( a[1], a[0]) ;
    if ( ESP_Config.LoraFreqCorr >= 32768 ) ESP_Config.LoraFreqCorr = ESP_Config.LoraFreqCorr - 65536;

    ESP_Config.LoraPreambleLen = fram.read8(234);
    ESP_Config.EncapType = fram.read8(235);

    fram.read(236, (uint8_t *)str, 10); ESP_Config.Loc_Lat = (char *)str;
    fram.read(246, (uint8_t *)str, 10); ESP_Config.Loc_Lon = (char *)str;

    fram.read(256, (uint8_t *)str, 32); ESP_Config.AprsHost = (char *)str;
    fram.read(288, (uint8_t *)str, 32); ESP_Config.AprsLogin = (char *)str;
    fram.read(320, (uint8_t *)str, 32); ESP_Config.AprsPass = (char *)str;
    fram.read(352, (uint8_t *)str, 32); ESP_Config.AprsFilter = (char *)str;
    fram.read(384, (uint8_t *)str, 64); ESP_Config.AprsRawBeaconIGATE = (char *)str;
    fram.read(448, (uint8_t *)str, 64); ESP_Config.AprsRawBeaconTRACK = (char *)str;

    fram.read(512, (uint8_t *)str, 32); ESP_Config.DeviceName = (char *)str;
    fram.read(544, (uint8_t *)str, 32); ESP_Config.DeviceId = (char *)str;

    fram.read(576, (uint8_t *)str, 16); ESP_Config.LoraFreq = (char *)str;
    fram.read(592, (uint8_t *)str, 16); ESP_Config.LoraBw = (char *)str;

    fram.read(608, (uint8_t *)str, 32); ESP_Config.AprsLoggerAddr = (char *)str;
    a[0] = fram.read8(640);  a[1] = fram.read8(641); ESP_Config.AprsLoggerPort = word( a[1], a[0]) ;

    a[0] = fram.read8(642);  a[1] = fram.read8(643); ESP_Config.IoT_Port = word( a[1], a[0]) ;
    fram.read(644, (uint8_t *)str, 32); ESP_Config.IoT_Host = (char *)str;
    fram.read(676, (uint8_t *)str, 32); ESP_Config.IoT_Login = (char *)str;
    fram.read(708, (uint8_t *)str, 32); ESP_Config.IoT_Key = (char *)str;

    fram.read(740, (uint8_t *)str, 16); ESP_Config.feed_1 = (char *)str;
    fram.read(756, (uint8_t *)str, 16); ESP_Config.feed_2 = (char *)str;
    fram.read(772, (uint8_t *)str, 16); ESP_Config.feed_3 = (char *)str;
    fram.read(788, (uint8_t *)str, 16); ESP_Config.feed_4 = (char *)str;
    fram.read(804, (uint8_t *)str, 16); ESP_Config.feed_5 = (char *)str;
    fram.read(820, (uint8_t *)str, 16); ESP_Config.feed_6 = (char *)str;

    fram.read(836, (uint8_t *)str, 16); ESP_Config.BeaconQRALocator = (char *)str;
    fram.read(852, (uint8_t *)str, 64); ESP_Config.BCN_LoRa_Vector = (char *)str;

    ESP_Config.i2c_sda = fram.read8(960);
    ESP_Config.i2c_scl = fram.read8(961);
    ESP_Config.spi_sck = fram.read8(962);
    ESP_Config.spi_miso = fram.read8(963);
    ESP_Config.spi_mosi = fram.read8(964);
    ESP_Config.oled_addr = fram.read8(965);
    ESP_Config.oled_rst = fram.read8(966);
    ESP_Config.oled_orient = fram.read8(967);
    ESP_Config.lora_cs = fram.read8(968);
    ESP_Config.lora_rst = fram.read8(969);
    ESP_Config.lora_dio = fram.read8(970);
    ESP_Config.gps_rx = fram.read8(971);
    ESP_Config.gps_tx = fram.read8(972);
    ESP_Config.PayloadStyle = fram.read8(973);
    ESP_Config.RepeaterOperation = fram.read8(975);
    ESP_Config.LocationCompression = fram.read8(983);
    ESP_Config.AgileBeaconing = fram.read8(984);
    ESP_Config.BlackList = fram.read8(985);
 


    ESP_Config.V5_ena = fram.read8(976);
    ESP_Config.V24_1_ena = fram.read8(977); 
    ESP_Config.V24_2_ena = fram.read8(978);
    ESP_Config.V24_3_ena = fram.read8(979);
    ESP_Config.ps_debug = fram.read8(980);
    ESP_Config.wifi_enable = fram.read8(981);
    ESP_Config.TCP_KISS_Mode = fram.read8(206);
    
    }   // end of check of CFG tag in FRAM
  else {   // no configuration TAG is in the FRAM so config is invalid
    Serial.println("FRAM Configuration NOT FOUND or invalid!!!!");
    int cntr=30 ;
    Serial.print("\n\rDevice configuration non found: forcing defaults... ");      
    while (cntr-- > 0){
       Serial.print(".");
       }; 
    Serial.println("\n\rRequest to load default configuration.. ");
    return false;     
    };    //end of configuration TAG test in the FRAM 
  }  // end of Config from FRAM case  if FRAN is equipped
else {  // just in case it is not equipped just fallback on flash storage
  // no FRAM available... so try loading config from flash FS
  Serial.printf("ReadConfig: FRAM not available ... getting  configuration from Flash ...\r\n");

  if(  ! loadConfigurationFromFlash("/WebConf.conf")) {   // load config from LittleFS
    Serial.println("Configurarion NOT FOUND in flash 2 !!!!");
    int cntr=30 ;
    Serial.print("\n\rDevice configuration non found: forcing defaults... ");      
    while (cntr-- > 0){
       Serial.print(".");
       }; 
    Serial.println("\n\rRequest to load default configuration.. ");
    return false;
    };
 }; 
#else
  // no FRAM available... so try loading config from flash FS
  Serial.printf("ReadConfig: found request for getting  configuration from Flash FS ...\r\n");

  if(  ! loadConfigurationFromFlash("/WebConf.conf")) {   // load config from LittleFS
    Serial.println("Configurarion NOT FOUND in flash 2 !!!!");
    int cntr=30 ;
    Serial.print("\n\rDevice configuration non found: forcing defaults... ");      
    while (cntr-- > 0){
       Serial.print(".");
       }; 
    Serial.println("\n\rRequest to load default configuration.. ");
    return false;
    };
#endif  

  UpdateLocalConfigData();
  if(WebConfig_debug) DisplayConfig(); // display configuration data
  return true;
}


// to generate a beacon prototype starting from user simplified inputs
String textToBeaconProto( String BeaconText ) {
  //debugA("textToBeaconProto input  ==============> %s \r",BeaconText.c_str() );
  char *strings[5]; char *ptr = NULL; byte index = 0; char array_loc[40] = "" ;
  // BeaconText= "I1XYZ-10,WIDE1-1,LoRa iGate,[,L";
  BeaconText.replace(" ","_");
  BeaconText.toCharArray( array_loc ,BeaconText.length()+1 );
  ptr = strtok(array_loc, ",");  // takes a list of delimiters
  while(ptr != NULL){
    strings[index] = ptr;
    index++;
    ptr = strtok(NULL, ",");  // takes a list of delimiters
    }; 
  // if(LoRa_debug) for(int n = 0; n < index; n++){  debugA("textToBeaconProto ==============> %s \r",strings[n]); } ;
  String output = strings[0] + 
  String(">APLS01,") + strings[1]  + String(":!")+ 
  String("GPS_LAT")+ strings[4] +    // change this to alter APRS symbol table
  String("GPS_LON")+ strings[3]+ 
//  strings[2] +             // this is our magic tag LoRa
  String("LoRa")+       // placeholder for the sequencenumber
  String(" BLK_TAG")+       // placeholder for the blasklisted tag
  String("SQN_NBR")+       // placeholder for the sequencenumber
  String("APRS_WC") ;     // placeholder for the APRS_WC


  APRS_WCtag = "" +
  String("B")+ ESP_Config.LoraBw + 
  String("S")+ ESP_Config.LoraSf + 
  String("C")+ ESP_Config.LoraCodingRate + 
  String("P")+ ESP_Config.LoraPreambleLen ;

  // if(LoRa_debug) debugA("textToBeaconProto output ==============> %s \r",output.c_str() );
  return (output);
}



void UpdateLocalConfigData(void ){ 
   gps_debug = ESP_Config.gps_debug ;
   LoRa_debug = ESP_Config.LoRa_debug ;
   APRS_debug = ESP_Config.APRS_debug ;
   RebootTag = ESP_Config.RebootTag ;
   RTC_debug = ESP_Config.RTC_debug ;
   ezTime_debug = ESP_Config.ezTime_debug ;
   pps_debug = ESP_Config.pps_debug ;
   PE_debug = ESP_Config.PE_debug ;
   BT_KISS_Mode = ESP_Config.BT_KISS_Mode ;
   Serial_KISS_Mode = ESP_Config.Serial_KISS_Mode ;
   TCP_KISS_Mode = ESP_Config.TCP_KISS_Mode ;
   Tracker_Mode = ESP_Config.Tracker_Mode ;
   IoT_enable = ESP_Config.IoT_enable ;
   mqtt_enable = ESP_Config.mqtt_enable ;
   WebConfig_debug = ESP_Config.WebConfig_debug ;
   no_gps = ESP_Config.no_gps ;  
   iGate_Mode = ESP_Config.iGate_Mode ;
   Beacon_Mode = ESP_Config.Beacon_Mode ;
   Admin_Mode = ESP_Config.Admin_Mode ;
   Reboot_Now = ESP_Config.Reboot_Now;
   EnableRfToIs = ESP_Config.EnableRfToIs;
   EnableIsToRf = ESP_Config.EnableIsToRf;
   EnableRepeater = ESP_Config.EnableRepeater;
   wifi_enable = ESP_Config.wifi_enable ;
   standalone = ESP_Config.standalone ;
   syslog_enable = ESP_Config.syslog_enable ;

#ifndef HW_TYPE_SARIMESH

    i2c_sda= ESP_Config.i2c_sda;
    i2c_scl= ESP_Config.i2c_scl;

    spi_sck= ESP_Config.spi_sck;
    spi_miso= ESP_Config.spi_miso;
    spi_mosi= ESP_Config.spi_mosi;

    oled_addr= ESP_Config.oled_addr;
    oled_rst= ESP_Config.oled_rst;
    oled_orient= ESP_Config.oled_orient;

    lora_cs= ESP_Config.lora_cs;
    lora_rst= ESP_Config.lora_rst;
    lora_dio= ESP_Config.lora_dio;
    
    gps_rx= ESP_Config.gps_rx;
    gps_tx= ESP_Config.gps_tx;
#endif   

#ifdef LORA_FUNCTIONS_ENABLE 

// Beacon app parameters
  BeaconId = ESP_Config.BeaconId ;
  BeaconSeqNbr = ESP_Config.BeaconSeqNbr;
  BeaconLocation = ESP_Config.BeaconLocation ;
  BeaconEngineType = ESP_Config.BeaconEngineType ;
  BeaconRun = ESP_Config.BeaconRun ;
  BCN_TimeSlotOffset = ESP_Config.BCN_TimeSlotOffset ;
  BCN_TimeSlotSync = ESP_Config.BCN_TimeSlotSync ;
  BeaconStatsCollector_IP[0] = ESP_Config.BeaconStatsCollector_IP[0];
  BeaconStatsCollector_IP[1] = ESP_Config.BeaconStatsCollector_IP[1];
  BeaconStatsCollector_IP[2] = ESP_Config.BeaconStatsCollector_IP[2];
  BeaconStatsCollector_IP[3] = ESP_Config.BeaconStatsCollector_IP[3];
  BeaconStatsCollector_Port = ESP_Config.BeaconStatsCollector_Port ;
  BeaconQRALocator = ESP_Config.BeaconQRALocator;
  BCN_LoRa_Vector = ESP_Config.BCN_LoRa_Vector ;  
  BeaconUnixTime = ESP_Config.BeaconUnixTime ;  
  BeaconFreq = ESP_Config.BeaconFreq ;  
  BeaconPower = ESP_Config.BeaconPower ;  
  BeaconWorkConditions = ESP_Config.BeaconWorkConditions ;  

// APRS Compatibility  
  PayloadStyle= ESP_Config.PayloadStyle;
  RepeaterOperation= ESP_Config.RepeaterOperation;
  EncapType=ESP_Config.EncapType ;
  LocationCompression = ESP_Config.LocationCompression;
  BlackList = ESP_Config.BlackList;

  AgileBeaconing = ESP_Config.AgileBeaconing;

 
#endif //  LORA_FUNCTIONS_ENABLE 
   ps_debug= ESP_Config.ps_debug;
   wifi_enable= ESP_Config.wifi_enable;
}



bool DisplayConfig_serial(void ){
  Serial.print("Display active Configuration data\n");
  Serial.printf("ESP_Config.DeviceName  = %s \n\r", ESP_Config.DeviceName.c_str() );
  Serial.printf("ESP_Config.DeviceId  = %s \n\r", ESP_Config.DeviceId.c_str() );
  Serial.printf("ESP_Config.cpu_type  = %s \n\r", ESP_Config.cpu_type.c_str() );
  
  Serial.printf("ESP_Config.dhcp        = %d \n\r", ESP_Config.dhcp );
  Serial.printf("ESP_Config.daylight    = %d \n\r", ESP_Config.daylight );
  Serial.printf("ESP_Config.Update_Time = %d \n\r", ESP_Config.Update_Time_Via_NTP_Every );
  Serial.printf("ESP_Config.timezone    = %d \n\r", ESP_Config.timezone );
//  Serial.printf("ESP_Config.LED_R       = %d \n\r", ESP_Config.LED_R );
//  Serial.printf("ESP_Config.LED_G       = %d \n\r", ESP_Config.LED_G );
//  Serial.printf("ESP_Config.LED_B       = %d \n\r", ESP_Config.LED_B );
  Serial.printf("ESP_Config.IP          = %d.%d.%d.%d \n\r", ESP_Config.IP[0],ESP_Config.IP[1],ESP_Config.IP[2],ESP_Config.IP[3] );
  Serial.printf("ESP_Config.Netmask     = %d.%d.%d.%d \n\r", ESP_Config.Netmask[0],ESP_Config.Netmask[1],ESP_Config.Netmask[2],ESP_Config.Netmask[3] );
  Serial.printf("ESP_Config.GatewayIP   = %d.%d.%d.%d \n\r", ESP_Config.Gateway[0],ESP_Config.Gateway[1],ESP_Config.Gateway[2],ESP_Config.Gateway[3] );
  Serial.printf("ESP_Config.DnsIP       = %d.%d.%d.%d \n\r", ESP_Config.Dns[0],ESP_Config.Dns[1],ESP_Config.Dns[2],ESP_Config.Dns[3] );
  Serial.printf("ESP_Config.ssid        = %s \n\r", ESP_Config.ssid.c_str() );
  Serial.printf("ESP_Config.password    = %s \n\r", ESP_Config.password.c_str() );
  Serial.printf("ESP_Config.ntpServer   = %s \n\r", ESP_Config.ntpServerName.c_str() );
//  Serial.printf("ESP_Config.AutoTurnOn  = %d \n\r", ESP_Config.AutoTurnOn );
//  Serial.printf("ESP_Config.AutoTurnOff = %d \n\r", ESP_Config.AutoTurnOff );
//  Serial.printf("ESP_Config.TurnOnHour  = %d \n\r", ESP_Config.TurnOnHour );
//  Serial.printf("ESP_Config.TurnOnMin   = %d \n\r", ESP_Config.TurnOnMinute );
//  Serial.printf("ESP_Config.TurnOffHour = %d \n\r", ESP_Config.TurnOffHour );
//  Serial.printf("ESP_Config.TurnOffMin  = %d \n\r", ESP_Config.TurnOffMinute );

  Serial.printf("ESP_Config.gps_debug  = %d \n\r", ESP_Config.gps_debug );
  Serial.printf("ESP_Config.LoRa_debug  = %d \n\r", ESP_Config.LoRa_debug );
  Serial.printf("ESP_Config.APRS_debug  = %d \n\r", ESP_Config.APRS_debug );
  Serial.printf("ESP_Config.RebootTag  = %d \n\r", ESP_Config.RebootTag );

  Serial.printf("ESP_Config.RTC_debug  = %d \n\r", ESP_Config.RTC_debug );
  Serial.printf("ESP_Config.ezTime_debug  = %d \n\r", ESP_Config.ezTime_debug );
  Serial.printf("ESP_Config.pps_debug  = %d \n\r", ESP_Config.pps_debug );
  Serial.printf("ESP_Config.PE_debug  = %d \n\r", ESP_Config.PE_debug );
  Serial.printf("ESP_Config.BT_KISS_Mode  = %d \n\r", ESP_Config.BT_KISS_Mode );
  Serial.printf("ESP_Config.Serial_KISS_Mode  = %d \n\r", ESP_Config.Serial_KISS_Mode );
  Serial.printf("ESP_Config.TCP_KISS_Mode  = %d \n\r", ESP_Config.TCP_KISS_Mode );
  
  Serial.printf("ESP_Config.Tracker_Mode  = %d \n\r", ESP_Config.Tracker_Mode );
  Serial.printf("ESP_Config.IoT_enable  = %d \n\r", ESP_Config.IoT_enable );

#ifdef PS_FUNCTIONS_ENABLE 
  Serial.printf("ESP_Config.V5_ena  = %d \n\r", ESP_Config.V5_ena );
  Serial.printf("ESP_Config.V24_1_ena  = %d \n\r", ESP_Config.V24_1_ena );
  Serial.printf("ESP_Config.V24_2_ena  = %d \n\r", ESP_Config.V24_2_ena );
  Serial.printf("ESP_Config.V24_3_ena  = %d \n\r", ESP_Config.V24_3_ena ); 
  Serial.printf("ESP_Config.PS_Out_Reset  = %d \n\r", ESP_Config.PS_Out_Reset );
  Serial.printf("ESP_Config.Bat_discarge  = %d \n\r", ESP_Config.Bat_discarge );
#endif
   
  Serial.printf("ESP_Config.mqtt_enable  = %d \n\r", ESP_Config.mqtt_enable );
  Serial.printf("ESP_Config.WebConfig_debug  = %d \n\r", ESP_Config.WebConfig_debug );

  Serial.printf("ESP_Config.no_gps  = %d \n\r", ESP_Config.no_gps );
  Serial.printf("ESP_Config.iGate_Mode  = %d \n\r", ESP_Config.iGate_Mode );
  Serial.printf("ESP_Config.Beacon_Mode  = %d \n\r", ESP_Config.Beacon_Mode );
  
  Serial.printf("ESP_Config.Admin_Mode  = %d \n\r", ESP_Config.Admin_Mode );
  Serial.printf("ESP_Config.Reboot_Now  = %d \n\r", ESP_Config.Reboot_Now );

  Serial.printf("ESP_Config.EnableRfToIs  = %d \n\r", ESP_Config.EnableRfToIs );
  Serial.printf("ESP_Config.EnableIsToRf  = %d \n\r", ESP_Config.EnableIsToRf );
  Serial.printf("ESP_Config.EnableRepeater  = %d \n\r", ESP_Config.EnableRepeater );

  Serial.printf("ESP_Config.Loc_Lat  = %s \n\r", ESP_Config.Loc_Lat.c_str() );
  Serial.printf("ESP_Config.Loc_Lon  = %s \n\r", ESP_Config.Loc_Lon.c_str() );

  Serial.printf("ESP_Config.AprsHost  = %s \n\r", ESP_Config.AprsHost.c_str() );
  Serial.printf("ESP_Config.AprsPort  = %d \n\r", ESP_Config.AprsPort );
  Serial.printf("ESP_Config.AprsLogin = %s \n\r", ESP_Config.AprsLogin.c_str() );
  Serial.printf("ESP_Config.AprsPass  = %s \n\r", ESP_Config.AprsPass.c_str() );
  Serial.printf("ESP_Config.AprsFilter = %s \n\r", ESP_Config.AprsFilter.c_str() );
  Serial.printf("ESP_Config.AprsRawBeaconIGATE = %s \n\r", ESP_Config.AprsRawBeaconIGATE.c_str() );
  Serial.printf("ESP_Config.AprsRawBeaconTRACK = %s \n\r", ESP_Config.AprsRawBeaconTRACK.c_str() );
  Serial.printf("ESP_Config.AprsRawBeaconPeriodSecondsIGATE  = %d \n\r", ESP_Config.AprsRawBeaconPeriodSecondsIGATE );
  Serial.printf("ESP_Config.AprsRawBeaconPeriodSecondsTRACK  = %d \n\r", ESP_Config.AprsRawBeaconPeriodSecondsTRACK );
  Serial.printf("ESP_Config.AprsLoggerAddr  = %s \n\r", ESP_Config.AprsLoggerAddr.c_str() );
  Serial.printf("ESP_Config.AprsLoggerPort  = %d \n\r", ESP_Config.AprsLoggerPort );

  Serial.printf("ESP_Config.IoT_Host  = %s \n\r", ESP_Config.IoT_Host.c_str() );
  Serial.printf("ESP_Config.IoT_Port  = %d \n\r", ESP_Config.IoT_Port );
  Serial.printf("ESP_Config.IoT_Login = %s \n\r", ESP_Config.IoT_Login.c_str() );
  Serial.printf("ESP_Config.IoT_Key  = %s \n\r", ESP_Config.IoT_Key.c_str() );

  Serial.printf("ESP_Config.IoT_Key  = %s \n\r", ESP_Config.feed_1.c_str() );
  Serial.printf("ESP_Config.IoT_Key  = %s \n\r", ESP_Config.feed_2.c_str() );
  Serial.printf("ESP_Config.IoT_Key  = %s \n\r", ESP_Config.feed_3.c_str() );
  Serial.printf("ESP_Config.IoT_Key  = %s \n\r", ESP_Config.feed_4.c_str() );
  Serial.printf("ESP_Config.IoT_Key  = %s \n\r", ESP_Config.feed_5.c_str() );
  Serial.printf("ESP_Config.IoT_Key  = %s \n\r", ESP_Config.feed_6.c_str() );  
  Serial.printf("ESP_Config.LoraFreq  = %s \n\r", ESP_Config.LoraFreq );
  Serial.printf("ESP_Config.LoraBw  = %s \n\r", ESP_Config.LoraBw );
  Serial.printf("ESP_Config.LoraSf  = %d \n\r", ESP_Config.LoraSf );
  Serial.printf("ESP_Config.LoraCodingRate  = %d \n\r", ESP_Config.LoraCodingRate );
  Serial.printf("ESP_Config.LoraSync  = %d \n\r", ESP_Config.LoraSync );
  Serial.printf("ESP_Config.LoraPower  = %d \n\r", ESP_Config.LoraPower );
  Serial.printf("ESP_Config.LoraFreqCorr  = %d \n\r", ESP_Config.LoraFreqCorr );
  Serial.printf("ESP_Config.LoraPreambleLen  = %d \n\r", ESP_Config.LoraPreambleLen );
  Serial.printf("ESP_Config.EncapType  = %d \n\r", ESP_Config.EncapType );

// Beacon app parameters
  Serial.printf("ESP_Config.BeaconId  = %d \n\r", ESP_Config.BeaconId );
  Serial.printf("ESP_Config.BeaconSeqNbr  = %d \n\r", ESP_Config.BeaconSeqNbr );
  Serial.printf("ESP_Config.BeaconLocation  = %d \n\r", ESP_Config.BeaconLocation );
  Serial.printf("ESP_Config.BeaconEngineType  = %d \n\r", ESP_Config.BeaconEngineType );
  Serial.printf("ESP_Config.BeaconRun  = %d \n\r", ESP_Config.BeaconRun );
  Serial.printf("ESP_Config.BCN_TimeSlotOffset  = %d \n\r", ESP_Config.BCN_TimeSlotOffset );
  Serial.printf("ESP_Config.BCN_TimeSlotSync  = %d \n\r", ESP_Config.BCN_TimeSlotSync );
  Serial.printf("ESP_Config.BeaconStatsCollector_IP   = %d.%d.%d.%d \n\r", ESP_Config.BeaconStatsCollector_IP[0],ESP_Config.BeaconStatsCollector_IP[1],ESP_Config.BeaconStatsCollector_IP[2],ESP_Config.BeaconStatsCollector_IP[3] );
  Serial.printf("ESP_Config.BeaconStatsCollector_Port  = %d \n\r", ESP_Config.BeaconStatsCollector_Port );
  Serial.printf("ESP_Config.BeaconQRALocator  = %s \n\r", ESP_Config.BeaconQRALocator.c_str() );
  Serial.printf("ESP_Config.BCN_LoRa_Vector  = %s \n\r", ESP_Config.BCN_LoRa_Vector.c_str() );
  Serial.printf("ESP_Config.BeaconUnixTime  = %d \n\r", ESP_Config.BeaconUnixTime );  
  Serial.printf("ESP_Config.BeaconFreq  = %dd \n\r", ESP_Config.BeaconFreq );  
  Serial.printf("ESP_Config.BeaconPower = %d \n\r", ESP_Config.BeaconPower );  
  Serial.printf("ESP_Config.BeaconWorkConditions  = %d \n\r", ESP_Config.BeaconWorkConditions );  
  Serial.printf("ESP_Config.wifi_enable  = %d \n\r", ESP_Config.wifi_enable );
  Serial.printf("ESP_Config.standalone  = %d \n\r", ESP_Config.standalone );
  Serial.printf("ESP_Config.syslog_enable  = %d \n\r", ESP_Config.syslog_enable );

  Serial.println("End of Configuration data\n\r");

  Serial.println("\r\n=========>DHCP acquired data<==========");
  Serial.println("localIP:    " + (String) WiFi.localIP()[0] + "." +  (String) WiFi.localIP()[1] + "." +  (String) WiFi.localIP()[2] + "." + (String) WiFi.localIP()[3]);
  Serial.println("subnetMask: " + (String) WiFi.subnetMask()[0] + "." +  (String) WiFi.subnetMask()[1] + "." +  (String) WiFi.subnetMask()[2] + "." + (String) WiFi.subnetMask()[3]);
  Serial.println("gatewayIP:  " + (String) WiFi.gatewayIP()[0] + "." +  (String) WiFi.gatewayIP()[1] + "." +  (String) WiFi.gatewayIP()[2] + "." + (String) WiFi.gatewayIP()[3]);
//  Serial.println("dnsIP:      " + (String) WiFi.dnsIP()[0] + "." +  (String) WiFi.dnsIP()[1] + "." +  (String) WiFi.dnsIP()[2] + "." + (String) WiFi.dnsIP()[3]);
//  Serial.println("macAddress: " + (String) WiFi.macAddress()[0] + ":" +  (String) WiFi.macAddress()[1] + ":" +  (String) WiFi.macAddress()[2] + ":" + (String) WiFi.macAddress()[3] + ":" +  (String) WiFi.macAddress()[4] + ":" +  (String) WiFi.macAddress()[5] );
  if (WiFi.status() == WL_CONNECTED){
     uint8_t macAddr[6];
     WiFi.macAddress(macAddr);
     Serial.printf("macAddress: %02x:%02x:%02x:%02x:%02x:%02x\n", macAddr[0], macAddr[1], macAddr[2], macAddr[3], macAddr[4], macAddr[5]);
     };
  Serial.println("\r\n======>WiFi connection status<=========");
  Serial.printf("WiFi status: %d  ",WiFi.status()); Serial.println("( 3=WL_CONNECTED 6=WL_DISCONNECTED )");
  Serial.printf("SSID:        %s\n\r", WiFi.SSID().c_str());
  Serial.printf("BSSID:       %s\n\r", WiFi.BSSIDstr().c_str());
  Serial.printf("RSSI:        %d dBm\n\r", WiFi.RSSI());

  Serial.println("\r\n======>Local Config Dependent data status<=========");
  Serial.printf("standalone:       %d \r\n",standalone); 
  Serial.printf("syslog_enable:    %d \r\n",syslog_enable); 
  
  Serial.printf("no_gps:           %d \r\n",no_gps); 
  Serial.printf("gps_debug:        %d \r\n",gps_debug); 
  Serial.printf("RTC_debug:        %d \r\n",RTC_debug); 
  Serial.printf("LoRa_debug:       %d \r\n",LoRa_debug);
  Serial.printf("APRS_debug:       %d \r\n",APRS_debug);
  Serial.printf("RebootTag:       %d \r\n",RebootTag);

  Serial.printf("ezTime_debug:     %d \r\n",ezTime_debug); 
  Serial.printf("mqtt_enable:   %d \r\n",mqtt_enable); 
  Serial.printf("WebConfig_debug:  %d \r\n",WebConfig_debug); 
  Serial.printf("PE_debug:         %d \r\n",PE_debug); 
  Serial.printf("pps_debug:        %d \r\n",pps_debug); 
  Serial.printf("BT_KISS_Mode:     %d \r\n",BT_KISS_Mode); 
  Serial.printf("Serial_KISS_Mode: %d \r\n",Serial_KISS_Mode); 
  Serial.printf("TCP_KISS_Mode: %d \r\n",TCP_KISS_Mode); 
  
  Serial.printf("Tracker_Mode:     %d \r\n",Tracker_Mode); 
  Serial.printf("IoT_enable:       %d \r\n",IoT_enable); 

 #ifdef PS_FUNCTIONS_ENABLE
  Serial.printf("V5_ena:           %d \r\n",V5_ena);  
  Serial.printf("V24_1_ena:        %d \r\n",V24_1_ena); 
  Serial.printf("V24_2_ena:        %d \r\n",V24_2_ena); 
  Serial.printf("V24_3_ena:        %d \r\n",V24_3_ena);  
  Serial.printf("PS_Out_Reset:     %d \r\n",PS_Out_Reset); 
  Serial.printf("Bat_discarge:     %d \r\n",Bat_discarge); 
 #endif  
  
  Serial.printf("iGate_Mode:      %d \r\n",iGate_Mode); 
  Serial.printf("Beacon_Mode:      %d \r\n",Beacon_Mode); 
  Serial.printf("EnableRfToIs:      %d \r\n",EnableRfToIs); 
  Serial.printf("EnableIsToRf:      %d \r\n",EnableIsToRf); 
  Serial.printf("EnableRepeater:      %d \r\n",EnableRepeater); 

  Serial.printf("Admin_Mode:      %d \r\n",Admin_Mode); 
  Serial.printf("Reboot_Now:      %d \r\n",Reboot_Now); 
  
  Serial.println("\r\nEnd of status data\n\r");

  Serial.println("Display Inventory data");
  Serial.printf("ESP_Config.DeviceName  = %s \n\r", ESP_Config.DeviceName.c_str() );
  Serial.printf("ESP_Config.DeviceId  = %s \n\r", ESP_Config.DeviceId.c_str() );
  Serial.printf("ESP_Config.cpu_type  = %s \n\r", ESP_Config.cpu_type.c_str() );
  
  Serial.printf("ESP_Config.dhcp        = %d \n\r", ESP_Config.dhcp );

  return true;
}




bool DisplayConfig(void ) {
  debugA("Display active Configuration data\r");
  debugA("ESP_Config.DeviceName  = %s \r", ESP_Config.DeviceName.c_str() );
  
  debugA("ESP_Config.DeviceId  = %s \r", ESP_Config.DeviceId.c_str() );
  debugA("ESP_Config.cpu_type  = %s \r", ESP_Config.cpu_type.c_str() );
  
  debugA("ESP_Config.dhcp        = %d \r", ESP_Config.dhcp );
  debugA("ESP_Config.daylight    = %d \r", ESP_Config.daylight );
  debugA("ESP_Config.Update_Time = %d \r", ESP_Config.Update_Time_Via_NTP_Every );
  debugA("ESP_Config.timezone    = %d \r", ESP_Config.timezone );
//  debugA("ESP_Config.LED_R       = %d \r", ESP_Config.LED_R );
//  debugA("ESP_Config.LED_G       = %d \r", ESP_Config.LED_G );
//  debugA("ESP_Config.LED_B       = %d \r", ESP_Config.LED_B );
  debugA("ESP_Config.IP          = %d.%d.%d.%d \r", ESP_Config.IP[0],ESP_Config.IP[1],ESP_Config.IP[2],ESP_Config.IP[3] );
  debugA("ESP_Config.Netmask     = %d.%d.%d.%d \r", ESP_Config.Netmask[0],ESP_Config.Netmask[1],ESP_Config.Netmask[2],ESP_Config.Netmask[3] );
  debugA("ESP_Config.GatewayIP   = %d.%d.%d.%d \r", ESP_Config.Gateway[0],ESP_Config.Gateway[1],ESP_Config.Gateway[2],ESP_Config.Gateway[3] );
  debugA("ESP_Config.DnsIP       = %d.%d.%d.%d \r", ESP_Config.Dns[0],ESP_Config.Dns[1],ESP_Config.Dns[2],ESP_Config.Dns[3] );
  debugA("ESP_Config.ssid        = %s \r", ESP_Config.ssid.c_str() );
  debugA("ESP_Config.password    = %s \r", ESP_Config.password.c_str() );
  debugA("ESP_Config.ntpServer   = %s \r", ESP_Config.ntpServerName.c_str() );
//  debugA("ESP_Config.AutoTurnOn  = %d \r", ESP_Config.AutoTurnOn );
//  debugA("ESP_Config.AutoTurnOff = %d \r", ESP_Config.AutoTurnOff );
//  debugA("ESP_Config.TurnOnHour  = %d \r", ESP_Config.TurnOnHour );
//  debugA("ESP_Config.TurnOnMin   = %d \r", ESP_Config.TurnOnMinute );
//  debugA("ESP_Config.TurnOffHour = %d \r", ESP_Config.TurnOffHour );
//  debugA("ESP_Config.TurnOffMin  = %d \r", ESP_Config.TurnOffMinute );

  debugA("ESP_Config.gps_debug  = %d \r", ESP_Config.gps_debug );
  debugA("ESP_Config.LoRa_debug  = %d \r", ESP_Config.LoRa_debug );
  debugA("ESP_Config.APRS_debug  = %d \r", ESP_Config.APRS_debug );
  debugA("ESP_Config.RebootTag  = %d \r", ESP_Config.RebootTag );

  debugA("ESP_Config.RTC_debug  = %d \r", ESP_Config.RTC_debug );
  debugA("ESP_Config.ezTime_debug  = %d \r", ESP_Config.ezTime_debug );
  debugA("ESP_Config.pps_debug  = %d \r", ESP_Config.pps_debug );
  debugA("ESP_Config.PE_debug  = %d \r", ESP_Config.PE_debug );
  debugA("ESP_Config.BT_KISS_Mode  = %d \r", ESP_Config.BT_KISS_Mode );
  debugA("ESP_Config.Serial_KISS_Mode  = %d \r", ESP_Config.Serial_KISS_Mode );
   debugA("ESP_Config.TCP_KISS_Mode  = %d \r", ESP_Config.TCP_KISS_Mode );
 
  debugA("ESP_Config.Tracker_Mode  = %d \r", ESP_Config.Tracker_Mode );
  debugA("ESP_Config.IoT_enable  = %d \r", ESP_Config.IoT_enable );
  
  debugA("ESP_Config.mqtt_enable   = %d \r", ESP_Config.mqtt_enable );
  debugA("ESP_Config.WebConfig_debug  = %d \r", ESP_Config.WebConfig_debug );
  debugA("ESP_Config.standalone       = %d \r", ESP_Config.standalone );
  debugA("ESP_Config.syslog_enable    = %d \r", ESP_Config.syslog_enable );
  
  debugA("ESP_Config.no_gps           = %d \r", ESP_Config.no_gps );
  debugA("ESP_Config.iGate_Mode       = %d \r", ESP_Config.iGate_Mode );
  debugA("ESP_Config.Beacon_Mode      = %d \r", ESP_Config.Beacon_Mode );
  
  debugA("ESP_Config.Admin_Mode       = %d \r", ESP_Config.Admin_Mode );
  debugA("ESP_Config.Reboot_Now       = %d \r", ESP_Config.Reboot_Now );

  debugA("ESP_Config.EnableRfToIs     = %d \r", ESP_Config.EnableRfToIs );
  debugA("ESP_Config.EnableIsToRf     = %d \r", ESP_Config.EnableIsToRf );
  debugA("ESP_Config.EnableRepeater   = %d \r", ESP_Config.EnableRepeater );

  debugA("ESP_Config.Loc_Lat          = %s \r", ESP_Config.Loc_Lat.c_str() );
  debugA("ESP_Config.Loc_Lon          = %s \r", ESP_Config.Loc_Lon.c_str() );

  debugA("ESP_Config.AprsHost         = %s \r", ESP_Config.AprsHost.c_str() );
  debugA("ESP_Config.AprsPort         = %d \r", ESP_Config.AprsPort );
  debugA("ESP_Config.AprsLogin        = %s \r", ESP_Config.AprsLogin.c_str() );
  debugA("ESP_Config.AprsPass         = %s \r", ESP_Config.AprsPass.c_str() );
  debugA("ESP_Config.AprsFilter       = %s \r", ESP_Config.AprsFilter.c_str() );
  debugA("ESP_Config.AprsRawBeaconIGATE = %s \r", ESP_Config.AprsRawBeaconIGATE.c_str() );
  debugA("ESP_Config.AprsRawBeaconTRACK = %s \r", ESP_Config.AprsRawBeaconTRACK.c_str() );
  debugA("ESP_Config.AprsRawBeaconPeriodSecondsIGATE  = %d \r", ESP_Config.AprsRawBeaconPeriodSecondsIGATE );
  debugA("ESP_Config.AprsRawBeaconPeriodSecondsTRACK  = %d \r", ESP_Config.AprsRawBeaconPeriodSecondsTRACK );
  debugA("ESP_Config.AprsLoggerAddr  = %s \r", ESP_Config.AprsLoggerAddr.c_str() );
  debugA("ESP_Config.AprsLoggerPort  = %d \r", ESP_Config.AprsLoggerPort );

  debugA("ESP_Config.IoT_Host  = %s \r", ESP_Config.IoT_Host.c_str() );
  debugA("ESP_Config.IoT_Port  = %d \r", ESP_Config.IoT_Port );
  debugA("ESP_Config.IoT_Login = %s \r", ESP_Config.IoT_Login.c_str() );
  debugA("ESP_Config.IoT_Key  = %s \r", ESP_Config.IoT_Key.c_str() );

  debugA("ESP_Config.IoT_Key  = %s \r", ESP_Config.feed_1.c_str() );
  debugA("ESP_Config.IoT_Key  = %s \r", ESP_Config.feed_2.c_str() );
  debugA("ESP_Config.IoT_Key  = %s \r", ESP_Config.feed_3.c_str() );
  debugA("ESP_Config.IoT_Key  = %s \r", ESP_Config.feed_4.c_str() );
  debugA("ESP_Config.IoT_Key  = %s \r", ESP_Config.feed_5.c_str() );
  debugA("ESP_Config.IoT_Key  = %s \r", ESP_Config.feed_6.c_str() );  
  debugA("ESP_Config.LoraFreq  = %s \r", ESP_Config.LoraFreq );
  debugA("ESP_Config.LoraBw  = %s \r", ESP_Config.LoraBw );
  debugA("ESP_Config.LoraSf  = %d \r", ESP_Config.LoraSf );
  debugA("ESP_Config.LoraCodingRate  = %d \r", ESP_Config.LoraCodingRate );
  debugA("ESP_Config.LoraSync  = %d \r", ESP_Config.LoraSync );
  debugA("ESP_Config.LoraPower  = %d \r", ESP_Config.LoraPower );
  debugA("ESP_Config.LoraFreqCorr  = %d \r", ESP_Config.LoraFreqCorr );
  debugA("ESP_Config.LoraPreambleLen  = %d \r", ESP_Config.LoraPreambleLen );
  debugA("ESP_Config.EncapType  = %d \r", ESP_Config.EncapType );
  debugA("ESP_Config.wifi_enable  = %d \r", ESP_Config.wifi_enable );

// Beacon app parameters
  debugA("ESP_Config.BeaconId  = %d \r", ESP_Config.BeaconId );
  debugA("ESP_Config.BeaconSeqNbr  = %d \r", ESP_Config.BeaconSeqNbr );
  debugA("ESP_Config.BeaconLocation  = %d \r", ESP_Config.BeaconLocation );
  debugA("ESP_Config.BeaconEngineType  = %d \r", ESP_Config.BeaconEngineType );
  debugA("ESP_Config.BeaconRun  = %d \r", ESP_Config.BeaconRun );
  debugA("ESP_Config.BCN_TimeSlotOffset  = %d \r", ESP_Config.BCN_TimeSlotOffset );
  debugA("ESP_Config.BCN_TimeSlotSync  = %d \r", ESP_Config.BCN_TimeSlotSync );
  debugA("ESP_Config.BeaconStatsCollector_IP   = %d.%d.%d.%d \r", ESP_Config.BeaconStatsCollector_IP[0],ESP_Config.BeaconStatsCollector_IP[1],ESP_Config.BeaconStatsCollector_IP[2],ESP_Config.BeaconStatsCollector_IP[3]);
  debugA("ESP_Config.BeaconStatsCollector_Port  = %d \r", ESP_Config.BeaconStatsCollector_Port );
  debugA("ESP_Config.BeaconQRALocator  = %s \r", ESP_Config.BeaconQRALocator.c_str() );
  debugA("ESP_Config.BCN_LoRa_Vector  = %s \r", ESP_Config.BCN_LoRa_Vector.c_str() );
  debugA("ESP_Config.BeaconUnixTime  = %d \r", ESP_Config.BeaconUnixTime );  
  debugA("ESP_Config.BeaconFreq  = %dd \r", ESP_Config.BeaconFreq );  
  debugA("ESP_Config.BeaconPower = %d \r", ESP_Config.BeaconPower );  
  debugA("ESP_Config.BeaconWorkConditions  = %d \r", ESP_Config.BeaconWorkConditions );  
  debugA("End of Configuration data\r");
  debugA("\r=========>DHCP acquired data<==========");
  debugA("%s\r" ,String("localIP:    " + (String) WiFi.localIP()[0] + "." +  (String) WiFi.localIP()[1] + "." +  (String) WiFi.localIP()[2] + "." + (String) WiFi.localIP()[3]).c_str());
  debugA("%s\r" ,String("subnetMask: " + (String) WiFi.subnetMask()[0] + "." +  (String) WiFi.subnetMask()[1] + "." +  (String) WiFi.subnetMask()[2] + "." + (String) WiFi.subnetMask()[3]).c_str());
  debugA("%s\r" ,String("gatewayIP:  " + (String) WiFi.gatewayIP()[0] + "." +  (String) WiFi.gatewayIP()[1] + "." +  (String) WiFi.gatewayIP()[2] + "." + (String) WiFi.gatewayIP()[3]).c_str());
//  debugA("dnsIP:      " + (String) WiFi.dnsIP()[0] + "." +  (String) WiFi.dnsIP()[1] + "." +  (String) WiFi.dnsIP()[2] + "." + (String) WiFi.dnsIP()[3]);
//  debugA("macAddress: " + (String) WiFi.macAddress()[0] + ":" +  (String) WiFi.macAddress()[1] + ":" +  (String) WiFi.macAddress()[2] + ":" + (String) WiFi.macAddress()[3] + ":" +  (String) WiFi.macAddress()[4] + ":" +  (String) WiFi.macAddress()[5] );

  if (WiFi.status() == WL_CONNECTED){
     uint8_t macAddr[6];
     WiFi.macAddress(macAddr);
     debugA("macAddress: %02x:%02x:%02x:%02x:%02x:%02x\n", macAddr[0], macAddr[1], macAddr[2], macAddr[3], macAddr[4], macAddr[5]);
     };
  debugA("\r======>WiFi connection status<=========\r");
  debugA("WiFi status: %d  ",WiFi.status()); debugA("( 3=WL_CONNECTED 6=WL_DISCONNECTED )");
  debugA("SSID:        %s\r", WiFi.SSID().c_str());
  debugA("BSSID:       %s\r", WiFi.BSSIDstr().c_str());
  debugA("RSSI:        %d dBm\r", WiFi.RSSI());

  debugA("\r======>Local Config Dependent data status<=========");
  debugA("standalone:       %d \r",standalone); 
  debugA("syslog_enable:    %d \r",syslog_enable); 

  debugA("no_gps:           %d \r",no_gps); 
  debugA("gps_debug:        %d \r",gps_debug); 
  debugA("RTC_debug:        %d \r",RTC_debug); 
  debugA("LoRa_debug:       %d \r",LoRa_debug);
  debugA("APRS_debug:       %d \r",APRS_debug);
  debugA("RebootTag:       %d \r",RebootTag);

  debugA("ezTime_debug:     %d \r",ezTime_debug); 
  debugA("mqtt_enable:   %d \r",mqtt_enable); 
  debugA("WebConfig_debug:  %d \r",WebConfig_debug); 
  debugA("PE_debug:         %d \r",PE_debug); 
  debugA("pps_debug:        %d \r",pps_debug); 
  debugA("BT_KISS_Mode:     %d \r",BT_KISS_Mode); 
  debugA("Serial_KISS_Mode: %d \r",Serial_KISS_Mode); 
  debugA("TCP_KISS_Mode: %d \r",TCP_KISS_Mode); 
  
  debugA("Tracker_Mode:     %d \r",Tracker_Mode); 
  debugA("IoT_enable:       %d \r",IoT_enable); 
  
  debugA("iGate_Mode:      %d \r",iGate_Mode); 
  debugA("Beacon_Mode:      %d \r",Beacon_Mode); 

  debugA("EnableRfToIs:      %d \r",EnableRfToIs); 
  debugA("EnableIsToRf:      %d \r",EnableIsToRf); 
  debugA("EnableRepeater:      %d \r",EnableRepeater); 

  debugA("Admin_Mode:      %d \r",Admin_Mode); 
  debugA("Reboot_Now:      %d \r",Reboot_Now); 

  debugA("\rEnd of status data\r");
//***********************************

  debugA("Display Inventory data\r");
  debugA("ESP_Config.DeviceName  = %s \r", ESP_Config.DeviceName.c_str() );
  debugA("ESP_Config.DeviceId  = %s \r", ESP_Config.DeviceId.c_str() );
  debugA("ESP_Config.cpu_type  = %s \r", ESP_Config.cpu_type.c_str() );
  
  debugA("ESP_Config.dhcp        = %d \r", ESP_Config.dhcp );

  return true;
  
}

/*
**
**  NTP 
**
*/
void NTPRefresh() {
  if (WiFi.status() == WL_CONNECTED){
    IPAddress timeServerIP; 
 //////////   WiFi.hostByName(ESP_Config.ntpServerName.c_str(), timeServerIP); 
    //sendNTPpacket(timeServerIP); // send an NTP packet to a time server

    debugA("NTPRefresh: sending NTP packet...");
    memset(packetBuffer, 0, NTP_PACKET_SIZE);
    packetBuffer[0] = 0b11100011;   // LI, Version, Mode
    packetBuffer[1] = 0;     // Stratum, or type of clock
    packetBuffer[2] = 6;     // Polling Interval
    packetBuffer[3] = 0xEC;  // Peer Clock Precision
    packetBuffer[12]  = 49;
    packetBuffer[13]  = 0x4E;
    packetBuffer[14]  = 49;
    packetBuffer[15]  = 52;
    UDPNTPClient.beginPacket(timeServerIP, 123); 
    UDPNTPClient.write(packetBuffer, NTP_PACKET_SIZE);
    UDPNTPClient.endPacket();

    vTaskDelay(pdMS_TO_TICKS(1000));
  
    int cb = UDPNTPClient.parsePacket();
    if (!cb) {
      debugA("NTP no packet yet");
    }
    else 
    {
      debugA("NTP packet received, length=%d\r", cb);
      UDPNTPClient.read(packetBuffer, NTP_PACKET_SIZE); // read the packet into the buffer
      unsigned long highWord = word(packetBuffer[40], packetBuffer[41]);
      unsigned long lowWord = word(packetBuffer[42], packetBuffer[43]);
      unsigned long secsSince1900 = highWord << 16 | lowWord;
      const unsigned long seventyYears = 2208988800UL;
      unsigned long epoch = secsSince1900 - seventyYears;
      UnixTimestamp = epoch;
    }
  }
}


void loop_global(){
/*  
  if (AdminEnabled) {
    if (AdminTimeOutCounter > AdminTimeOut){
       AdminEnabled = false;
       debugA("Admin Mode disabled! AdminTimeOutCounter=%d - AdminTimeOut=%d\r", AdminTimeOutCounter, AdminTimeOut);
     //  WiFi.mode(WIFI_STA);
       };
    };
 */
  if (ESP_Config.Update_Time_Via_NTP_Every  > 0 ) {
    if (cNTP_Update > 5 && firstStart){
      NTPRefresh();
      cNTP_Update =0;
      firstStart = false;
      }
    else if ( cNTP_Update > (ESP_Config.Update_Time_Via_NTP_Every * 60) ) {
      NTPRefresh();
      cNTP_Update =0;
      };
    };

  if(DateTime.minute != Minute_Old) {
     Minute_Old = DateTime.minute;
     if (ESP_Config.AutoTurnOn) {
       if (DateTime.hour == ESP_Config.TurnOnHour && DateTime.minute == ESP_Config.TurnOnMinute) {
          debugA("SwitchON");
          };
       };

     Minute_Old = DateTime.minute;
     if (ESP_Config.AutoTurnOff) {
       if (DateTime.hour == ESP_Config.TurnOffHour && DateTime.minute == ESP_Config.TurnOffMinute) {
          debugA("SwitchOff");
          };
       };
     };
 
  web_server.handleClient();
}

// device configuration strategy:  if GET_CONFIG_FROM_FRAM is defined the config will be assumed to be written in FRAM; 
// otherwise it will be assumed to be in ITTLEFS
// if a valid config is not retrived from the above a default configuration will be applied and stored in the above stoage
// if the force_defaults prametr is true the default config will be applied and written to storage

bool LoadConfig( bool force_defaults ){
  // load or initialize device configuration from FRAM or LittleFS
  Serial.printf("LoadConfig:  Start searching device configuration from FRAM or Flash...\r\n");

  bool needs_loading_defaults = false;
  if ( force_defaults) {
    Serial.printf("LoadConfig: found request for forced loading from FRAM or Flash...\r\n");
    needs_loading_defaults = true ;
    }
  else if (!ReadConfig()) {
     needs_loading_defaults = true ;
     };
   if(needs_loading_defaults){  
        // DEFAULT CONFIG ESP_Config structure to be loaded in case no config available from FRAM or LittleFS
        ESP_Config.ssid = "wifi_SSID_tbd";
        ESP_Config.password = "wifi_pass_tbd";
        ESP_Config.dhcp = true;
        ESP_Config.IP[0] = 192;ESP_Config.IP[1] = 168;ESP_Config.IP[2] = 2;ESP_Config.IP[3] = 60;
        ESP_Config.Netmask[0] = 255;ESP_Config.Netmask[1] = 255;ESP_Config.Netmask[2] = 255;ESP_Config.Netmask[3] = 0;
        ESP_Config.Gateway[0] = 192;ESP_Config.Gateway[1] = 168;ESP_Config.Gateway[2] = 2;ESP_Config.Gateway[3] = 1;
        ESP_Config.Dns[0] = 8;ESP_Config.Dns[1] = 8;ESP_Config.Dns[2] = 8;ESP_Config.Dns[3] = 8;
        ESP_Config.ntpServerName = "ntp1.inrim.it";
        ESP_Config.Update_Time_Via_NTP_Every =  0;
        ESP_Config.timezone = +1;
        ESP_Config.daylight = true;
        ESP_Config.DeviceName = "Not Available tbd";
        ESP_Config.DeviceId = "Not Available";
        ESP_Config.cpu_type = CPU_TYPE ;
//      ESP_Config.SW_Revision = SW_Revision ; 
        ESP_Config.AutoTurnOff = false;
        ESP_Config.AutoTurnOn = false;
        ESP_Config.TurnOffHour = 0;
        ESP_Config.TurnOffMinute = 0;
        ESP_Config.TurnOnHour = 0;
        ESP_Config.TurnOnMinute = 0;
        ESP_Config.gps_debug = 0;
        ESP_Config.LoRa_debug = 0;
        ESP_Config.RebootTag = 0;

        
        ESP_Config.RTC_debug = 0;
        ESP_Config.ezTime_debug = 0;
        ESP_Config.pps_debug = 0;
        ESP_Config.PE_debug = 0;
        ESP_Config.BT_KISS_Mode = false;
        ESP_Config.Serial_KISS_Mode = false; 
        ESP_Config.TCP_KISS_Mode = false; 
        
        ESP_Config.Tracker_Mode = true; 
        ESP_Config.IoT_enable = false; 
        ESP_Config.BeaconId = 0;
        ESP_Config.BeaconSeqNbr = 0;
        ESP_Config.BeaconLocation = 0;
        ESP_Config.BeaconEngineType = 0 ;
        ESP_Config.BeaconRun = 0;
        ESP_Config.BCN_TimeSlotOffset = 0;
        ESP_Config.BCN_TimeSlotSync = 0 ;
        ESP_Config.BeaconStatsCollector_IP[0] = 192;ESP_Config.BeaconStatsCollector_IP[1] = 168;ESP_Config.BeaconStatsCollector_IP[2] = 2;ESP_Config.BeaconStatsCollector_IP[3] = 150;
        ESP_Config.BeaconStatsCollector_Port = 33330 ;
        ESP_Config.BeaconQRALocator = "QRALocator";
        ESP_Config.BCN_LoRa_Vector = "BCN_LoRa_Vector";
        ESP_Config.BeaconUnixTime = 0;
        ESP_Config.BeaconFreq = 0;
        ESP_Config.BeaconPower = 0;
        ESP_Config.BeaconWorkConditions = 0;
        ESP_Config.mqtt_enable = false ;
        ESP_Config.WebConfig_debug = 0;
        ESP_Config.syslog_enable = 0;
        
#ifdef GPS_MOD        
        ESP_Config.no_gps = 0;
#else
        ESP_Config.no_gps = 1;
#endif
        ESP_Config.iGate_Mode = 0;
        ESP_Config.Beacon_Mode = 0;
        ESP_Config.Admin_Mode = 0;         // 0 to startup in normal operation  1 to startup in admin mode
        ESP_Config.Reboot_Now = 0;
        ESP_Config.Loc_Lat = "4000.00N" ;
        ESP_Config.Loc_Lon = "01300.00E" ;
        ESP_Config.AprsHost = "rotate.aprs2.net" ;
        ESP_Config.AprsPort = 14580;        
        ESP_Config.AprsLogin = "I1XYZ-10" ;     // should be aprs-is_userid_tbd
        ESP_Config.AprsPass = "aprs-is_pass_tbd";    // should be aprs-is_userid_tbd   
        ESP_Config.AprsFilter = "m/1" ;
        ESP_Config.AprsRawBeaconIGATE = "I1XYZ-10,WIDE1-1,LoRa,&,L";
        ESP_Config.AprsRawBeaconTRACK = "I1XYZ-10,WIDE1-1,LoRa,&,L" ;
        ESP_Config.AprsRawBeaconPeriodSecondsIGATE = 1800;
        ESP_Config.AprsRawBeaconPeriodSecondsTRACK = 300;
        ESP_Config.AprsLoggerAddr = "192.168.2.150" ;
        ESP_Config.AprsLoggerPort = 44444;
        ESP_Config.IoT_Host = "io.adafruit.com" ;
        ESP_Config.IoT_Port = 1883;
        ESP_Config.IoT_Login = "IoT_TBD" ;
        ESP_Config.IoT_Key =  "IoT_TBD";
        ESP_Config.feed_1 = "para_1";
        ESP_Config.feed_2 = "para_2";
        ESP_Config.feed_3 = "para_3";
        ESP_Config.feed_4 = "para_4";
        ESP_Config.feed_5 = "para_5";
        ESP_Config.feed_6 = "para_6";
    
        ESP_Config.LoraFreq = "433.775";
        ESP_Config.LoraBw = "125";
        ESP_Config.LoraSf = 12;
        ESP_Config.LoraCodingRate = 8;
        ESP_Config.LoraSync = 0x12 ;  //  0x34=APRS 0x12=OE-Style
        ESP_Config.LoraPower = 15;
        ESP_Config.LoraFreqCorr = 0 ;
        ESP_Config.LoraPreambleLen = 10  ;
        ESP_Config.EncapType = 2  ;        // 0=native 1= APRS  2= OE-Style
        ESP_Config.PayloadStyle = 0  ;        // 0=Sarimesh LoRa  1= Pure OE_Style  2= Modified OE_Style
        ESP_Config.RepeaterOperation = 1  ;        // 0=Sarimesh(enabled)  1= OE_Style(disabled)
        ESP_Config.LocationCompression = 1 ;  // 0= disabled  1=enabled
        ESP_Config.BlackList = 0 ;  // 0= disabled  1=enabled

        ESP_Config.AgileBeaconing = 0 ;       // 0=disable != 0 enabled
        ESP_Config.BCN_LoRa_Vector = "0, 433.800, 10.4, 12, 8, 0x34, 22, -2, 10";
        ESP_Config.BCN_TimeSlotSync = 4  ;        // 
        ESP_Config.ps_debug = false  ; 
        ESP_Config.wifi_enable = false  ; 
        ESP_Config.standalone = 1;

    WriteConfig();   // save configuration in non volatile memory ( FRAM or LittleFS)
    ///////printFile("/WebConf.conf");
    Serial.printf("Default config applied\r\n");
    return false;
    }
  else{
    Serial.printf("ESP_Config Successfully loaded\r\n");
    return true;    
    }

}

extern const char PAGE_Information[] PROGMEM ;
extern const char PAGE_Style_css[] PROGMEM ;
extern const char PAGE_Statistics[] PROGMEM ;
extern const char PAGE_Dashboard[] PROGMEM ;
extern const char PAGE_Dashboard_kiss[] PROGMEM ;

extern const char PAGE_AdminMainPage_kiss[] PROGMEM ;
extern const char PAGE_AdminMainPage_core[] PROGMEM ;
extern const char PAGE_AdminMainPage_aprs[] PROGMEM ;
extern const char PAGE_AprsConfiguration[] PROGMEM ;
extern const char PAGE_AprsWaitAndReload[] PROGMEM ;
extern const char PAGE_BeaconConfiguration[] PROGMEM ;
extern const char PAGE_BeaconWaitAndReload[] PROGMEM ;
extern const char PAGE_Root[] PROGMEM ;

extern const char PAGE_Dashboard_kiss[] PROGMEM ;
extern const char PAGE_Dashboard_kiss[] PROGMEM ;
extern const char PAGE_Dashboard_kiss[] PROGMEM ;
extern const char PAGE_Dashboard_kiss[] PROGMEM ;
extern const char PAGE_Dashboard_kiss[] PROGMEM ;
extern const char PAGE_Dashboard_kiss[] PROGMEM ;
extern const char PAGE_Dashboard_kiss[] PROGMEM ;
extern const char PAGE_Dashboard_kiss[] PROGMEM ;
extern const char PAGE_Dashboard_kiss[] PROGMEM ;


bool WebServer_Init( void ){
//******************************  management webserver startup **************************
    debugA( "Setup HTTP Management server... \r" );

    web_server.on ( "/", processHome  );
    web_server.on ( "/files.html", FilesHomePage);
    web_server.on ( "/download", File_Download);
    web_server.on ( "/upload",   File_Upload);
    web_server.on ( "/clear",   File_Clear);    
    web_server.on ( "/fupload",  HTTP_POST,[](){ web_server.send(200);}, handleFileUpload);
    web_server.on ( "/admin/filldynamicdata", filldynamicdata );
    web_server.on ( "/favicon.ico",   []() { debugA("favicon.ico"); web_server.send ( 200, "text/html", "" );   }  );
    web_server.on ( "/config.html", send_network_configuration_html );
    web_server.on ( "/debug.html", send_debug_configuration_html );
    web_server.on ( "/inventory.html", send_inventory_configuration_html );
    web_server.on ( "/iot.html", send_iot_configuration_html );
    web_server.on ( "/hwadj.html", send_hwadj_configuration_html );
    web_server.on ( "/info.html", []() { debugA("info.html"); web_server.send ( 200, "text/html", PAGE_Information );   }  );
    web_server.on ( "/ntp.html", send_NTP_configuration_html  );
    web_server.on ( "/general.html", send_general_html  );
    web_server.on ( "/index.html", send_home_html );
    web_server.on ( "/style.css", []() { debugA("style.css"); web_server.send ( 200, "text/plain", PAGE_Style_css );  } );
    web_server.on ( "/microajax.js", []() { debugA("microajax.js"); web_server.send ( 200, "text/plain", PAGE_microajax_js );  } );
    web_server.on ( "/admin/values", send_network_configuration_values_html );
    web_server.on ( "/admin/debugvalues", send_debug_configuration_values_html );
    web_server.on ( "/admin/inventoryvalues", send_inventory_configuration_values_html );
    web_server.on ( "/admin/hwadjvalues", send_hwadj_configuration_values_html );
    web_server.on ( "/admin/IoTvalues", send_iot_configuration_values_html );
    web_server.on ( "/admin/connectionstate", send_connection_state_values_html );
    web_server.on ( "/admin/infovalues", send_information_values_html );
    web_server.on ( "/admin/ntpvalues", send_NTP_configuration_values_html );
    web_server.on ( "/admin/generalvalues", send_general_configuration_values_html);
    web_server.on ( "/admin/examplevalues", send_example_configuration_values_html);
    web_server.on ( "/admin/devicename",     send_devicename_value_html);
    web_server.onNotFound ( []() { debugA("Page Not Found\r"); web_server.send ( 400, "text/html", "Page not Found" );   }  );

 
    if(!BT_KISS_Mode && !Serial_KISS_Mode && !TCP_KISS_Mode){  // this is for APRS
       web_server.on ( "/admin.html", []() { debugA("admin.html"); web_server.send ( 200, "text/html", PAGE_AdminMainPage_aprs);   }  );
       }
    else{   // this is for KISS
       web_server.on ( "/admin.html", []() { debugA("admin.html"); web_server.send ( 200, "text/html", PAGE_AdminMainPage_kiss);   }  );
       };
    web_server.on ( "/stats.html", []() { debugA("stats.html"); web_server.send ( 200, "text/html", PAGE_Statistics );   }  );
    web_server.on ( "/admin/statsvalues", send_stats_values_html ); 

    if(!BT_KISS_Mode && !Serial_KISS_Mode && !TCP_KISS_Mode){   // this is for APRS
       web_server.on ( "/dashboard.html", []() { debugA("dashboard.html"); web_server.send ( 200, "text/html", PAGE_Dashboard );   }  );
       web_server.on ( "/encap.html", send_encap_configuration_html );    
       }
    else{ // this is for KISS
       web_server.on ( "/dashboard.html", []() { debugA("dashboard.html"); web_server.send ( 200, "text/html", PAGE_Dashboard_kiss );   }  );
       web_server.on ( "/encap.html", send_encap_configuration_html_kiss );    
       }; 

    web_server.on ( "/beacon.html", send_beacon_configuration_html );
    web_server.on ( "/lora.html", send_lora_configuration_html );
    web_server.on ( "/aprs.html", send_aprs_configuration_html );
    
    web_server.on ( "/admin/Aprsvalues", send_aprs_configuration_values_html );
    web_server.on ( "/admin/Encapvalues", send_encap_configuration_values_html );
    web_server.on ( "/admin/Beaconvalues", send_beacon_configuration_values_html );
    web_server.on ( "/admin/LoRavalues", send_lora_configuration_values_html );
    web_server.on ( "/admin/dashboardvalues", send_dashboard_values_html ); 


    web_server.begin();
    debugA( " started\r" );

    red_flash();   // flash red led attached to bit 8 of port expander
    return true ;
}


void write_spot_to_log(String my_record){   // write RX spot to FRAM log
     char Msg[512]; 
     time_t ora = now();
     sprintf(Msg,"%d",ora);
     String spot_time = Msg ; 
     my_record= String(spot_time) + "|"  + String(my_record);
     WriteRecord(&my_record);
}

void write_event_to_log(String my_record){   // write event to FRAM log
     char Msg[512]; 
     time_t ora = now();
     sprintf(Msg,"%d",ora);
     String spot_time = Msg ; 
     my_record= String(spot_time) + "|"  + "EVENT" + "|" + String(my_record);
     WriteRecord(&my_record);
}

void Second_Tick(void)
{
//  debugA("Second_Tick......");
//  red_flash_short(); 
//  strDateTime tempDateTime;
  AdminTimeOutCounter++;
  if(! LoRa_initialized ) { display_event = 1;}
  return ;
}
 
