/* ESP32_LoRa_Radiolib_Beacon.ino  vr. 3.0.x
 * Last updated 20220111 by MF
 * 
 * 
 * 
 */


// support for CORE_DEBUG level
#ifdef CORE_DEBUG_LEVEL
#undef CORE_DEBUG_LEVEL
#endif

#define CORE_DEBUG_LEVEL 5
//#define LOG_LOCAL_LEVEL ESP_LOG_DEBUG

bool  debug_disabled = false ;   // indicates if most of the debugging messages are disabled for performance 
bool  wait_default_config = false ; // wait for defaults loading by pushing button at startup


//********************** include FreeRTOS API support ************************
#include "freertos/FreeRTOS.h"
#include "freertos/task.h"
#include "freertos/timers.h"
#include "driver/pcnt.h"  /////
#include "ESP32_RTOS_Support_Functions.h"
#include "esp_task.h"    
#include <esp_int_wdt.h>
#include <esp_task_wdt.h>
#include <freertos/semphr.h>
#include <freertos/queue.h>

// include arduino basics
#include "Arduino.h"

#define INTERRUPT_ATTR IRAM_ATTR 

// task sync and IPP support
SemaphoreHandle_t i2c_mutex_v; 
SemaphoreHandle_t SPI_mutex_v;
SemaphoreHandle_t LocationManager_mutex_v;
SemaphoreHandle_t ClockManager_mutex_v;
SemaphoreHandle_t LoRa_Device_Handler_mutex_v;
QueueHandle_t xQueue_in;
QueueHandle_t xQueue_APRS;
QueueHandle_t xQueue_native;
QueueHandle_t msg_queue;
QueueHandle_t xQueue_park;
QueueHandle_t xQueue_native_park;

// create multiple tasks handles
TaskHandle_t BCN_Controller_Handle;          // handles BCN engine operation
TaskHandle_t LocationManager_handle;         // handles device location
TaskHandle_t LoRa_Device_Handler_handle;         // handles device location
TaskHandle_t ClockManager_handle;            // handles device local realtime clock
TaskHandle_t DisplayManager_handle;          // handle local device displays
TaskHandle_t AuxResourceManager_handle;      // handles leds, buttons, buzzer, etc.
TaskHandle_t LoRaManager_handle;             // handles low level LoRa devices for setup, send and receive operations
TaskHandle_t BeaconController_handle;        // controls beacon send/receive functionalities
TaskHandle_t MQTT_Manager_handle;            // controls MQTT send/receive functionalitie
TaskHandle_t Task3;                          // used by tft_....

// task service routines prototypes
void BCN_Controller( void * pvParameters );      //BCN_Controller: Beacon sender/receiver controller task
void LocationManager( void * pvParameters );         // handles device location
void LoRa_Device_Handler( void * pvParameters );         // handles device location
void ClockManager( void * pvParameters );            // handles device local realtime clock
void DisplayManager( void * pvParameters );          // handle local device displays
void AuxResourceManager( void * pvParameters );      // handles leds, buttons, buzzer, etc.
void LoRaManager( void * pvParameters );             // handles low level LoRa devices for setup, send and receive operations
void BeaconController( void * pvParameters );        // controls beacon send/receive functionalities
void MQTT_Manager( void * pvParameters );            // controls MQTT send/receive functionalitie
volatile bool Task3_flag = true ;  //  starts APRS  packet tansmission via Task_3 

// RT Task enable flags
volatile bool BCN_Controller_flag = false ;  //  starts BCN_Controller
volatile bool LocationManager_flag = false ;         // handles device geo location infos
volatile bool LoRa_Device_Handler_flag = false ;     // handles device geo location infos
volatile bool ClockManager_flag = false ;            // handles device local realtime clock
volatile bool DisplayManager_flag = false ;          // handle local device displays
volatile bool AuxResourceManager_flag = false ;      // handles leds, buttons, buzzer, etc.
volatile bool LoRaManager_flag = false ;             // handles low level LoRa devices for setup, send and receive operations
volatile bool BeaconController_flag = false ;        // controls beacon send/receive functionalities
volatile bool MQTT_Manager_flag = false ;            // controls MQTT send/receive functionalitie

#ifdef FORCE_DEFAULTS                   // if true force default configuration 
  bool force_config_init = true ;
#else
  bool force_config_init = false ;
#endif

#ifdef SAVE_PROTECT                    // if false inhibits configuration saving for most parameters 
  volatile bool save_config_enable = false ;
#else
  volatile bool save_config_enable = true ;
#endif

//******************************* Device Configuration related data  ***************************
#include "Device_Conf.h"

//*******************************  config data, flags & parameters - Operation mode  ***************************
//#include "default_configuration_data.h"   // defines default srtConfig configuration class and instance
// allocate device configuration structure
strConfig  ESP_Config ;

//*************   wire library to support I2C driver ****************
#include "Wire.h"

//******************** I2C_Support_Functions ************************************
#include "I2C_Support_Functions.h"

//******************** RemoteDebug Support  ************************************
#ifndef FORCE_SERIAL_DEBUG
#include "RemoteDebug.h"     //https://github.com/JoaoLopesF/RemoteDebug
RemoteDebug Debug ;
#endif

// **********************  on board button , leds from cpu or via PCF1684 parallel IO expander support setup *******************
// PCF857484 on board IO expander I2C driven: this is the I2C address used by the device on board
// libreria utilizzata https://www.mischianti.org/2019/01/02/pcf8574-i2c-digital-i-o-expander-fast-easy-usage/
//#include "PCF8574.h"
#include "PortExpander8574_Support_Functions.h"

// create pcf8574 object  to manage pcf8574 attached devices
PCF8574 pcf8574(0x21);

//*******************************  FRAM Support  ***************************
// https://github.com/FrankBoesing/FastCRC/blob/master/examples/FastCRC_validate/FastCRC_validate.ino
//#include <util/crc16.h>
#include <FastCRC.h>
FastCRC32 CRC32;
#include "FRAM.h"    //     URL: https://github.com/RobTillaart/FRAM_I2C
FRAM fram;

//**************** support for LITTLEFS *********************
// from  https://github.com/lorol/LITTLEFS now in the standard libs
#include <FS.h>
#include <LittleFS.h> 

// Our local FS will be on the internal SPI flash of the esp32 board
fs::FS localFs = LittleFS;
volatile bool has_LittleFS = false;           // willl be asserted by autodiscovery

// *******************  hardware serial interface support in esp32 ****************************
// mandatory to use serial interfaces 1 and 2 ; interface 0 is mapped over the USB and is not physically accessible on board
// Serial HW device support
#include <HardwareSerial.h>
HardwareSerial Esp32Serial1(1); // GPS
HardwareSerial Esp32Serial2(2); // DRA828
Stream * aSerial;

//******************************** GPS support via TinyGPSPlus Library for ESP32 *****************
// TinyGPSPlus  https://github.com/mikalhart/TinyGPSPlus
// include local GPS support functions
#ifdef GPS_MOD
 volatile bool no_gps = false;
#else
 volatile bool no_gps = true;
#endif
#include "GPS_Support_Functions.h"

//******************   support for ezTime high precision time library ***************************
// ezTime library  https://github.com/ropg/ezTime
#include "ezTime_loc.h" // local copy patched by MF for GPS/RTC addition to NTP sync

// ******************* RTC DS3231 & PPS_Support_Functions support ******************************
#include "PPS_Support_Functions.h"       // defines timers and ISR
#include "DS3231_Support_Functions.h"
RtcDS3231<TwoWire> Rtc(Wire);

//************************************ EEPROM ***********************************************
#include <EEPROM.h>

//******************   support for wifi/ethernet/NTP/mDNS/Syslog  ***************************
// include support for WiFi interface
#include "WiFi_Support_Functions.h"
// WiFi Related data  local AP address for GUI support
IPAddress apIP(192, 168, 5, 1);
byte DNS_PORT = 53;
DNSServer dnsServer;
WiFiMulti wifiMulti;
//how many clients should be able to telnet to this ESP32
//WiFiClient serverClients[MAX_SRV_CLIENTS];
WebServer web_server(80);       // The Webserver for GUI
WiFiServer Kiss_server(10001);  // KISS_Over_TCP server
WiFiClient Kiss_client ;        // client definition for listening on  kiss_server incoming clients
WiFiUDP UDPNTPClient;           // used by NTP Client for NTP server interaction
// private LoRa GeoLocator server support
// direwolf log report support
// by MF 20200818
// IP address to send UDP data to.
// it can be ip address of the server or 
// a network broadcast address
// here is broadcast address
//create UDP instance
String udpAddress = "192.168.2.150";
int udpPort = 44444;
WiFiUDP udp;     // this is the cochet for talking with remote udp server repository

