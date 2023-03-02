// WiFi_Support_Functions.h
#ifndef WIFI_SUP_H
#define WIFI_SUP_H

#include "rdbg.h"
#include "configuration_data_definition.h"
extern strConfig ESP_Config;

#define USE_MDNS true
#define WEB_SERVER_ENABLED true
#define WEBSOCKET_PORT 8232
#define WIFI_SSID "wifi_ssid_tbd"  // your network SSID (name)
#define WIFI_PASS "wifi_pass_tbd"  // your network key


//#include "RemoteDebug.h"        //https://github.com/JoaoLopesF/RemoteDebug

#include <WiFi.h>
#include <WiFiClient.h>
#include <WebServer.h>  
#include <WiFiMulti.h>
#include <WiFiAP.h>
#include <WiFiUdp.h>
#include <ESPmDNS.h>
#include <esp_wifi.h>
#include <DNSServer.h> // support for DNS server  // https://github.com/bbx10/DNSServer_tng
#include "driver/adc.h"
#define MAX_SRV_CLIENTS 50
#include <ArduinoOTA.h>

// WebServer HTTPServer(80);

//#ifndef DEBUG_DISABLED // Only if debug is not disabled (for production/release)
// RemoteDebug Debug;  // Instance of RemoteDebug
//#endif

#include <Syslog.h>
extern Syslog syslog ;
extern bool syslog_initialized ;
extern bool volatile syslog_available  ;


#include "ezTime_loc.h"



extern strConfig ESP_Config;
extern volatile bool AdminEnabled;
extern  bool standalone ;
extern volatile uint32_t last_wifi_up ;
extern int last_wifi_tag ;
extern  int wifi_unavailable ;
extern bool wifi_enable ;

extern Timezone Italy;
extern Timezone UTC ;
extern time_t now;


// local AP address for GUI support
extern IPAddress ap_local_ip;
extern IPAddress ap_gateway;
extern IPAddress ap_subnet;
extern DNSServer dnsServer;
extern IPAddress apIP;
extern byte DNS_PORT ;

void wifi_scan();
void wifi_scan_serial();
void do_wifi_scan();
void do_wifi_scan_new_tbt();
void WiFi_Upstream_Setup();
void WiFi_AP_Setup();
void NetworkUpstreamMonitor();
void disableWiFi();
void enableWiFi();

#endif
