#ifndef MASTER_CONFIG_H
#define MASTER_CONFIG_H
//ESP32 LoRa Beacon by SARIMESH -  versione HW Sarimesh -  chipset 2nd generazione
// MINITRACKER Vr4.1 customizzazione 20230127_1504
// proc ESP32 Dev Module on COM5
// PCB Vr4.1 PCS 6 / Vr. 3 pcs 4
// SW Revision id
#define SW_REVISION_ID "4.1.0_20230515"
//#define CPU_TYPE "ESP32 Dev V4"  // name of microntroller cpu module
#define CPU_TYPE "TTGO"  // name of microntroller cpu module
//#define CPU_TYPE "HELTEC"  // name of microntroller cpu module


// Master configuration directives: need to be customized depending on HW/SW configuration of the device
#define WIFI_UPSTREAM               // defines main upstream network:  WiFi / Ethernet upstream (in undefined)
#define DUTY_CYCLE_MONITOR_ENABLE   // compile and enable DUTY_CYCLE_MONITOR service
//#define TEMPERATURE_MONITOR_ENABLE   // compile and enable TEMPERATURE_MONITOR service
//#define AIO_MQTT_ENABLE             // compile and enable AIO_MQTT services

#define HMQ_MQTT_ENABLE             // compile and enable HMQ_MQTT services
//#define HMQ_MQTT_VPN_ENABLE         // enable HMQ_MQTT_VPN // requires HMQ_MQTT_ENABLE 
//#define WEBCLIENT_TEST_ENABLE     // enable WEBCLIENT_TEST
//#define APRS-IS_DISABLE             // disables APRS-IS interface
//#define TX_DISABLE                  // completely disables TX functionality: safety for RX only operation 
//#define SPLIT_MODE                  // enable RX/TX spli operation using completely different RX and TX LoRa vectors

//#define DEBUG_DISABLED            // to completely disable serial and remote debugging: no overhead for debug
//#define FORCE_SERIAL_DEBUG        // force using SerialDebug  only / if ndef RemoteDegug will be used

//#define CORE_ONLY_FUNCTIONS_ENABLE
#define LORA_FUNCTIONS_ENABLE    // enables all LoRa related functionalities for core system  evaluation
//#define PS_FUNCTIONS_ENABLE    // enables all PowerSpply related functionalities for core system  evaluation

#define DEBUG_DISABLE_DEBUGGER true // debugger not used in this project
//#define DEBUG_USE_TAG true        // just do not change it !
//#define WEBSOCKET_DISABLED true   // disable remote console debug support; just leave it enabled
#define DEBUG_INITIAL_LEVEL DEBUG_LEVEL_VERBOSE

#define EZTIME_GPS_RTC              // allows to integrate GPS support into eZtime library operation; must also be updated in ezTime.cpp 
#define USE_ADAFRUIT_TFT_LIB        // selects the TFT library to use ( adafruit/eSPI ) default is eSPI lib
#define USE_SSD1306_ASCII_LIB     // select SSD1306 library to use (ASCII_LIB / standard lib) default is standard lib
#define BLUETOOTH_DISABLED          // to completely remove any bluetooth support
#define USE_ARDUINO_OTA             // enable Over The Air SW loading
#define ACCESS_POINT_PASSWORD  "esp32" 
#define AdminTimeOut 6000             // original 180 secs Defines the Time in Seconds, when the Admin-Mode will be disabled
//#define SAVE_PROTECT                // to disallow saving configuration 
//#define FORCE_DEFAULTS              // to force default configuration
// #define GET_CONFIG_FROM_FRAM       // if defined the device configuration will be retrived from FRAM; otherwise from LittleFS 

#define NET_UPSTREAM_MON_ENABLE       // enables NetworkUpstreamMonitor

//******************************* define HW device customization ********************************
//#define HW_TYPE_SARIMESH
//#define LORA_MODULE_TYPE_SX1268
#define LORA_MODULE_TYPE_SX1278
//#define LORA_DEVICE "E22_400M30S" // i.e.  E22_400M30S, E22_400M22S, RFM98 , RA_01S ,
//#define LORA_DEVICE "RA_01S" 
#define LORA_DEVICE "RFM98" 
//#define force_CAD_disable     // force to completely disable CAD for fora transmission

// HW modules equipped
#define LORA_MOD         // define if a LoRa device is available
#define GPS_MOD           // define if a serial GPS is available
//#define W5500_MOD        // define if an W5500 ethernet module is available
//#define W5500_USE_DHCP
#define OLED_MOD         // define if an OLED display is available
//#define TFT_MOD          // define if a TFT display is available
//#define SD_MOD           // define if a SD card reader is available on a ST7735 TFT/SD card module
//#define PCF8574_MOD      // PCF8574 port rxpander equipped
//#define INA219_MOD       // INA210 current probe equipped  over I2C
//#define MUX4051_MOD       // analog mux  equipped seect by IO
//#define PWRS_MOD         // Power Supply modules