//Syslog syslog(syslogClient, SYSLOG_PROTO_IETF);   // Create a new empty syslog instance
WiFiUDP syslogClient;           // A syslog instance to let us send and receive packets over UDP to a remote syslog server to collect our log messages
Syslog syslog(syslogClient, SYSLOG_PROTO_BSD);   // Create a new empty syslog instance
String SyslogDeviceName = "TBD";

//******************* OTA support *********************************
#ifdef USE_ARDUINO_OTA
  #include <ArduinoOTA.h>
  WebServer HTTPServer(88);
#endif

//******************* Common Remote and  Serial Debug support *********************************
#ifdef FORCE_SERIAL_DEBUG  
// SerialDebug Library
// Disable all debug ? Good to release builds (production)
// as nothing of SerialDebug is compiled, zero overhead :-)
// For it just uncomment the DEBUG_DISABLED
//#define DEBUG_DISABLED true

// Disable SerialDebug debugger ? No more commands and features as functions and globals
// Uncomment this to disable it 
//#define DEBUG_DISABLE_DEBUGGER true   // not used in this project

// Debug TAG ?
// Usefull with debug any modules
// For it, each module must have a TAG variable:
//     const char* TAG = "...";
// Uncomment this to enable it
//#define DEBUG_USE_TAG true

// Define the initial debug level here (uncomment to do it)
// #define DEBUG_INITIAL_LEVEL DEBUG_LEVEL_VERBOSE

  #include "SerialDebug.h" //https://github.com/JoaoLopesF/SerialDebug
#else
  #include "RemoteDebug.h"        //https://github.com/JoaoLopesF/RemoteDebug
#endif

#ifndef DEBUG_DISABLED          // Only if debug is not disabled (for production/release)
// RemoteDebug related variables
// Disable the auto function feature of RemoteDebug  Good if your code already have func name on debug messages
//#define DEBUG_DISABLE_AUTO_FUNC true
// Disable Websocket This is used with RemoteDebugApp connection
//#define WEBSOCKET_DISABLED true
#ifndef WEBSOCKET_DISABLED // Only if Web socket enabled (RemoteDebugApp) If enabled, you can change the port here (8232 is default)
  #define WEBSOCKET_PORT 8232
  // Internally, the RemoteDebug uses a local copy of the arduinoWebSockets library (https://github.com/Links2004/arduinoWebSockets)
  // Due it not in Arduino Library Manager If your project already use this library, Uncomment the line below, to do it:
  #define USE_LIB_WEBSOCKET true
#endif 
#endif

void Debug_Loop(void) ;
void RemoteDebug_Setup(void) ;
void SerialDebug_Setup(void) ;


//*******************************   WebConfig vr 1.1.3 support  ***************************
#include "global.h"                  // includes default webconfig configuration 
#define ServerVersion "1.0"          // GUI server reference version
#include "File_Download_Upload.h"    // save/load configuration support
#include "helpers.h"
volatile bool WebConfig_debug ;      // to debug webConfig 
byte packetBuffer[ NTP_PACKET_SIZE]; 

// local AP address for GUI support
IPAddress ap_local_ip(192,168,5,1);
IPAddress ap_gateway(192,168,5,1);
IPAddress ap_subnet(255,255,255,0) ;


//****************************** web_client Test  setup ***************************************
#ifdef WEBCLIENT_TEST_ENABLE  
#ifdef WIFI_UPSTREAM
  WiFiClient web_client; // Initialize the WiFi web_client
#endif
#endif


//************************* HiveMQ Brocker Setup *********************************
#ifdef HMQ_MQTT_ENABLE  
  // https://www.survivingwithandroid.com/esp32-mqtt-client-publish-and-subscribe/
  // to test use http://www.hivemq.com/demos/websocket-client/
  //   subscribe  /micfuc2020/replay   publish  /micfuc2020/commands
  #define HMQ_SERVER      "96a46d5d34ba46b88464e911a9d5f064.s1.eu.hivemq.cloud"
  #define HMQ_SERVERPORT  8883
  #define HMQ_USERNAME    "micfuc2020"
  #define HMQ_PASSWORD    "Papera_123"

  // this is the public MQTT server HMQ
  char *HMQ_mqttServer = (char *)"broker.hivemq.com";
  int HMQ_mqttPort = 1883;
  //char *HMQ_mqttServer = (char *)HMQ_SERVER;
  //int HMQ_mqttPort = HMQ_SERVERPORT;

  uint32_t HMQ_last_time = millis(); 
  uint8_t HMQ_cntr = 0; 
  uint32_t HMQ_last_connect_trial = millis(); 

  #include <PubSubClient.h>

  #ifdef WIFI_UPSTREAM
    WiFiClient HMQ_Client;  // HMQ MQTT client instance
    PubSubClient mqttClient(HMQ_Client); 
  #endif
#endif

//************************* MQTT VPN Setup *********************************
#ifdef HMQ_MQTT_VPN_ENABLE
  #include "mqttif.h"
  // VPN settings
  char* vpn_broker = "mqtt://broker.hivemq.com:1883";
  char* vpn_mqtt_user = "micfuc2020";
  char* vpn_mqtt_password = "Papera_123";
  char* vpn_password = "auguri";
  ip4_addr_t vpn_ipaddr;
  ip4_addr_t vpn_netmask;
  ip4_addr_t vpn_gw;
#endif

//*********************** Adafruit.io initialize MQTT protocol support
#ifdef AIO_MQTT_ENABLE  
  #include "Adafruit_MQTT.h"
  #include "Adafruit_MQTT_Client.h"
  #include "MQTT_test_case.h"
  uint32_t AIO_last_time = millis(); 
  uint8_t AIO_cntr = 0; 

  //************************* Adafruit.io Brocker Setup *********************************
  #define AIO_SERVER      "io.adafruit.com"
  #define AIO_SERVERPORT  1883
  #define AIO_USERNAME    "micfuc2020"
  #define AIO_KEY         "aio_CTVA815UG8zgtbvelXUgyc9JbNdn"

  #ifdef WIFI_UPSTREAM
    WiFiClient mqtt_client; //Set up the wifi client
    Adafruit_MQTT_Client mqtt(&mqtt_client, AIO_SERVER, AIO_SERVERPORT, AIO_USERNAME, AIO_KEY);
    #define halt(s) { Serial.println(F( s )); while(1);  }

    //******************************  Adafruit MQTT Feed setup ***************************************
    // https://io.adafruit.com/micfuc2020/dashboards
    // Setup a feed called 'photocell' for publishing.
    // Notice MQTT paths for AIO follow the form: <username>/feeds/<feedname>
    Adafruit_MQTT_Publish photocell = Adafruit_MQTT_Publish(&mqtt,  AIO_USERNAME "/feeds/photocell");

    // Setup a feed called 'onoff' for subscribing to changes.
    Adafruit_MQTT_Subscribe onoffbutton = Adafruit_MQTT_Subscribe(&mqtt, AIO_USERNAME "/feeds/onoff");
  #endif
#endif

// **********************  SPI  devices support *******************
#include <SPI.h>

//**************************** support for OLED and TFT display  *************************
#include <Adafruit_GFX.h>    // Core graphics library
#include <Fonts/FreeMonoBoldOblique12pt7b.h>
#include <Fonts/FreeMono9pt7b.h>
#include "FONTS/Basic_Comical_Regular_NC12pt7b.h"
#include "FONTS/Basic_Comical_Regular_NC11pt7b.h"
#include "FONTS/Basic_Comical_Regular_NC10pt7b.h"
#include "FONTS/Basic_Comical_Regular_NC9pt7b.h"
#include "FONTS/Basic_Comical_Regular_NC8pt7b.h"
#include "FONTS/Basic_Comical_Regular_NC7pt7b.h"
#include "FONTS/Basic_Comical_Regular_NC6pt7b.h"

#ifdef TFT_MOD
  #ifdef USE_ADAFRUIT_TFT_LIB 
    // SPI ST7735/7789 based TFT display + SD card support
    #include <Adafruit_ST7735.h> // Hardware-specific library for ST7735
    #include <Adafruit_ST7789.h> // Hardware-specific library ffor ST7789
    // create tft object
    //Adafruit_ST7735 tft = Adafruit_ST7735(TFT_CS, TFT_DC, TFT_RST);
    Adafruit_ST7789 tft = Adafruit_ST7789(TFT_CS, TFT_DC, TFT_RST);      // 1.14" display
  #else
    // SPI TFT_eSPI ST7735 display support
    #include <TFT_eSPI.h> // Hardware-specific library local copy patched for compatibiity with adafruit libraries
    #include "FONTS/Comics_16.h"
    #include "FONTS/Comics_18.h"
    #include "FONTS/Comics_20.h"
    #include "FONTS/Free_Fonts.h" // Include the header file attached to this sketch
    TFT_eSPI tft = TFT_eSPI();       // Invoke custom library
  #endif
