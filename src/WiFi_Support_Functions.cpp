// WiFi_Support_Functions.cpp
// Master configuration directives: need to be customized depending on HW/SW configuratio of the device
#include "Master_Config.h"
#include "WiFi_Support_Functions.h"

// wifi scan 
void wifi_scan(){
    debugA("wifi_scan start...\r");
    // Set WiFi to station mode and disconnect from an AP if it was previously connected
    WiFi.mode(WIFI_STA);
    WiFi.disconnect();
    vTaskDelay(pdMS_TO_TICKS(100));
    // WiFi.scanNetworks will return the number of networks found
    int n = WiFi.scanNetworks();
    debugAl("scan done: ");
    if (n == 0) {
        debugAl("no networks found\r");
    } else {
        debugAl("%d",n);
        debugAl(" networks found\r");
        for (int i = 0; i < n; ++i) {
            // Print SSID and RSSI for each network found
            debugAl("%d",i + 1);
            debugAl(": ");
            debugAl("%s",WiFi.SSID(i));
            debugAl(" (");
            debugAl("%d",WiFi.RSSI(i));
            debugAl(")");
            debugAl("%s",(WiFi.encryptionType(i) == WIFI_AUTH_OPEN)?" ":"*");
            vTaskDelay(pdMS_TO_TICKS(10));
        }
    }
    debugAl("\r");
  
}

void wifi_scan_serial(){
  Serial.print("wifi_scan start...");
    // Set WiFi to station mode and disconnect from an AP if it was previously connected
    WiFi.mode(WIFI_STA);
    WiFi.disconnect();
    vTaskDelay(pdMS_TO_TICKS(100));
    // WiFi.scanNetworks will return the number of networks found
    int n = WiFi.scanNetworks();
    Serial.print("scan done: ");
    if (n == 0) {
        Serial.println("no networks found");
    } else {
        Serial.print(n);
        Serial.println(" networks found");
        for (int i = 0; i < n; ++i) {
            // Print SSID and RSSI for each network found
            Serial.print(i + 1);
            Serial.print(": ");
            Serial.print(WiFi.SSID(i));
            Serial.print(" (");
            Serial.print(WiFi.RSSI(i));
            Serial.print(")");
            Serial.println((WiFi.encryptionType(i) == WIFI_AUTH_OPEN)?" ":"*");
            vTaskDelay(pdMS_TO_TICKS(10));
        }
    }
    Serial.println("");
     
}

// wifi scan 
void do_wifi_scan(){
    debugA("wifi_scan start...");
    // WiFi.scanNetworks will return the number of networks found
    int n = WiFi.scanNetworks();
    debugA("scan done: ");
    if (n == 0) {
        debugA("no networks found\r");
    } else {
        debugA("%d",n);
        debugA(" networks found\r");
        for (int i = 0; i < n; ++i) {
            // Print SSID and RSSI for each network found
            debugA("%d",i + 1);
            debugA(": ");
            debugA("%s",WiFi.SSID(i));
            debugA(" (");
            debugA("%d",WiFi.RSSI(i));
            debugA(")");
            debugA("%s\r",(WiFi.encryptionType(i) == WIFI_AUTH_OPEN)?" ":"*");
            vTaskDelay(pdMS_TO_TICKS(10));
        }
    }
    debugA("\r");  
}

// wifi scan 
void do_wifi_scan_new_tbt(){
    debugA("wifi_scan start...");
    // WiFi.scanNetworks will return the number of networks found
    int n = WiFi.scanNetworks();
    debugAl("\r\r\r\rscan done: ");
    if (n == 0) {
        debugAl("no networks found\r");
    } else {
        debugAl("%d",n);
        debugAl(" networks found\r");
        for (int i = 0; i < n; ++i) {
            // Print SSID and RSSI for each network found
            debugAl("%d",i + 1);
            debugAl(": ");
            debugAl("%s",WiFi.SSID(i));
            debugAl(" (");
            debugAl("%d",WiFi.RSSI(i));
            debugAl(")");
            debugAl("%s\r",(WiFi.encryptionType(i) == WIFI_AUTH_OPEN)?" ":"*");
            vTaskDelay(pdMS_TO_TICKS(10));
        }
    }
    debugAl("\r");
  
}

