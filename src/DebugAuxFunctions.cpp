//DebugAuxFunctions.cpp

#include "rdbg.h"

void processCmdRemoteDebug() ;
extern String  SW_Revision ;
extern volatile uint8_t  hw_arch;   // 0= undefined ( get values from GUI) 1=SARIMESH , 2= TTGO , 3 = Heltec

void SerialDebug_Setup(void){
  //******************** initilize SerialDebug  start ***************************
    Serial.begin(115200); // Can change it to 115200, if you want use debugIsr* macros
    delay(500); // Wait a time
    // SerialDebug starts disabled and it only is enabled if have data avaliable in Serial
    // Good to reduce overheads.
    // if You want debug, just press any key and enter in monitor serial
    // Note: all debug in setup must be debugA (always), due it is disabled now.
    Serial.println("SerialDebug_Setup (msg-01): starting serial connection..."); // To not stay in end of dirty chars in boot

#ifdef FORCE_SERIAL_DEBUG    
    debugSetProfiler(true);
#endif    
    debugA("**** Setup: initializing ...");

  //******************** initilize SerialDebug  end ***************************

  return;
}

#ifndef FORCE_SERIAL_DEBUG

void RemoteDebug_Setup(void){
#ifndef DEBUG_DISABLED // Initialize RemoteDebug  
  String _rdbName = WiFi.macAddress();
  _rdbName = "ESP32-" + _rdbName ;
  _rdbName.replace(":","");
  Debug.begin(_rdbName.c_str());   // Initialize the RemoteDebug WiFi server
  Debug.setPassword("esp32");      // Password on telnet connection ?
  Debug.setResetCmdEnabled(false); // Enable the reset command
  Debug.showProfiler(false);       // Profiler (Good to measure times, to optimize codes)
  Debug.showColors(true);          // Colors
  Debug.setSerialEnabled(true);    // All messages too send to serial too, and can be see in serial monitor

  // Project commands
  String helpCmd = "";
  helpCmd.concat("i2c_scan\n");   
  helpCmd.concat("wifi_scan\n"); 
//  helpCmd.concat("show_temperature\n");   
  helpCmd.concat("show_gps\n");   
  helpCmd.concat("show_dashboard\n");
  helpCmd.concat("show_stats\n");  
  helpCmd.concat("show_events\n");
  helpCmd.concat("show_log\n");    
  helpCmd.concat("show_config\n");   
  helpCmd.concat("fram_log_set\n");   
  helpCmd.concat("fram_log_reset\n"); 
  helpCmd.concat("fram_dump\n");
  helpCmd.concat("config_enable\n");
  helpCmd.concat("config_disable\n");
  helpCmd.concat("reboot\n");
//  helpCmd.concat("help\n");
//  helpCmd.concat("selftest_start\n");   
//  helpCmd.concat("selftest_stop\n");  
//  helpCmd.concat("area_restart\n"); 
//  helpCmd.concat("send_magic\n"); 
//  helpCmd.concat("foo - foo");
  Debug.setHelpProjectsCmds(helpCmd);
  Debug.setCallBackProjectCmds(&processCmdRemoteDebug);
 
  // End of setup - show our banner now onto the serial and remote consolle...
  debugA("\r\nsetup(): Initialize RemoteDebug server (telnet or web socket) .... \r");
  debugA("ESP32_LoRa_Radiolib_Beacon Vr. %s (hw_arch = %d)\r", SW_Revision.c_str(), hw_arch ); 
  debugA("* Arduino RemoteDebug Library: WiFI connected. IP address: %d.%d.%d.%d \r",WiFi.localIP()[0], WiFi.localIP()[1], WiFi.localIP()[2], WiFi.localIP()[3] );
  debugA("* Please use the telnet client (telnet for Mac/Unix or putty and others for Windows)\r");
  debugA("* or the RemoteDebugApp (in browser: http://joaolopesf.net/remotedebugapp)\r");
  debugA("* Please try change debug level in client (telnet or web app), to see how it works\r");
  debugA("*\r");
#endif  
}
#endif


void Debug_Loop(void){
  // common Remote and Serial Debug handle
  // NOTE: if in inactive mode (until receive anything from serial),
  // it show only messages of always or errors level type
  // And the overhead during inactive mode is very much low

  debugHandle();  
}