#endif   // TFT_MOD


//*************************** SSD1306 OLED Display support *****************************************
#ifdef OLED_MOD
  #ifdef USE_SSD1306_ASCII_LIB
    #include "SSD1306Ascii.h"
    #include "SSD1306AsciiWire.h"
    SSD1306AsciiWire display;
  #else                                // OLED display support via adafruit OLED library
    #include <Adafruit_SSD1306.h>
    #define SCREEN_WIDTH 128 // OLED display width, in pixels
    #define SCREEN_HEIGHT 64 // OLED display height, in pixels
    #ifdef O_RST     // Define proper RST_PIN if required.
      //  Adafruit_SSD1306 display(SCREEN_WIDTH, SCREEN_HEIGHT, &Wire, O_RST);
      Adafruit_SSD1306 display(SCREEN_WIDTH, SCREEN_HEIGHT, &Wire, oled_rst); 
    #else
      Adafruit_SSD1306 display(SCREEN_WIDTH, SCREEN_HEIGHT, &Wire );
    #endif
        //  #include "OLED_test_case.h"
  #endif
#endif   // OLED_MOD

//***************************************** SD card support  **************************
#ifdef SD_MOD
  // These are used to get information about static SRAM and flash memory sizes
  extern "C" char __data_start[];    // start of SRAM data
  extern "C" char _end[];     // end of SRAM data (used to check amount of SRAM this program's variables use)
  extern "C" char __data_load_end[];  // end of FLASH (used to check amount of Flash this program's code and data uses)
  // Libraries for SD card
  #include "FS.h"
  #include "SD.h"
  #include "sd_test_case.h"
#endif  // SD_MOD

//*********************  Support for power supply subsystem ******************************************
#ifdef INA219_MOD
  #include <INA219.h>
  INA219 monitor_24(INA219::I2C_ADDR_41 );  // please check module straps setup
  INA219 monitor_5(INA219::I2C_ADDR_44 );   // please check module straps setup
  volatile bool has_INA219_5 ;                // autodiscovery
  volatile bool has_INA219_24 ;                // autodiscovery
  void PS_INA219_get (void) ;
#endif   // INA219_MOD


#ifdef MUX4051_MOD
  // Analog Mux 4051 support
  #include "Mux.h"
  volatile bool has_MUX4051 = true ;

  using namespace admux;
  // Creates a Mux instance.
  //  1st argument is the SIG (signal) pin (Arduino analog input pin A0).
  // 2nd argument is the S0-S3 (channel control) pins (Arduino pins 8, 9, 10, 11).
  admux::Mux mux(Pin(IA0, INPUT, PinType::Analog), Pinset(CS0, CS1));
  uint8_t channels[4]={EN_5V,EN_24V_1, EN_24V_2, EN_24V_3 };
  //double chan_scale[4]={1.707, 9.563, 9.563, 9.563 };
  double chan_scale[4]={3.36, 11.92, 11.92, 11.92 };
  uint8_t chan_status[4]={0,0,0,0 };    // 0 => OFF   < 100 => OFF-counting up   if 100 => ON   > 100 => ON-counting counting down
  double Vin_bat , V24_curr, V5_curr, V24_power, V5_power ;
  double V5_out, V24_out_1, V24_out_2, V24_out_3;
  void Mux_loop();
#else
  volatile bool has_MUX4051 = false ;
#endif // end  MUX4051_MOD

#ifdef PS_FUNCTIONS_ENABLE
  volatile bool has_PS_5V = false ;     // to be confirmed by autodetection
  volatile bool has_PS_24V_1 = false;  
  volatile bool has_PS_24V_2 = false;  
  volatile bool has_PS_24V_3 = false;
  volatile bool V5_ena=0 ;               // enable V5 out channel
  volatile bool V24_1_ena = 0 ;          // enable V24_1 out channel
  volatile bool V24_2_ena = 0 ;          // enable V24_2 out channel
  volatile bool V24_3_ena = 0;           // enable V24_3 out channel
  volatile uint8_t PS_Out_Reset = 0 ;
  volatile bool Bat_discarge = 0 ;
  bool PS_HBT = false;
  void PS_Chan_enable(uint8_t chan);
  void PS_Chan_disable(uint8_t chan);
  double PS_Chan_read(uint8_t chan);
  void PS_Chan_reset(uint8_t chan);
  void PS_Chan_Manager(void);
  void loop_PS();
#endif

// support for SPI bus
#include <SPI.h>

// support for arduino-timer
#include <arduino-timer.h>


#ifdef LORA_FUNCTIONS_ENABLE    // disables all LoRa related functionalities for core system stability evaluation

  // ********************************** LoRa  Libraries and parameters *************************************
  #include <RadioLib.h>
  #include "LoRa_Device_Handler.h"

  // ********************************** loraprs app support *************************************
  #include "loraprs_config.h"
  #include "loraprs_service.h"
  //  volatile bool aprsis_available = false; 
  extern LoraPrs::Service loraPrsService;

  //****************************** LoRa Native Beacon support ***************************************
  //supports LoRa beconing based on a binary payload serialized via the LoRa Serialization library
  // https://github.com/thesolarnomad/lora-serialization 

  #include "beacon_aux_functions.h"

  //*********************** ShortBeacon functionality support *********************
  // extern volatile uint8_t EncapType ;                       // 0=LoRa   1=APRS


  // ********************************** LoRa  Libraries and parameters *************************************
  #include <RadioLib.h>
  #include "LoRa_Device_Handler.h"

  String  LoRaDevice = LORA_DEVICE ;   // E22_400M30S , E22_400M22S, RFM98 , RA_01S
 
  volatile  uint8_t LoraDeviceType = -1 ;   // lora device generation -1= not jet identified  0=first generation 1=second generation 
  SX1268 lora_2g = new Module(L_SS, L_DI0, L_RST,L_BUSY);
  SX1278 lora_1g = new Module(L_SS, L_DI0, L_RST,L_BUSY);


  // ********************************** loraprs app support *************************************
  #include "loraprs_config.h"
  #include "loraprs_service.h"

  //****************************** LoRa Native Beacon support ***************************************
  //supports LoRa beconing based on a binary payload serialized via the LoRa Serialization library
  // https://github.com/thesolarnomad/lora-serialization 

  #include "beacon_aux_functions.h"

  //*********************** ShortBeacon functionality support *********************
  //  extern volatile uint8_t EncapType ;                       // 0=LoRa   1=APRS
  #include "BCN_Structs.h"
  #include "Lora_Radiolib_Support_Functions.h"
#endif // LORA_FUNCTIONS_ENABLE


//*************************** Tickers definition *****************************************
#include <Ticker.h>
Ticker tkSecond;                 // 1 Second - Timer 
Ticker tk30Second;              // 30 Second - Timer 
Ticker tk60WebClient;           // 30 Second - Timer 
Ticker tk_mqtt;                 // mqtt  - Timer 
Ticker tkTimeSync;              // TimeSyncCheck  - Timer 
Ticker tkDutyCycle;             // DutyCycle  - Timer 
Ticker tk60NTPClient ;
Ticker tk_ps_mon;               // power supply monitor  - Timer  5 sec default
Ticker tk_upstream_mon;            // 
Ticker tk_aprsis_mon;            // 

void reconnectAprsis();
void Second_Tick();
void DutyCycle_Monitor(void);
void loop_global(void);
void OTA_loop(void);
void printLocalTime();