void WiFi_Upstream_Setup(){  
    Serial.println("WiFi_Upstream_Setup starting... ");
    Serial.println("Trying to connect to wifi upstream network ");
    Serial.printf("%s",ESP_Config.ssid.c_str());
//    WiFi.mode(WIFI_AP_STA);
    WiFi.mode(WIFI_STA);
    
    Serial.printf("\rUsing SSID=%s - PASSWD=%s\r\n", ESP_Config.ssid.c_str(), ESP_Config.password.c_str());
    WiFi.begin(ESP_Config.ssid.c_str(), ESP_Config.password.c_str());
    if (!ESP_Config.dhcp){
		WiFi.config(IPAddress(ESP_Config.IP[0],ESP_Config.IP[1],ESP_Config.IP[2],ESP_Config.IP[3] ),  IPAddress(ESP_Config.Gateway[0],ESP_Config.Gateway[1],ESP_Config.Gateway[2],ESP_Config.Gateway[3] ) , IPAddress(ESP_Config.Netmask[0],ESP_Config.Netmask[1],ESP_Config.Netmask[2],ESP_Config.Netmask[3] ), IPAddress(ESP_Config.Dns[0],ESP_Config.Dns[1],ESP_Config.Dns[2],ESP_Config.Dns[3]) );
	};

    int32_t wifi_timeout=60;   // 30 sec timeout
    while ( (WiFi.status() != WL_CONNECTED) && (wifi_timeout > 0) ){
        wifi_timeout--;
        vTaskDelay(pdMS_TO_TICKS(500));
        debugAl(".");
        };
    if(wifi_timeout > 0){
        Serial.println("\rWiFi upstream network connected!"); 
        Serial.printf(" IP address: %d.%d.%d.%d \r", WiFi.localIP()[0], WiFi.localIP()[1], WiFi.localIP()[2], WiFi.localIP()[3] );
        wifi_unavailable = 0;
        }
    else{
        Serial.print("\rWiFi_Upstream_Setup: Unable to connect WiFi ! \r");
        wifi_unavailable = 1 ;
        };
}


void WiFi_AP_Setup(){
    //********************************* ESP32 AP startup *****************************************
    String _apName = WiFi.macAddress();
    _apName = "ESP32-" + _apName ;
    _apName.replace(":","");

    Serial.printf("\rSetting WIFI_AP mode...\n");
    WiFi.mode(WIFI_AP);
    Serial.printf("\rStarting softAP: ssid_ap=%s, password_ap=%s\n\r", _apName.c_str(), ACCESS_POINT_PASSWORD);
    // WiFi.softAP(_apName.c_str(), ACCESS_POINT_PASSWORD );
    WiFi.softAP(_apName.c_str() );
    vTaskDelay(pdMS_TO_TICKS(500));
    WiFi.softAPConfig(ap_local_ip, ap_gateway, ap_subnet);  // set our local default softAP address
    vTaskDelay(pdMS_TO_TICKS(100));
    Serial.printf("ESP32 softAP started: ssid=%s \r\n",_apName.c_str());
    IPAddress myIP = WiFi.softAPIP();
    Serial.print("AP IP address: ");
    Serial.println(myIP);
}



// Following routine try to manage upstream network connection dynamics... if upstream is available no problems... go on
// If wifi upstream is not available try to reconnect... if after few trial it appears that upstream do not
// connect just stop trying for next 200 trials... then try again just in case wifi reappears..
//

