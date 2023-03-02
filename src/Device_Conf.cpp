// Device_Conf.cpp     // programmatically define main HW features:  I2C bus, SPI bus, etc.
// Master configuration directives: need to be customized depending on HW/SW configuratio of the device
#include "Master_Config.h"
#include "hw_def.h"

bool HW_Set_SARIMESH(void){
    debugA("Running HW_Set_SARIMESH....\r"); 
    hw_arch = 1 ;    // HW SARIMESH
    i2c_sda = O_SDA;
    i2c_scl = O_SCL;
    spi_sck = SCK ;
    spi_miso = MISO ;
    spi_mosi = MOSI ;
    red_led = RED_LED;
    green_led = GREEN_LED;
    fp_button = BTN_PIN;
    oled_addr = O_ADDR;
#ifdef O_RST     
    oled_rst = O_RST;
#else
    oled_rst = 0;
#endif    
    oled_orient = O_FLIPSCREEN ;
    
    lora_cs = L_SS;
    lora_rst = L_RST;
    lora_dio = L_DI0;
    lora_busy = L_BUSY;

    gps_rx = GPS_RX;
    gps_tx = GPS_TX;


    reboot_pin = REBOOT;  // 0 to disable
    buzzer_pin = BUZZER;  // 0 to disable 

    powerchip = false ;
    debugA("Done\r"); 
    return (true);
}

void show_HW_Device_Config(void){
    debugA("show_HW_Device_Config \r",hw_arch); 

    debugA("hw_arch:       %d \r",hw_arch); 
    debugA("i2c_sda:       %d \r",i2c_sda); 
    debugA("i2c_scl:       %d \r",i2c_scl); 
    
    debugA("spi_sck:       %d \r",spi_sck); 
    debugA("spi_miso:      %d \r",spi_miso); 
    debugA("spi_mosi:      %d \r",spi_mosi);
    
    debugA("red_led:       %d \r",red_led); 
    debugA("green_led:     %d \r",green_led); 
    debugA("fp_button:     %d \r",fp_button); 
    
    debugA("oled_addr:     %d \r",oled_addr); 
    debugA("oled_rst:      %d \r",oled_rst); 
    debugA("oled_orient:   %d \r",oled_orient); 
    
   /*
    debugA("LoraDeviceType:   %d \r\n",LoraDeviceType); 
    debugA("lora_cs:       %d \r\n",lora_cs); 
    debugA("lora_rst:      %d \r\n",lora_rst); 
    debugA("lora_dio:      %d \r\n",lora_dio); 
    debugA("lora_busy:     %d \r\n",lora_busy); 

    debugA("gps_rx:        %d \r\n",gps_rx); 
    debugA("gps_tx:        %d \r\n",gps_tx); 

    debugA("reboot_pin:    %d \r\n",reboot_pin); 
    debugA("buzzer_pin:    %d \r\n",buzzer_pin); 
    
    debugA("powerchip:     %d \r\n",powerchip); 
   */ 
    debugA("\r"); 
    
    return;  
}

void show_HW_Device_Config_Serial(void){
    Serial.printf("show_HW_Device_Config \r\n",hw_arch); 

    Serial.printf("hw_arch:       %d \r\n",hw_arch); 
    Serial.printf("i2c_sda:       %d \r\n",i2c_sda); 
    Serial.printf("i2c_scl:       %d \r\n",i2c_scl); 
    
    Serial.printf("spi_sck:       %d \r\n",spi_sck); 
    Serial.printf("spi_miso:      %d \r\n",spi_miso); 
    Serial.printf("spi_mosi:      %d \r\n",spi_mosi);
    
    Serial.printf("red_led:       %d \r\n",red_led); 
    Serial.printf("green_led:     %d \r\n",green_led); 
    Serial.printf("fp_button:     %d \r\n",fp_button); 
    
    Serial.printf("oled_addr:     %d \r\n",oled_addr); 
    Serial.printf("oled_rst:      %d \r\n",oled_rst); 
    Serial.printf("oled_orient:   %d \r\n",oled_orient); 
    
    Serial.printf("LoraDeviceType:   %d \r\n",LoraDeviceType); 
    Serial.printf("lora_cs:       %d \r\n",lora_cs); 
    Serial.printf("lora_rst:      %d \r\n",lora_rst); 
    Serial.printf("lora_dio:      %d \r\n",lora_dio); 
    Serial.printf("lora_busy:     %d \r\n",lora_busy); 

    Serial.printf("gps_rx:        %d \r\n",gps_rx); 
    Serial.printf("gps_tx:        %d \r\n",gps_tx); 

    Serial.printf("reboot_pin:    %d \r\n",reboot_pin); 
    Serial.printf("buzzer_pin:    %d \r\n",buzzer_pin); 
    
    Serial.printf("powerchip:     %d \r\n",powerchip); 
    
    Serial.printf("\r\n"); 
    
    return;  
}


/*
BoardConfig::BoardConfig(String name, BoardType type, uint8_t oledsda, uint8_t oledscl, uint8_t oledaddr, uint8_t oledreset, uint8_t lorasck, uint8_t loramiso, uint8_t loramosi, uint8_t loracs, uint8_t lorareset, uint8_t lorairq, bool needcheckpowerchip, bool powercheckstatus)
                                                                                      [      OLED     ] [         LoRa        ]                                       
BoardConfig TTGO_LORA32_V1        ("TTGO_LORA32_V1",         eTTGO_LORA32_V1,          4, 15, 0x3C,  0,  5, 19, 27, 18, 14, 26);
BoardConfig TTGO_LORA32_V2        ("TTGO_LORA32_V2",         eTTGO_LORA32_V2,         21, 22, 0x3C,  0,  5, 19, 27, 18, 14, 26, true);
BoardConfig TTGO_T_Beam_V0_7      ("TTGO_T_Beam_V0_7",       eTTGO_T_Beam_V0_7,       21, 22, 0x3C,  0,  5, 19, 27, 18, 14, 26, true);
BoardConfig TTGO_T_Beam_V1_0      ("TTGO_T_Beam_V1_0",       eTTGO_T_Beam_V1_0,       21, 22, 0x3C,  0,  5, 19, 27, 18, 14, 26, true, true);

BoardConfig ETH_BOARD             ("ETH_BOARD",              eETH_BOARD,              33, 32, 0x3C,  0, 14,  2, 15, 12,  4, 36);
BoardConfig TRACKERD              ("TRACKERD",               eTRACKERD,                5,  4, 0x3C,  0, 18, 19, 23, 16, 14, 26);

BoardConfig HELTEC_WIFI_LORA_32_V1("HELTEC_WIFI_LORA_32_V1", eHELTEC_WIFI_LORA_32_V1,  4, 15, 0x3C, 16,  5, 19, 27, 18, 14, 26);
BoardConfig HELTEC_WIFI_LORA_32_V2("HELTEC_WIFI_LORA_32_V2", eHELTEC_WIFI_LORA_32_V2,  4, 15, 0x3C, 16,  5, 19, 27, 18, 14, 26);
*/