void setup(){
  //******************** initilize standard serial connection and console ***************************
  SerialDebug_Setup();  // startup SerialDebug 

#ifdef FORCE_SERIAL_DEBUG  
  debugA("ESP32_LoRa_Sarimesh_Core Vr. %s \r", SW_Revision.c_str() ); // Serial.println("");
  debugA( "Last Update 20230110 by MF" );
  debugA("setup():SerialDebug consolle is BIDIRECTIONAL now...\r" ); 
#else
  Serial.printf("ESP32_LoRa_Sarimesh_Core Vr. %s \r\n", SW_Revision.c_str() ); // Serial.println("");
  Serial.printf( "Last Update 20230110 by MF\r\n" );
  Serial.printf("setup(): SerialDebug consolle is DISPLAY only now...\r\n" ); 
  delay(500); // Wait a time
#endif

  //******************************* Load default HW_Device_Configuration ****************************
  HW_Set_SARIMESH(); // always try to start with our default HW device configuration
  show_HW_Device_Config() ;

#ifdef DEBUG_DISABLED
  Serial.print("\r\n\r\n========= System Debug features are disabled by build definition ===========\r\n");
  debug_disabled = true ;
#endif


// startup kernel CORE_LOGGING support
  LastFunction = __func__ ;
  debugA("Setup kernel CORE_LOGGING debug support\r"); // will be shown in the terminal
  Serial.setDebugOutput(false);
  esp_log_level_set("*", ESP_LOG_NONE);
/*  
  ESP_LOGD("EXAMPLE", "This doesn't show");
  log_v("Verbose");
  log_d("Debug");
  log_i("Info");
  log_w("Warning"); 
  log_e("Error");
*/

//******************** create i2c_mutex_v ***************************
    i2c_mutex_v = xSemaphoreCreateMutex(); 
    if (i2c_mutex_v != NULL) { 
       debugA("i2c_mutex_v Mutex successfully created"); 
     };

//******************** create SPI_mutex_v ***************************
    SPI_mutex_v = xSemaphoreCreateMutex(); 
    if (SPI_mutex_v != NULL) { 
       debugA("SPI_mutex_v Mutex successfully created"); 
       };  

//******************** create LocationManager_mutex_v ***************************
    LocationManager_mutex_v = xSemaphoreCreateMutex(); 
    if (LocationManager_mutex_v != NULL) { 
       debugA("LocationManager_mutex_v Mutex successfully created"); 
       };

//******************** create ClockManager_mutex_v ***************************
    ClockManager_mutex_v = xSemaphoreCreateMutex(); 
    if (ClockManager_mutex_v != NULL) { 
       debugA("ClockManager_mutex_v Mutex successfully created"); 
       };


//******************** create LoRa_Device_Handler_mutex_v ***************************
    LoRa_Device_Handler_mutex_v = xSemaphoreCreateMutex(); 
    if (LoRa_Device_Handler_mutex_v != NULL) { 
       debugA("LoRa_Device_Handler_mutex_v Mutex successfully created"); 
       };

//******************** create LoRa_Device_Handler InterProcessCommunication ***************************
    if(Setup_LDH_IPP()){
       debugA("Setup_LDH_IPP: successfully completed"); 
        } 
    else {
       debugA("Setup_LDH_IPP: failed... no IPP available... aborting  "); 
       };

//*********************  setup WDT *******************
// freeRTOS system tuning https://github.com/espressif/arduino-esp32/blob/master/cores/esp32/esp32-hal.h#L76-L91
  disableCore0WDT();
  disableCore1WDT();
  disableLoopWDT();
//  feedLoopWDT();


//******************** disable master SW reset if present ***************************
  if(reboot_pin){
     pinMode(reboot_pin, OUTPUT);      // disable master reset  
     digitalWrite(reboot_pin, HIGH);
     };

//******************** quick buzzer disable ***************************
if(buzzer_pin){
  pinMode(buzzer_pin, OUTPUT);      // buzzer driver for PIO12
  buzzer_disable();
  };

//*********************  init I2C wire.h driver and try to pre-init OLED display  *******************
// try to initialize I2C bus 
Wire.setClock(100000L);
i2c_initialized = false  ; int i2cDevices=0 ; hw_arch=0 ;//  try pin pairs 4,15 or 21,22 
Wire.begin(i2c_sda, i2c_scl); 
I2C_Scanner() ;
i2cDevices=I2C_Scanner_alt() ;
if ( i2cDevices == 0){
  Serial.printf("i2C_init(): NO device found.. may be I2C INIT FAILED: i2c_initialized=%d - i2c_sda=%d  - i2c_scl=%d - running on core=", i2c_initialized, i2c_sda, i2c_scl );   
  Serial.println(xPortGetCoreID());    
  }
else {
  i2c_initialized = true  ;
  if(has_FM24W256 && has_PCF8574 ) hw_arch = 101 ;   // SARIMESH device with FRAM and PCF8574 - iGate PCB
  if(has_FM24W256 && ! has_PCF8574 ) hw_arch = 102 ; // SARIMESH device with FRAM - miniTracker PCB
     
  if( ! has_FM24W256 ){  
    if(i2c_sda == 21) hw_arch = 105 ; // TTGO like device
    if(i2c_sda == 4) hw_arch = 106 ;  // Heltec like device       
    } ;  
  Serial.printf("i2c_init(): SUCCESS: i2cDevices=%d i2c_initialized=%d - hw_arch=%d - i2c_sda=%d  - i2c_scl=%d - running on core=", i2cDevices , i2c_initialized, hw_arch ,i2c_sda, i2c_scl );   
  Serial.println(xPortGetCoreID());    
};

#ifdef OLED_MOD
// try to reset OLED display via software if a reset pin is available and an OLED display is present
if(oled_rst){                 // try to reset OLED display via IOpin just in case
  pinMode(oled_rst, OUTPUT);
  digitalWrite(oled_rst, LOW);
  vTaskDelay(pdMS_TO_TICKS(20));
  digitalWrite(oled_rst, HIGH);
  };
#endif

Serial.printf("i2c_init(): status_display: has_PCF8574=%d - has_DS3231=%d - has_DS3231_eeprom=%d - has_SSD1306=%d - has_FM24W256=%d - running on core=", has_PCF8574, has_DS3231 ,has_DS3231_eeprom, has_SSD1306, has_FM24W256 );   
Serial.println(xPortGetCoreID());    


//************************** init EEPROM based configuration  *************************
  EEPROM.begin(512);  // open EEPROM storage holding the device configration infos

//*************** disable all possible conflicting pins on SPI bus *****************
#ifdef W5500_MOD
  pinMode(W5500_RST, OUTPUT);      // reset for ethernet W5500
  digitalWrite(W5500_RST, HIGH);
#endif

#ifdef LORA_MOD
  pinMode(lora_rst, OUTPUT);      // reset for LoRa 
  digitalWrite(lora_rst, HIGH);
  pinMode(lora_cs, OUTPUT);      // CS for LoRa - 1 CS disable
  digitalWrite(lora_cs, HIGH);
#endif

#ifdef TFT_MOD
  pinMode(TFT_CS, OUTPUT);      // CS for SPI display - 1 CS disable
  digitalWrite(TFT_CS, HIGH);
  pinMode(TFT_DC, OUTPUT);      // CS for SPI display - 1 DC disable
  digitalWrite(TFT_DC, LOW);
#endif

#ifdef SD_MOD
  pinMode(SD_CS, OUTPUT);      // CS for SD Card - 1 CS disable
  digitalWrite(SD_CS, HIGH);
  pinMode(SD_RST, OUTPUT);      // CS for SD Card - 1 CS disable
  digitalWrite(SD_RST, HIGH);     
#endif

//******************** init driver for PCF8574 port expander connected devices ***************************
if(has_PCF8574){                                     // setup port expander
  debugA("\rSetup port expander pcf8574... (LastFunction=%s\r)", LastFunction.c_str());

  #ifndef PWRS_MOD
     // Set pinMode to OUTPUT or INPUT for LoRa Tracker
     pcf8574.pinMode(P7, OUTPUT);  // green led
     pcf8574.pinMode(P6, OUTPUT);  // red led
     pcf8574.pinMode(P5, INPUT);   // pushbutton
     pcf8574.pinMode(P4, OUTPUT);  // ex buzzer
  #else
     // Set pinMode to OUTPUT or INPUT for PS
     pcf8574.pinMode(P0, OUTPUT);  // EN_5V
     pcf8574.pinMode(P1, OUTPUT);  // EN_24V_1
     pcf8574.pinMode(P2, OUTPUT);   // EN_24V_2
     pcf8574.pinMode(P3, OUTPUT);  // EN_24V_3
     pcf8574.pinMode(P4, OUTPUT);  // EXT_REL
     pcf8574.pinMode(P5, INPUT);   // PB
     pcf8574.pinMode(P6, OUTPUT);  // G_LD
     pcf8574.pinMode(P7, OUTPUT);  // R_LD  
  #endif
  
  // initialize pcf8574 library
  if (pcf8574.begin()){
    debugA("success");
       }
  else{
    debugA("Failed: PCF8574 discovered but not working --> assume has_PCF8574=0 ");
    has_PCF8574 = false;   
      };
    } 
else{    // no port expander available
  if(green_led){
    pinMode(green_led, OUTPUT);      // if no pcf8574 this is the Green_LED (mini Tracker case)
    digitalWrite(green_led, LOW);   
    };
  if(red_led){
    pinMode(red_led, OUTPUT);      // if no pcf8574  this is the red_LED (mini Tracker case)
    digitalWrite(red_led, LOW);   
    };
};  // end of has_PCF8574 
 
// test red/green leds
green_flash();            // flash green led
red_flash();              // flash red led 
vTaskDelay(pdMS_TO_TICKS(1000));
green_red_flash();        // flash red and green leds
soundBuzzer();            // beep buzzer

debugA("Setup(): I2C led/buzzer init completed.(LastFunction=%s)\r", LastFunction.c_str());


//******************** initilize FRAM storage ***************************

if(has_FM24W256){
   debugA("Initialize FRAM device access....");
   int rv;
   if(i2c_sda){ rv = fram.begin(i2c_sda,i2c_scl,0x50);} else { rv = fram.begin(0x50);};
   if (rv != 0){
      Serial.print(rv);
      };
   // Serial.print("FRAM ManufacturerID: ");
   // Serial.println(fram.getManufacturerID());
   // Serial.print("     ProductID: ");
   // Serial.println(fram.getProductID());
   // Serial.print("     memory size: ");
   // Serial.println(fram.getSize());
   debugA("done");
};

//******************** initialize LITTLEFS ***************************
#include "LittleFS_Support_Functions.h"

debugA("Setup LittleFS volume... ");
#define FORMAT_LITTLEFS_IF_FAILED true
if(!LittleFS.begin(FORMAT_LITTLEFS_IF_FAILED)){
  debugA("LittleFS Mount Failed");
  has_LittleFS = false;
  } 
else{
  has_LittleFS = true;
  };
debugA("done");

//  LittleFS.remove("/WebConf.conf");  / to remove a file...

bool selftest_debug= false ;
if(selftest_debug){
  debugA("\n\r==================== running LITTLEFS unit test case ==========================\r");
  ut_littlefs_setup();
  debugA("\r================= WebConf.conf file dump from LITTLEFS =========================");
  File file = LittleFS.open("/WebConf.conf");  // sample file access
  if(!file){ 
    Serial.println("Failed to open /WebConf.conf file for reading");
    }
  else {
    listDir(LittleFS, "/", 2);
    debugA("/WebConf.conf file Content:");
    while(file.available()){
      Serial.write(file.read());
      };
    file.close();
  };
  debugA_loc("\n\r==================== end of LITTLEFS test case ==========================\r\n");
};



//******************** Restore to Factory default  ***************************

// open  time windows to load default configuration
Serial.print("\n\r--------------> Open wait_default_config time window... \r\n");
wait_default_config = true   ;   
RED_LED_enable();
GREEN_LED_enable();
int cntrx = 100;
while (cntrx--) {
  vTaskDelay(pdMS_TO_TICKS(50));
  Serial.print("*");
  check_button();
  };
Serial.println("*");
RED_LED_disable();
GREEN_LED_disable();
wait_default_config = false  ; 
Serial.println("--------------> Close wait_default_config time window \r");

// close default config loading on startup time window


//******************** load or initialize device configuration from FRAM or LittleFS ***************************
// if no config is found a default config is loaded: hence after this step a valid configuration should be always available.

// force_config_init= true ; // to test forcing default configuration
if (LoadConfig( force_config_init ) == false) Serial.print("Failed loading Device configurations: forced default configuration\r\n");

// assuming AprsLogin from the device configuration as official NodeName
NodeName = ESP_Config.AprsLogin ;

//DisplayConfig_serial(); 
Serial.print("We should have here a valid configuration... either the default or the custom one.... \r\n");  
    
// These data are hardcoded in this SW build
ESP_Config.SW_Revision = SW_Revision ;  // force present SW version
ESP_Config.cpu_type = CPU_TYPE ;  // force present cpu_type
AdminEnabled = true;  // start with admin mode enabled 

// update local variables, configuration dependent; this customize such parameters according to device NVM based configuration
Serial.print("Update now local runtime variables, configuration dependent; this customize such parameters according to device NVM based configuration\r\n");
UpdateLocalConfigData();    //  implements the current configuration in the run time status of this device

//******************** test for spurious reboot ***************************
RebootTag = 0 ;          // check for spurios system reboot
if( has_FM24W256 ){  
  RebootTag = fram.read8(2015);
  if (RebootTag != 123 ){
    // write_event_to_log("========= SPURIOUS system reboot discovered ===========");
    debugA("\r\n\r\n\r\n==== SPURIOUS system reboot discovered  RebootTag=%d - Forcing clean reboot =======\r\n", RebootTag);
    RebootTag = 123 ;
    fram.write8(2015,RebootTag);
    vTaskDelay(pdMS_TO_TICKS(1000));
    RebootTag = 0;
    RebootTag = fram.read8(2015);
    if( RebootTag == 123) Serial.printf("===> Reboot tag correctly written to FRAM ..."); 
      debugA("===> Rebooting...");         
      pinMode(14, OUTPUT);      // hard reset 
      digitalWrite(14, LOW); 
      }
    else {
      // write_event_to_log("========= CLEAN system reboot found ===========");
      debugA("\r\n\r\n========= CLEAN system reboot found  RebootTag=%d ===========\r\n", RebootTag);
      RebootTag = 0 ;
      fram.write8(2015,RebootTag);
      vTaskDelay(pdMS_TO_TICKS(1000));         
      };  
    };

  // fix syslog_enable / standalone
  //  standalone = false ; syslog_enable = false ;  wifi_enable = false ;  LoRa_debug = 0; APRS_debug =0 ;   no_gps = false ; 

  //      ESP_Config.AprsRawBeaconIGATE = "I1XYZ-10,WIDE1-1,LoRa iGate,#,L";
  //      ESP_Config.AprsRawBeaconTRACK = "I1XYZ-10,WIDE1-1,LoRa Tracker,#,L" ;


//*********************************** OLED DISPLAY HW  setup  *******************


#ifdef OLED_MOD
    #ifdef USE_SSD1306_ASCII_LIB
     Serial.print( "Using SSD1306_ASCII_LIB LIB\r\n" );
    #else
     Serial.print( "Using Adafruit SSD1306_LIB LIB\r\n" );
    #endif

    if(has_SSD1306){
    #ifdef USE_SSD1306_ASCII_LIB   // use USE_SSD1306_ASCII_LIB
      if (oled_rst > 0){
        display.begin(&Adafruit128x64, oled_addr, oled_rst);
        }
      else { 
        display.begin(&Adafruit128x64, oled_addr);
      };
      display.displayRemap(true);
      //  display.setFont(X11fixed7x14);
      display.setFont(Arial14);
      display.setContrast(200);
      display.clear();
      display.setRow(1); 
      display.println("        Welcome to");  
      display.setRow(3);
      display.println("Esp32 LoRa Beacon ");  
      display.setRow(5);
      display.println("       by SARIMESH");
    #else   // Adafruit OLED SSD1306 library
      if(oled_rst){                      // For Heltec WiFi LoRa 32 
        pinMode(oled_rst,OUTPUT);
        digitalWrite(oled_rst, LOW);    // set GPIO16 low to reset OLED
        vTaskDelay(pdMS_TO_TICKS(50));
        digitalWrite(oled_rst, HIGH);   // while OLED is running, must set GPIO16 in high
      };
 
     if(!display.begin(SSD1306_SWITCHCAPVCC, oled_addr , false, false)) { // Address 0x3C for 128x32
        Serial.println("SSD1306 allocation failed");
     };
      display.setTextWrap(false);
      display.setFont(&Basic_Comical_Regular_NC7pt7b);
      display.clearDisplay();
      if(oled_orient){ display.setRotation(2); } else { display.setRotation(0); };
      display.dim(false);
      display.setTextColor(WHITE);
      display.setTextSize(1);
      display.setCursor(0,10);
      display.print("         Welcome to");  
      display.setCursor(0,28);
      display.print("   Esp32 LoRa Beacon ");  
      display.setCursor(0,46);
      display.print("       by SARIMESH");  
      display.display();  
    #endif  // end of Adafruit OLED SSD1306 library
  }; // end of if has_SSD1306

  vTaskDelay(pdMS_TO_TICKS(3000)); // let's display our banner for few seconds!

  if(has_SSD1306){
    #ifdef USE_SSD1306_ASCII_LIB   // uses task2 for loop: disabled  uncommento to re-inable
      display.clear();
      display.setRow(1);
      display.println( VersionBanner.c_str()); 
      display.setRow(4);
      display.println("        Booting...."); 
    #else   // Adafruit OLED SSD1306 library
      display.clearDisplay(); display.display();
      display.setTextWrap(false);
      if(oled_orient){ display.setRotation(2); } else { display.setRotation(0); };
      display.setTextColor(WHITE);
      display.setFont(&Basic_Comical_Regular_NC6pt7b);        
      display.setCursor(0, 30); display.print( VersionBanner.c_str()); 
      display.setCursor(40, 45); display.print( "Booting....");  
      display.display();  
    #endif  // end of Adafruit OLED SSD1306 library
  }; // end of has_SSD1306

#endif   //  OLED_MOD

//*********************************** TFT DISPLAY HW  setup  *******************
#ifdef TFT_MOD
  #ifdef USE_ADAFRUIT_TFT_LIB
    Serial.print( "Using ADAFRUIT_SPI_TFT LIB\r\n" );
  #else
    Serial.print( "Using TFT_eSPI LIB\r\n" );
  #endif

  if(has_ST77XX){
    #ifdef USE_ADAFRUIT_TFT_LIB     // using USE_ADAFRUIT_TFT_LIB
      tft.init(135, 240);           // Init ST7789 240x135
      tft.fillScreen(ST77XX_BLACK);
      //  tft.setFont(&FreeMono9pt7b);
      tft.setFont(&Basic_Comical_Regular_NC12pt7b);
      //  tft.setFont();
      tft.setRotation(1);
      tft.fillScreen(ST77XX_BLACK);
      //  tftPrintTest();
      tft.setCursor(0, 0); 
      //  tft.setTextFont(GFXFF);
      //  tft.setTextColor(TFT_WHITE, TFT_BLACK);
      tft.setTextColor(ST77XX_GREEN);
      tft.setCursor(0, 30); tft.print("         Welcome to");
      tft.setCursor(0, 60); tft.print("   Esp32 LoRa Beacon ");
      tft.setCursor(0, 90); tft.print("       by SARIMESH");
    #else          //  Using TFT_eSPI LIB
      tft.init(); 
      tft.setRotation(3);
      tft.loadFont(Final_Comics_18);  
      digitalWrite(25, HIGH);
      tft.fillScreen(TFT_BLACK);
    #endif
  };  // end of has_ST77XX

  vTaskDelay(pdMS_TO_TICKS(3000)); // let's display our banner for few seconda!

  //************************* send booting message on display... *********************
  // let's start TFT display test task (Task3)
  if(selftest_debug){
    // setup tft test cases: disabled ; to re-enable uncomment....
    #ifdef USE_ADAFRUIT_TFT_LIB   // uses task2 for loop
      // tft_adafruit test case
      setup_tft_adafruit();
    #else
      // tft_eSPI_test case
      setup_tft_eSPI();
    #endif
  };   // selftest_debug

  if(has_ST77XX){  // adafruit TFT_LIB
    #ifdef USE_ADAFRUIT_TFT_LIB
      tft.fillScreen(ST77XX_BLACK);
      tft.setTextWrap(false);
      tft.setTextColor(ST77XX_GREEN);
      tft.setTextSize(1);
      tft.setCursor(10, 40);  tft.print(VersionBanner.c_str());
      tft.setCursor(30, 60);  tft.print("Booting....(TFTLib)");
    #else   // eSPI
      tft.fillScreen(TFT_BLACK);
      tft.setTextColor(TFT_GREEN, TFT_BLACK);
      tft.setCursor(10, 40);  tft.print(VersionBanner.c_str());
      tft.setCursor(30, 60);  tft.print("Booting....(eSPILib)");
    #endif
  };   // has_ST77XX

#endif   //  TFT_MOD

//*********************************** SD_card  setup  *******************
// Setup SD_Card test case
#ifdef SD_MOD
    setup_SD_Card();  // do SD card discovery & init TFT display
#endif


// NETWORK_TAG start setup

//*********************************** upstream wiFi setup  *******************
//Set DeviceId from wifi MAC address
String _apName = WiFi.macAddress();
_apName = "ESP32-" + _apName ;
_apName.replace(":","");
DeviceId=_apName;

//standalone = false ;
//wifi_enable = ! standalone  ; 
//AdminEnabled = false;

// check for standalone mode requested ( i.e. no network connection at all)
if(standalone){
    Serial.println("Starting in standalone mode (no WiFi upstream) by configuration ...");
    WiFi_AP_Setup();

    // mDNS startup
    String _mdnsName = WiFi.macAddress();
    _mdnsName = "ESP32-" + _mdnsName ;
    _mdnsName.replace(":","");
    if(!MDNS.begin(_mdnsName.c_str())){             // Start the mDNS responder for esp32.local
       Serial.println("Error setting up MDNS responder!");
      }
    else{
       Serial.printf("mDNS responder started (%s.local)\r",_mdnsName.c_str());
    };

     
    // modify TTL associated  with the domain name (in seconds) default is 60 seconds
    dnsServer.setTTL(300);
    // set which return code will be used for all other domains (e.g. sending ServerFailure instead of NonExistentDomain will reduce number of queries sent by clients)
    dnsServer.setErrorReplyCode(DNSReplyCode::ServerFailure);// default is DNSReplyCode::NonExistentDomain
    // start DNS server for a specific domain name dnsServer.start(DNS_PORT, "www.example.com", apIP);
    dnsServer.start(DNS_PORT, "lora.esp32", apIP);
    // setup NTP client
    //  timeClient.begin();
  
  }
else {   // NON standalone situation.. we need to initialize upstream network connections
    Serial.println("Starting in NO standalone mode with  WiFi Upstream / NTP enabled by configuration ...");
    Serial.print( "Setting WIFI_UPSTREAM\r\n" );
    // first of all perform a wifi scanning to discover upream WiFi networks available
    wifi_scan_serial();
    WiFi_Upstream_Setup();  // this will take care of most of the STA mode setup    
    // mDNS startup
    String _mdnsName = WiFi.macAddress();
    _mdnsName = "ESP32-" + _mdnsName ;
    _mdnsName.replace(":","");
    if(!MDNS.begin(_mdnsName.c_str())){             // Start the mDNS responder for esp32.local
       Serial.println("Error setting up MDNS responder!");
      }
    else{
       Serial.printf("mDNS responder started (%s.local)\r",_mdnsName.c_str());
    };
      
    // modify TTL associated  with the domain name (in seconds) default is 60 seconds
    dnsServer.setTTL(300);
    // set which return code will be used for all other domains (e.g. sending ServerFailure instead of NonExistentDomain will reduce number of queries sent by clients)
    dnsServer.setErrorReplyCode(DNSReplyCode::ServerFailure);// default is DNSReplyCode::NonExistentDomain
    // start DNS server for a specific domain name dnsServer.start(DNS_PORT, "www.example.com", apIP);
    dnsServer.start(DNS_PORT, "lora.esp32", apIP);
    // setup NTP client
    //  timeClient.begin();
};  // end of standalone check

// NETWORK_TAG end

#ifndef FORCE_SERIAL_DEBUG
  Serial.printf("Starting RemoteDebug...\r" );
  // Initialize the server (telnet or web socket) of RemoteDebug
  RemoteDebug_Setup();  // from nowon we can use debugA to issue advices on both the seral and remote consoleif enabled
#endif


//****************************** syslog service setup ***************************************
// prepare syslog configuration here (can be anywhere before first call of log/logf method)
// full ref. https://github.com/arcao/Syslog/blob/master/examples/AdvancedLogging/AdvancedLogging.ino#L72
// Syslog server connection info
if(syslog_enable) {
  #ifdef WIFI_UPSTREAM
    if( ! standalone){     
      debugA("Start Syslog server connection .... \r");
      SyslogDeviceName = WiFi.macAddress();
      SyslogDeviceName = "ESP32-" + SyslogDeviceName ;
      SyslogDeviceName.replace(":","");
      syslog.server(SYSLOG_SERVER, SYSLOG_PORT);
      syslog.deviceHostname(SyslogDeviceName.c_str());
      syslog.appName(APP_NAME);
      syslog.defaultPriority(LOG_LOCAL0);
      syslog_initialized = true ;   // the server is now setup ... but the connection could be not jet available
      if(! wifi_unavailable) { 
        syslog.logf(LOG_INFO, "%s: Starting %s syslog service at %s", VersionBanner.c_str(),  SyslogDeviceName.c_str(), (UTC.dateTime(ezt::now(), "d/n/Y H:i:s.v T")).c_str());
        syslog_available = true ;
        debugA("Syslog server connection started (%s)\r", SyslogDeviceName.c_str());
        } 
      else {
        syslog_available = false ;
        debugA("Syslog server connection not jet available (%s)\r", SyslogDeviceName.c_str());
        };
      }
    else {
      syslog_available = false ;
      syslog_initialized = false ;
      debugA("Syslog server connection is disabled (because upstream network is disabled)!\r");  
    };
  #else
    debugA("Syslog server connection is unavailable due to missing upstream network connection )!\r");
    syslog_initialized = false ;
    syslog_available = false ;
  #endif
  }
else {  
  debugA("Syslog server connection is disabled by Configuration!\r");
  syslog_initialized = false ;
  syslog_available = false ;
  };



//*********************  initialize mDash remote controller *******************
/*   VOID
mDashSetLogLevel("3");
mDashBegin(DEVICE_PASSWORD);
mDashSetLogLevel("3");
*/

//**************** initialize OTA  ***************************************

#ifdef USE_ARDUINO_OTA
  if( Admin_Mode) { 
    debugA("Perform OTA setup .... \r");
    OTA_setup();
  };   
#endif

red_flash();   // flash red led 

//******************************  management webserver startup **************************
WebServer_Init();
debugA("management web server started...\r"); 

//******************************  init random number generator **************************

randomSeed(millis()%10);              // initialize pseudo-random randon numeber generator

if(syslog_available) syslog.log(LOG_INFO, "========= END_OF_GENERIC_TAG ===========");




// END_OF_GENERIC_TAG

if( ! Admin_Mode) {     // start of addons initialization 
  // *************************  initialize AprsLogger parameters  ************************************
  udpAddress = ESP_Config.AprsLoggerAddr;
  udpPort = ESP_Config.AprsLoggerPort;
  debugA("Aprs Logger Parameters:  %s:%d\r", udpAddress.c_str(), udpPort);

  // *************************  initialize gps engine  ************************************
  //   if(syslog_available) syslog.log(LOG_INFO, "========= initialize gps engine ===========");

  Loc_Lat = ESP_Config.Loc_Lat; // initialize our default by GUI location if available
  Loc_Lon = ESP_Config.Loc_Lon;
  debugA("Location Coordinates by GUI defaults:  (%s / %s)\r", Loc_Lat.c_str(), Loc_Lon.c_str());
   
  //******************** create task to perform LocationManager operation ***************************//    if(syslog_available) syslog.log(LOG_INFO, "========= initialize LocationManager ===========");
    
  debugA("Create LocationManager task to perform geo location update\r"); 
  //create a task that will execute the LocationManager() function, with priority 1 and executed on core 1
  // it is up to LocationManager to initialize GPS engine and allow to have always
  // a consistent location value also if GPS is not available or invalid
  xTaskCreatePinnedToCore(
                    LocationManager,   // Task function. 
                    "LocationManager", // name of task. 
                    10000,            // Stack size of task 
                    NULL,             // parameter of the task 
                    1,                // priority of the task 
                    &LocationManager_handle,  // Task handle to keep track of created task 
                    0);               // pin task to core 0                   

  vTaskDelay(pdMS_TO_TICKS(500));
  LocationManager_flag = true ;  // start LocationManager disabled to allow further initialiation  
  vTaskDelay(pdMS_TO_TICKS(2000));
  // LocationManager services should now be up&running

  debugA("\rGPS engine should be now up&running \r"); 
  //******************* Initialize time related services for DS3231, NTP and GPS operation  **********************//   if(syslog_available) syslog.log(LOG_INFO, "========= initialize time related services ===========");
  //******************** create task to perform ClockManager operation ***************************
  debugA("Create ClockManager task to perform local time clock update\r"); 
  //create a task that will execute the ClockManager() function, with priority 1 and executed on core 1
  // it is up to ClockManager to initialize connection with GPS engine, DS3231 and allow to have always
  // a consistent clock value also if GPS or DS3231 is not available or invalid
  xTaskCreatePinnedToCore(
                    ClockManager,     // Task function. 
                    "ClockManager",   // name of task. 
                    10000,            // Stack size of task 
                    NULL,             // parameter of the task 
                    1,                // priority of the task 
                    &ClockManager_handle,  // Task handle to keep track of created task 
                    0);               // pin task to core 0                   

  vTaskDelay(pdMS_TO_TICKS(500));
  ClockManager_flag = true ;  // start ClockManager enabled to allow further initialiation  
  vTaskDelay(pdMS_TO_TICKS(2000));
  // ClockManager services should now be up&running

  debugA("\rClockManager should be now up&running \r"); 

  //******************   support for ezTime high precision time library ***************************
  // ezTime library  https://github.com/ropg/ezTime
  // global GPS eTime support variables
  timeStatus_t GPS_time_status ;
  timeStatus_t NTP_time_status ;

  #include "ezTime_loc.h"      // local copy patched by MF for GPS/RTC integration


  // time related func prototypes
  time_t nowUTC(const bool update_last_read );
  const long  gmtOffset_sec = 3600;
  const int   daylightOffset_sec = 3600;

  // ************************  Initialize Current measurement ******************************************
  #ifdef INA219_MOD
    monitor_24.begin();
    monitor_5.begin();  
    // begin calls:
    // configure() with default values RANGE_32V, GAIN_8_320MV, ADC_12BIT, ADC_12BIT, CONT_SH_BUS
    // calibrate() with default values D_SHUNT=0.1, D_V_BUS_MAX=32, D_V_SHUNT_MAX=0.2, D_I_MAX_EXPECTED=2
    // in order to work directly with ADAFruit's INA219B breakout
    //  monitor.configure(INA219::RANGE_32V, INA219::GAIN_8_320MV, INA219::ADC_64SAMP, INA219::ADC_64SAMP, INA219::CONT_SH_BUS);
    //  monitor_5.configure(INA219::RANGE_32V, INA219::GAIN_8_320MV, INA219::ADC_64SAMP, INA219::ADC_64SAMP, INA219::CONT_SH_BUS);
    #ifdef PS_FUNCTIONS_ENABLE
      //   tk_ps_mon.attach(5,loop_PS);
      Bat_discarge= false;   // battery is not discarging
    #endif

    // enable upstream battery connection
      RED_LED_enable();   
  #endif
      
//*************************  Set webclient and mqtt test *******************************
#ifdef WEBCLIENT_TEST_ENABLE  
  //   if(syslog_available) syslog.log(LOG_INFO, "========= initialize webclient and mqtt ===========");
  // start webclient test to check internet connectivity
  tk60WebClient.attach(60,loop_W5500_WebClient);
#endif

void test_beacon(void);
//  tk30Second.attach(35,test_beacon);  // to test beacon operation sending a beacon every 30 secs

#ifdef HMQ_MQTT_ENABLE  
if(mqtt_enable){
  HMQ_setupMQTT();  // setup HiveMQ connection
  };
#endif

// setup MQTT vpn
#ifdef HMQ_MQTT_VPN_ENABLE
  Serial.println("Starting VPN Arduino ESP32"); // will be shown in the terminal
  Serial.setDebugOutput(true);
  esp_log_level_set("*", ESP_LOG_VERBOSE);
  IP4_ADDR(&vpn_ipaddr, 10,0,1,2);
  IP4_ADDR(&vpn_netmask, 255,255,255,255);
  IP4_ADDR(&vpn_gw, 0,0,0,0);
  mqtt_vpn_if_init(vpn_broker, vpn_mqtt_user, vpn_mqtt_password, "mqttip", vpn_password, vpn_ipaddr, vpn_netmask, vpn_gw);
#endif

#ifdef AIO_MQTT_ENABLE  
  // Adafruit IO MQTT brocker connection test setup
  mqtt.subscribe(&onoffbutton);
  // start MQTT loop tiker driven  
  tk_mqtt.attach(10,loop_mqtt);
#endif


#ifdef DUTY_CYCLE_MONITOR_ENABLE
  // start DutyCycle monitor tiker driven
  tkDutyCycle.attach(60,DutyCycle_Monitor);
#endif

#ifdef LORA_FUNCTIONS_ENABLE
  //********************************** LoRa setup  *****************************************
  // initialize LoRa device and communication
  //   if(syslog_available) syslog.log(LOG_INFO, "========= initialize LoRa_Device_Handler ===========");

// following ifdef to be replace by autodetection with new generation lora carriers
#ifdef LORA_MODULE_TYPE_SX1268
  LoraDeviceType = 1 ;   // lora device generation -1= not jet identified  0=first generation 1=second generation 
  debugA("Assuming Second Generation LoRa module installed.... \r"); 
#else
  LoraDeviceType = 0 ;   // lora device generation -1= not jet identified  0=first generation 1=second generation 
  debugA("Assuming First Generation LoRa module installed.... \r"); 
#endif

  debugA("\rUsing LoRaDevice=%s - LoraDeviceType=%d \r", LoRaDevice.c_str(), LoraDeviceType ); 
  LoRa_Device_Handler_setup();   // alternative operation mode to avoid SPI mutexing...
  debugA("LoRa_Device_Handler should be now up&running \r"); 
  debugA("Setup Radiolib_LoraPrs deferred to later.... startup will be managed by loop_Lora_aprs \r"); 
  BCN_Controller_flag = false ;  // disable BCN_Controller ---
  debugA("Setup Radiolib_BCN deferred to later.... startup will be managed by loop_Lora_BNC \r"); 




#endif // LORA_FUNCTIONS_ENABLE

//*************************  activate Ticker managed activities  *******************************
if(syslog_available) syslog.log(LOG_INFO, "========= initialize Ticker managed activities ===========");
tkSecond.attach(1,Second_Tick);  // manage adminmode timeout

};  // end of NON admin mode activated functions


// Following functions will be active in either Admin mode or NON Admin Mode ( then always active...) 

#ifdef NET_UPSTREAM_MON_ENABLE
  if(! standalone ){     
    debugA("NetworkUpstreamMonitor: starting periodic check (60 sec).... \r");
    tk_upstream_mon.attach(60,NetworkUpstreamMonitor);
  };

#ifndef APRS_IS_DISABLE
  if( ! standalone ) {
    debugA("APRS-IS_UpstreamMonitor: starting periodic check (70 sec).... \r");
    tk_aprsis_mon.attach(70,AprsMonitor);
    aprsis_available = false ; 
  };
#endif

#endif

debugA("check_time_sync_source_availability: starting periodic check (120 sec).... \r");
tkTimeSync.attach(120,check_time_sync_source_availability);  // manage TimeSyncCheck timeout

debugA("ESP32 chip temperature indication = %6.2f\r", temperatureRead() );
cpu_temp = temperatureRead();

#ifdef TEMPERATURE_MONITOR_ENABLE
  debugA("print_temperature: starting periodic check (60 sec).... \r");
  tk30Second.attach(60,print_temperature);
#endif

// let's start the LocationManager and flag for gps_fix_static
if(has_GPS && ! no_gps) {
  LocationManager_flag = true ;      // handles device geo location infos
  gps_fix_static = 0 ;
  }
else {
  gps_fix_static = 1 ;
  };


// end of setup - start main loop to service web interface
green_red_flash(); 
last_millis=millis();
  
configTime(gmtOffset_sec, daylightOffset_sec, ntpServer.c_str());
printLocalTime();
soundBuzzer(); vTaskDelay(pdMS_TO_TICKS(500)); soundBuzzer();

//*************** show_HW_Device_Config *****************
//  debugA("\rshow_HW_Device_Config: after system startup....\r"); 
//  show_HW_Device_Config();

// re-init SSD1306 display just in case ( display library incompatibility with multitasking...)
debugA("Starting Main loop at now=%d - millis=%d\r",ezt::now(),millis());  last_display_update=ezt::now(); 
//  debugA("*********************************************\r");

write_event_to_log("========= system reboot completed ===========");
// Severity levels can be found in Syslog.h. They are same like in Linux syslog.
if(syslog_available) syslog.logf(LOG_INFO, "System %s reboot completed at %s", SyslogDeviceName.c_str(),(UTC.dateTime(ezt::now(), "d/n/Y H:i:s.v T")).c_str());

// int iteration = 1;
// Log message can be formated like with printf function.
//  syslog.logf(LOG_ERR,  "This is error message no. %d", iteration);
//  syslog.logf(LOG_INFO, "This is info message no. %d", iteration);

// You can force set facility in pri parameter for this log message. More 
// facilities in syslog.h or in Linux syslog documentation.
//  syslog.logf(LOG_DAEMON | LOG_INFO, "This is daemon info message no. %d",  iteration);

// F() macro is supported too
//  syslog.log(LOG_INFO, F("End loop"));
// iteration++;

debugA("LastFunction=%s\r", LastFunction.c_str());
display_event = 7 ;  // show waiting for LoRa spot on local display

debugA("End of setup... Entering Main loop\r\n");

}  ;  // End of setup activities


