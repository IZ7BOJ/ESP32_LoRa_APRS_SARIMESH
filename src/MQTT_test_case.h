//MQTT_test_case.h
#ifndef MQTT_SUPPORT_H
#define MQTT_SUPPORT_H

#include "rdbg.h"
#include <Arduino.h>
#include <PubSubClient.h>
#include "FRAM.h"
#include <Syslog.h>

#ifdef WIFI_UPSTREAM
extern WiFiClient HMQ_Client;  // HMQ MQTT client instance
extern PubSubClient mqttClient; 
#endif

extern uint8_t HMQ_cntr ; 
extern FRAM fram;
extern Syslog syslog ;
extern bool syslog_initialized ;
extern volatile bool syslog_available ;
extern String SyslogDeviceName ;
extern volatile uint8_t RebootTag ;           // RebootTag 
extern char *HMQ_mqttServer ;
extern int HMQ_mqttPort ;
extern volatile bool Admin_Mode  ;         //   

void loop_mqtt();
void Adafruit_MQTT_connect();
void HMQ_setupMQTT();
bool HMQ_reconnect();
void HMQ_callback(char* topic, byte* payload, unsigned int length);
void HMQ_pong(void);
void HMQ_loop_MQTT();
String msg_dashboard(void);           // create a json formatted stats dump for transfer via mqtt
void write_event_to_log(String my_record);
void HMQ_dashboard(void) ;

#endif