// https://github.com/Xinyuan-LilyGO/LilyGo-LoRa-Series/blob/master/examples/RadioLibExamples/SX1278/SX1278_Receive_Interrupt/utilities.h
// LED/button/buzzer pin indicators
#define RED_LED 14
#define GREEN_LED 14
#define REBOOT 0
#define BTN_PIN  39 //  39  // via analog_read 
#define BUZZER 0


// I2C bus pins
#define O_ADDR 0x3c
#define O_SDA 21
#define O_SCL 22
#define O_RST 0
#define O_FLIPSCREEN 1

// LoRa specific pins
// SPI bus pins;defaults defined in C:\Users\root\Documents\Arduino\hardware\espressif\esp32\variants\ttgo-lora32-v1/pins_arduino.h:
#define L_SCK 5    
#define L_MISO 19   
#define L_MOSI 27  
#define L_SS 18   // GPIO18 -- SX1278's CS  -->  Tbeam
#define L_RST 23 //  14     // default 23   // GPIO23 -- SX1278's RESET --> Tbeam
#define L_DI0 26   // GPIO26 -- SX1278's IRQ(Interrupt Request)  -->  Tbeam
#define L_DI02 0
#define L_BUSY 32 


// GPS RX/TX serial pins 
#define GPS_RX 12
#define GPS_TX 15

// power management chip avalable in some HW platforms like TTGO
#define POWERCHIP 0  // 0 = powerchip not available


// Power supply subsystem
//#define EN_5V 0
//#define EN_24V_1 1
//#define EN_24V_2 2
//#define EN_24V_3 3

//#define IA0 35
//#define CS0 26
//#define CS1 5

// AgileBeaconing parameters
#define MIN_DELTA_TIME  7000.0   // minimum time interval between beacons in msecs 
#define MIN_DELTA_DISTANCE 30.0  // minimum delta distance to save a new spot position 
#define MIN_DELTA_COURSE 20      // minimum course change to be detected as change direction event in ° 
#define TRS_SLOW_SPEED 40.0      // slow speed treshold Kmh
#define TRS_MEDIUM_SPEED 80.0    // medium speed treshold Kmh
#define TRS_HIGH_SPEED 110.0     // high speed treshold Kmh


/*
// TTGO / Heltec PCB data
BoardConfig::BoardConfig(String name, BoardType type, uint8_t oledsda, uint8_t oledscl, uint8_t oledaddr, uint8_t oledreset, uint8_t lorasck, uint8_t loramiso, uint8_t loramosi, uint8_t loracs, uint8_t lorareset, uint8_t lorairq, bool needcheckpowerchip, bool powercheckstatus)

                                                                                  RX TX,  [      OLED     ] [         LoRa        ]                                       
BoardConfig TTGO_LORA32_V1        ("TTGO_LORA32_V1",         eTTGO_LORA32_V1,     34,12    4, 15, 0x3C,  0,  5, 19, 27, 18, 14, 26);
BoardConfig TTGO_LORA32_V2        ("TTGO_LORA32_V2",         eTTGO_LORA32_V2,             21, 22, 0x3C,  0,  5, 19, 27, 18, 14, 26, true);
BoardConfig TTGO_T_Beam_V0_7      ("TTGO_T_Beam_V0_7",       eTTGO_T_Beam_V0_7,   12,15 , 21, 22, 0x3C,  0,  5, 19, 27, 18, 14, 26, true);
BoardConfig TTGO_T_Beam_V1_0      ("TTGO_T_Beam_V1_0",       eTTGO_T_Beam_V1_0,           21, 22, 0x3C,  0,  5, 19, 27, 18, 14, 26, true, true);

BoardConfig ETH_BOARD             ("ETH_BOARD",              eETH_BOARD,                  33, 32, 0x3C,  0, 14,  2, 15, 12,  4, 36);
BoardConfig TRACKERD              ("TRACKERD",               eTRACKERD,                    5,  4, 0x3C,  0, 18, 19, 23, 16, 14, 26);

BoardConfig HELTEC_WIFI_LORA_32_V1("HELTEC_WIFI_LORA_32_V1", eHELTEC_WIFI_LORA_32_V1, 0, 0,4, 15, 0x3C, 16,  5, 19, 27, 18, 14, 26);
BoardConfig HELTEC_WIFI_LORA_32_V2("HELTEC_WIFI_LORA_32_V2", eHELTEC_WIFI_LORA_32_V2,      4, 15, 0x3C, 16,  5, 19, 27, 18, 14, 26);
*/

// NOTA IMPORTANTE:  se si cambia il file User_setup.h della libreria TFP_eSPI fare attenzione a settare il parametro SPI_FREQUENCY altrimenti salta LoRa SX126x
// #define SPI_FREQUENCY  27000000
//    //#define SPI_FREQUENCY  40000000
// se si usa SX126x  BISOGNA disabilitare la libreria TFT_esPI 

#endif