/**************************************************************************
  Main loop function, called once 'setup' is complete 
**************************************************************************/
void loop ( void ) {
  LastFunction = __func__ ;  
  //============================== activity led enable ================================================     
  uint32_t timeBeginLoop = millis();  // main loop monitoring
  bool volatile loop_debug = true;
  loop_time = millis() - last_millis ;
  if ( loop_time >= 1000 ){ // just try to perform this check every 1000 milliseconds... 
    activity_start = millis(); activity_flag = true;
    last_millis=millis();
    GREEN_LED_enable(); 
    if( (loop_time >= 1900 ) && loop_debug ){
      debugA("====> FreeMem:%d - millis=%d - loop_time=%d - activity_time=%d - uptime=%lld - time_jitter=%ld - UTC.dateTime: %s\r", ESP.getFreeHeap() , millis(), loop_time , activity_time,  esp_timer_get_time(), time_jitter, (UTC.dateTime(ezt::now(), "d/n/Y H:i:s.v T")).c_str());
//     if(syslog_available) syslog.logf(LOG_INFO, "FreeMem:%d - millis=%d - loop_time=%d - activity_time=%d - uptime=%lld - time_jitter=%ld - UTC.dateTime: %s ", ESP.getFreeHeap() , millis(), loop_time , activity_time,  esp_timer_get_time(), time_jitter , (UTC.dateTime(now(), "d/n/Y H:i:s.v T")).c_str());
      if(syslog_available) syslog.logf(LOG_INFO, "loop_time=%d - act_time=%d - time_jitter=%ld - uptime=%lld - UTC.dateTime: %s ", loop_time , activity_time,  time_jitter,  esp_timer_get_time(), (UTC.dateTime(ezt::now(), "d/n/Y H:i:s.v T")).c_str());
    };
  };

  // START_APPLICATION_TAG 

  if( ! Admin_Mode) {   // application_part
    //******************** LoRa services handling  **************************************
    #ifdef LORA_FUNCTIONS_ENABLE
    loop_Lora_native();            // LoRa native service subsystem monitor
    loop_Lora_aprs();              // LoRa APRS service subsystem monitor
    LoRa_Device_Handler_loop();    // LoRa_Device_Handler_loop
    #endif // LORA_FUNCTIONS_ENABLE

    //=========================== HMQ MQTT service loop =====================================================
    #ifdef HMQ_MQTT_ENABLE 
    if(mqtt_enable){
      HMQ_loop_MQTT() ;   // keep alive mqtt server connection
      };
    #endif
  };  // end of activities to be performed ONLY when NON in Admin mode

  //******************** global loop for core management activities  **************************************
  loop_global();   // serve web management GUI interface on port 80
  Debug_Loop();    // Serve common Remote and Serial Debug console:over serial, telnet, websocket running on port 23 / 8232

  #ifdef USE_ARDUINO_OTA   
  if( Admin_Mode) {  
    OTA_loop();  // OTA SW update support on port 88 if in admin mode
    };
  #endif

  //============================== local display update ===============================================
  LocalDisplayUpdate();

  //============================== pushbutton status update ================================================
  if( millis() - last_button_millis >= 100 ){   // pushbutton polling
    last_button_millis = millis();
    check_button();
  };

  #ifndef DEBUG_DISABLED
  // Show a debug - warning if time of these loop is over 50 (info) or 100 ms (warning)
  uint32_t time = (millis() - timeBeginLoop);
  if (time > 1000) {
    debugI("* Time elapsed for the loop: %u ms.", time);
    } 
  else if (time > 1050) {
    debugW("* Time elapsed for the loop: %u ms.", time);
    };
  #endif  // DEBUG_DISABLED  4
   
  //============================== local DNS server update ================================================   
  if(! standalone ){
    // serve DNS requests
    dnsServer.processNextRequest();
  };

  //============================== dummy loop time consumer ================================================
 
  #ifndef LORA_FUNCTIONS_ENABLE   
    vTaskDelay(pdMS_TO_TICKS(200));  // just give time to see the green_led flashing....
  #endif 

  //============================== activity led disable ================================================   
  if(activity_flag) {
    GREEN_LED_disable();
    activity_flag = false ;
    activity_time = millis() - activity_start;
  };
    
}    // end of main loop

//  LOOP_END_TAG


void loop_test(){
  Serial.print(".");
  debugA("* This is a message of debug level VERBOSE");
	debugD("* This is a message of debug level DEBUG");
	debugI("* This is a message of debug level INFO");
	debugW("* This is a message of debug level WARNING");
  debugHandle();  
  vTaskDelay(pdMS_TO_TICKS(1000));
}