void NetworkUpstreamMonitor(){   
  // while in declared standalone mode or initial AdminEnabled mode no wifi upstream is allowed....
  if( standalone  ) return ; 
  debugA("NetworkUpstreamMonitor: check WiFi connection.... \r");

#ifdef WIFI_UPSTREAM                    // these function just appliesfor wifiupstream mode
  int32_t wifi_timeout=10;   // 10 sec timeout

  if (wifi_unavailable ++ > 5 ) {  // just count how many times we tried without success to upstream restore...
     if( wifi_unavailable < 10 ){
//          debugA("NetworkUpstreamMonitor: WiFi down by auto-management (%d) \r", wifi_unavailable);
          last_wifi_tag = wifi_unavailable ;
          return ;  // if it is to much it means that wifi upstream is not available at all... so stop trying!
         };
     // try to reconnect just in case wifi network re-appeared....
//     debugA("NetworkUpstreamMonitor: trying to recover wifi upstream connection (%d) \r", wifi_unavailable);
     wifi_unavailable = 4 ; // let's try if just in case wifi is again available....
     } ;

   if (WiFi.status() != WL_CONNECTED){
    if(syslog_initialized ) {  // firs of all just in case mark unavailable syslog service
        syslog_available = false ;
        };
//     debugA("NetworkUpstreamMonitor: WiFi is disconnected... try to reconnect.... (%d) \r", wifi_unavailable);
     WiFi.disconnect();
     vTaskDelay(pdMS_TO_TICKS(1000));
     WiFi.reconnect();
     }
   else {                   // wifi network connection restored...
     wifi_unavailable = 0 ;  
     //     debugA("NetworkUpstreamMonitor: WiFi is connected... (%d) \r", wifi_unavailable);
     if(syslog_available && last_wifi_tag > 0 ) syslog.logf(LOG_INFO, "WiFi Upstream recovery completed at %s -  was down at %s", (UTC.dateTime( ezt::now(), "d/n/Y H:i:s.v T")).c_str() , (UTC.dateTime(last_wifi_up, "d/n/Y H:i:s.v T")).c_str());       
     last_wifi_up = ezt::now() ;
     last_wifi_tag = wifi_unavailable ;
     return ;
     } ;
  
  
  while ( (WiFi.status() != WL_CONNECTED) && (wifi_timeout > 0) ){
    wifi_timeout--;
    vTaskDelay(pdMS_TO_TICKS(500));
    debugAl(".");
    };
  if(wifi_timeout > 0){
//     debugA("NetworkUpstreamMonitor: WiFi upstream network recoverd!\r");
     debugA(" IP address: "); Serial.println(WiFi.localIP());     
     wifi_unavailable = 0 ;
     if(syslog_available) syslog.logf(LOG_INFO, "WiFi Upstream recovery completed at %s -  was down at %s", (UTC.dateTime(ezt::now(), "d/n/Y H:i:s.v T")).c_str() , (UTC.dateTime(last_wifi_up, "d/n/Y H:i:s.v T")).c_str());  
     last_wifi_up = ezt::now() ;
     last_wifi_tag = wifi_unavailable ; 
     if(syslog_initialized ) { 
        syslog_available = true ;
        };
     }
  else{
//     debugA("NetworkUpstreamMonitor: Unable to connect WiFi (%d) ! \r", wifi_unavailable);
     };

#else
//     debugA("NetworkUpstreamMonitor: Unable to connect WiFi ! Using Ethernet upstream only... please connected ethernet cable ......\r");
#endif
}

void disableWiFi(){
//    adc_power_off();
    WiFi.disconnect(true);  // Disconnect from the network
    WiFi.mode(WIFI_OFF);    // Switch WiFi off
}

void enableWiFi(){
 //   adc_power_on();
    WiFi.disconnect(false);  // Reconnect the network
    WiFi.mode(WIFI_STA);    // Switch WiFi off
 
    Serial.println("START WIFI");
//    WiFi.begin(STA_SSID, STA_PASS);
 
//    while (WiFi.status() != WL_CONNECTED) {
//        delay(500);
//        Serial.print(".");
//    }
 
//    Serial.println("");
//    Serial.println("WiFi connected");
//    Serial.println("IP address: ");
//    Serial.println(WiFi.localIP());
}
