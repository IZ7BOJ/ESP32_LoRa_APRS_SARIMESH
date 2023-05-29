/* ESP32_LoRa_Radiolib_Beacon.ino  vr. 3.0.x
 * Last updated 20220111 by MF
 * 
 * questo SW supporta solo i moduli LoRa e la base di riferimento è la versione HW C:\KICAD\LoRa_Beacon_2020_Vr4  e successive
 *
 * Package functionalities: this is the main SW package for the  LoRa_Beacon_ESP32 HW assembly and supports all the HW available functionalities in 
 * three main operational modes:
 *   - iGate/Digipeater APRS/LoRa node
 *   - APRS/LoRa Tracker node
 *   - KISS client support via HW serial interface 
 *   - OPI (Orange PI onboard controller) support via Serial RX/TX connection 
 *   - UDP logging interface toward sariserver in direwolf log format
 *   - local logging in FRAM
 *   - RemoteDebug via telnet with https://github.com/JoaoLopesF/RemoteDebug/
 *   - OTA SW package replacement ( under implementation ) https://github.com/esp8266/Arduino/blob/master/doc/ota_updates/readme.rst
 *   
 * The three operationsl configurations will only differ in enabled SW funtionalities, the SW package will be a single one and the operational mode will be selected
 * on SW configuration.
 * 
 * Main supported building blocks:
 *   - OPI controller Orange PI Zero LTS 
 *   - RS232 Serial HW interface
 *   - ESP32 bulk functionalities ( CPU, WiFi, etc.)
 *   - GPS engine ( uBlock, ATGM33x )
 *   - RTC DS3231 
 *   - OLED Display SSD1306 (via SSD1306 library https://github.com/ThingPulse/esp8266-oled-ssd1306
 *                          or SSD1306_Ascii 
 *   - OLED Display ST7789 color 1.14" 
 *   - LoRa RTX support via mikroBus adapter board (first and second gen support)
 *   - PCF857484 port expander
 * 
 * All these blocks are simultaneosly supported via suitable nonblocking SW modules.
 * The HW can communicate with an external PC via:
 *   - USB/serial/WiFi connection for monitoring and SW package loading
 *   - WiFi via local AP 
 *   - wiFi via STA access 
 *   
 * The HW can connect to internet via:  
 *   - WiFi STA upstream functionility
 *   - Ethernet upstream functionility
 * 
 * The onboard serially controlled devices can be accessed via a special passtrought mode:
 *   - Serial1 --> GPS engine / OnBoardController
 * 
 * There are two on board leds:
 *   - RED led
 *   - GREEN Led 
 * 
 * It is possible for the device to operate in following modes with relation to time/frequency synchronization:
 *   - standalone: the device will use the RTC stored time and the RTC intrinsic frequency as base for time/frequency disciplining
 *   - NTP disciplined: WiFi connectivity must be available; external NTP time server is used to discipline local RTC and time/frequency
 *   - GPS disciplined: GPS must be available; time/frequency are disciplined by GPS; this is the hightest precision operational mode.
 * 
 * Basic SW implement a double source ( GPS and NTP)  frequency disciplined oscillator targetted to LoRa and VFO applications.
 * Basic idea is to use GPS PPS and an NTP derived PPS based on a DS3231 RTC to mesure with high precision the frequency of the two oscillators available 
 * in the loRa RTx and in the Si5351 VFO oscillator.
 * The two PPS signals GPS and NTP are used as a gate to count clock pulses between to rising or falling edges respectively
 * The GPS PPS is derived directly from the GPS engine
 * The NTP PPS is sintesized using the DS3231 and a timer on the main CPU disciplined by the NTP info coming from an external NTP server. 
 *
 * A command interface allow to change at run time mode and eventually parameters. based on https://www.john-lassen.de/en/projects/esp-8266-arduino-ide-webconfig
 * Actual library used is https://github.com/GerLech/WebConfig  
 * 
 * Device control is achived via Command line interface via RemoteDebug console and via WiFi mediated WEB interface via WebConfig IF; 
 * configuration data are stored in FRAM memory onboard 
 * 
 * The SW make heavy use of FreeRTOS features for multitasking operation; following main tasks are created:
 *   - bitLash console
 *   - ( LoRa TX/RX control )
 * 
 * Remote debug app support:   https://github.com/JoaoLopesF/RemoteDebugApp#installing
 * 
 * Change History:
 * 20200907:  added UDP logging interface toward sariserver  
 * 202011xx:  added FRAM supported device configuration
 * 202011xx:  semaphore protection of  I2C access from different tasks running on the two cores removed
 * 202011xx:  support for hard reboot by pulsing low pPIO2 ( reset ) via PIO14; supported by a RemoteDebug and bitlash macro reboot
 * 202012xx:  removed bitlash telnet/web console
 * 202012xx:  added RemoteDebug support 
 * 20120106:  removed support for KISS_BT_MODe due to RemoteDebug and OTA incompatibilities.
 * 20210102:  added FRAM local logging support
 * 20210106:  rimosso completamente supporto bluetooh (via #define BLUETOOTH_DISABLED); risparmio 25% di flash (500Kbytes)
 
 * 20210310:  added additional features: 
 *            - Master_config file now contains most HW/SW customization data to allow easy porting to new HW platforms
 *            - Support for HW platform TTGO T-Beam V1
 *            - support for HW platform Heltec WiFi LoRa 32
 *            - support for LITTLEFS based device configuration if FRAM is missing on the HW platform
 *            - support for non standard I2C bus pins ( i.e. Heltec board)
 *            - support for non standard LEDs pins and button pin

 * 20210527: added additional features
 *            - disable repeating LoRa packets  originated from APRS-IS: by using LoRa keyword available in all LoRa orinated packets
 *            - correction display_log first record drop
 *            - introduction GUI Dashboard feature to mimic display content and give additional infos
 *            
 *  20210808: fixed # mark as position field delimitator in APRS strings  
 *            reviwed Time synch strategy        
 *  
 *  20210812: start of 1.0.7 revision; shorten spot report and introduce cpu_temp and AprsLogin in report
 *  
 *  20210818: start of 1.0.8 revision; inclusion of shortBeacon functionality
 *  
 *  20210825: First 1.0.9 revision issue: support for OE_Style encapsulation and OE_Style SW interoperation
 * 
 *  20210927: First 1.0.9.2 revision issue:
 *            support for better TTGO device support
 *            OLED display splashscreen
 *            TFT display splashscreen
 *            small & big HW support unified
 *
 *  20211006: first 1.0.10 revision: support for LoRa Native Beacon functionality - pre VR2 base
 *  
 *  20210909: start of Vr. 2.0 release
 *  20211006: integration of 1.0.9.2 addons inside mainstream (support for TTGO/Heltec devices)
 *            this release supports both SARIMESH and TTGO/Heltec devices by Master.conf customization 
 *            strategy; TTGO/Heltec pin assignement via GUI; supports both 1st and 2nd gen LoRa devices 
 *            via Master.conf customization 
 *  20211010: - Vr. 2.0.3 ; cleanup codice 
 *            - introduzione supporto chip sets RFM98 SX1276 rispettivamente per sarimesh vr. 4.1  e TTGO/Heltec
 *            - segregazione parte APRS e LoRa 
 *            - revisione clock synchronization; introduzione clock jitter
 *            - dettaglio pachetti LoRa, e pacchetti errati
 *            - accodamento pacchetti da trasmetter per evitare packet loss e per implementae timed transmission
 *            - utlizzo diretto FreeRTOS per supporto multitask per parte clock e GPS
 *  20211015: - prima realease ufficiale della Vr2.x
 *  20211016: - Release 2.0.4
 *  20211018: - Inizio vr. 2.0.5 
 *            - introduzione misura dutycycle basato sulla misura del OnAirTime mediato 
 *            - introduzione Proactive Congestion Control basato su misurazione dutycycle da parte dei dispositivi 
 *              e con feedback da parte degli iGate: ogni igate riporta nel suo report il valore di PCC corrente
 *              se un iGate misura un valore di PCC > PCC_Tresh può implementare una metodica di soppressione 
 *              incrementale e randomica dei pacchetti repeated/APRS-IS-originated
 *              se un tracker riceve un report con indicazione di PCC superiore ad una certa soglia, può implementare 
 *              una metodica di riduzione del suo ritmo di invio di spots.
 *              Per i tracker un possibile criterio è imporre che il suo spot-interval_time sia tale 
 *              che il suo duty_cycle risulti < (100 - iGate_duty_cycle)/30
 *              ulteriori meccanismi possono essere implementati come per es. riduzione del ritmo di spot emessi 
 *              in caso di non cambiamento della posizione o inbase alla velocità del tracker 
 *              ( introduzione del concetto di BEACON POLICY )
 *            - introduzione auto_standalone t disable automatic wifi retry inhibit for igate case...  
 *            
 *  20211021: - Vr. 2.0.6 
 *            - introduzione remote reboot via LoRa feature to alleviate WiFi upstream faileure ( via dutycycle report workaroud)
 *              & workaround per cambio configurazione
 *            - inizio introduzione intra-LoRa transparent communication mode: modalità di comunicazione 
 *              diretta tra nodi lora a minimo overheead utilizzando il LoRa native mode.
 *              Un messaggio APRS diretto con path pari al nodo LoRa di destinazione provoca il passaggio 
 *              del nodo TARGET su una frequenza ausiliaria in modalità native/transparent con setup di un 
 *              remote monitor sul nodo target; la modalità persisterà per un certo tempo T dopo di che 
 *              passerà dinuovo in modalità APRS standard sulla frequenza originale
 *              
 *  20211222: - vr. 2.0.16             
 *            - Aggiornameto piattaforma Arduino ad 1.8.19  e ESP32 a 2.0.1; aggiornamento molte librerie in uso
 *            
 *  20211230: - Vr. 3.0.1           
 *            - principale cambio: remote access features & stability issues resolution
 *            - Vr. 3.0.1 get all application LoRa functionalities disabled by default 
 *              in order to evaluate "core" functionalitie stability: LORA_FUNCTIONS_ENABLE 
 *            - Introduzione CORE_LOGGING support
 *            - Introduzione SerialDebug in replacement of bitlash console
 *            
 *  20220108: - Vr 3.0.4         
 *            - supporto Beacon tuningMode
 *            - cleanup debug interface
 *            
 *            
 *  20220109: - Vr. 3.0.5            
 *            - support HMQ MQTT client via https://github.com/knolleary/pubsubclient  API   https://pubsubclient.knolleary.net/api#PubSubClient
 *            - supporto WebClient test 
 *            - supporto MQTT VPN
 *            
 *  20220124: - Vr. 3.0.6                       
 *            - Introduzione supporto INA219  https://github.com/flav1972/   https://whatibroke.com/2019/08/10/change-i2c-address-of-ina219/   indirizzi IN219
 *            - introduzione supporto analog mux 4051
 *            - introduzione port expander CPM23017 
 *               https://www.best-microcontroller-projects.com/mcp23017.html#:~:text=MCP23017%20Addressing&text=So%20the%20MCP23017%20I2C%20address,to%20any%20single%20I2C%20bus. 
                 datasheet  https://www.best-microcontroller-projects.com/support-files/mcp23017-20001952c.pdf
                 https://github.com/adafruit/Adafruit-MCP23017-Arduino-Library  libreria supporto
                 https://github.com/DFRobot/DFRobot_MCP23017  librera che sembra funzionare

 *  20220130: - Vr. 3.0.7_20220130_113   
 *            - allineamento alla versione 5.1 della libreria Radiolib 
 *            - fissato problema lora.standby()  ( https://github.com/jgromes/RadioLib/issues/50 )
 *            - fix CAD 
 *  20220210: - 3.0.8_20220201_2252
 *            - introduzione upstream monitor
 *            - non regression test su core functions and GUI operation
 *            
 *  20220204: - 3.0.9_20220104_2852          
 *            - introdotto display animation su solo display TFT
 *            - sostituita libreria OLED con SSD1306_ASCIILib per fixing adafruit memory bug con ESP32
 *            - regression tests su HMQ faileure e su supporto ASCII Lib OLED
 *            
 *  20220219: - 3.0.10               
 *  
 *  20220214: - 3.0.11
 *            - introdotto randomizzazione intervallo di beacon
 *            - ridotto a 15 sec lo schedule beacon singola fase
 *            
 *  20220214: - 3.0.12
 *            - introduzione distinguo single_phase_10 single_phase_18  single_phase_23  single_phase_100 per distinguere la durata della fase Beacon
 *            - tutte le modalità eccetto la _100 sono singolo collegamento; la _100 consete di avere fino a 4 fasi di 23 secondi
 *
 *  20220310: - 3.0.15_20220310
 *            - gps_loop modification to skip GPSUpdate if GPS is not available or disabled
 *            
 *  20220316: - 3.0.18      
 *            - hang_time reboot if hangtime > 30 sec
 *            
 *  20220318: - 3.0.19_20220318            
 *            - fix gps and RTC hungs
 *            - TCP_KISS_Mode introduction
 *            
 * 20220320:  - 3.0.20_20220320
 *            - TCP_KISS_Mode + AX25/OE_Style + GUI + display
 *            
 * 20220416:  - 3.0.21_20220416
 * 
 * 20220425:  - 3.0.22_20220425
 *            - fix aprs beacon emission time in mixed beacon/aprs mode; now aprs beacon are sent 
 *              synchronously when in beacon mode, at times related the current device Id
 *         
 *  20220609: - 3.0.24_20220609        
 *            - fix setCurrentLiit
 * 
 * 20221208:  - 3.0.25_20221209
 *            - introduzione supporto modulo RA-01S
 *            - introduzione supporto CAD disable by master_config force_CAD_disable
 *            - gestione CAD disable da GUI
 *            - introduzione RX-only in modalità iGate gestita da GUI
 *            - introduzione modalità split-frequency per usare frequenze e modi diversi in RX e TX
 *            - introduzione modalità beacon-only RX/TX per fare prove di copertura radio
 *            - introduzione modalità becon adattivo vs velocità
 */
 
 
 
 
