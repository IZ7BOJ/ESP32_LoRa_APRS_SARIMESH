// PortExpander8574_Support_Functions.ino

#include "PortExpander8574_Support_Functions.h"

// #ifdef PCF8574_MOD

void hard_reboot(void) {
   debugA("hard_reboot: ===> Rebooting...");
   if(has_FM24W256){
      RebootTag = 123 ;
      fram.write8(2015,RebootTag);
      };
   vTaskDelay(pdMS_TO_TICKS(2000));
   if(reboot_pin){
      pinMode(reboot_pin, OUTPUT);      // HW reboot cpu
      digitalWrite(reboot_pin, LOW);    
      } 
   else {
      ESP.restart();    
      };
   return ;
}


void check_button(){
   if(fp_button){  // check fp_button just in case
      uint8_t port_data ;
      if(has_PCF8574){
         if( !(xSemaphoreTake(i2c_mutex_v, ( TickType_t ) 2) )){ if(PE_debug) debugA("check_button: xSemaphoreTake FAILED: aborting "); return;  };
         port_data = pcf8574.digitalRead(P5);
         xSemaphoreGive(i2c_mutex_v);
         }
      else {   //  https://microcontrollerslab.com/adc-esp32-measuring-voltage-example/
         int ADC_VALUE = 0;
         ADC_VALUE = analogRead(fp_button);
         // if(PE_debug) 
//       Serial.printf("================>   ADC VALUE = %d \r\n", ADC_VALUE);
         if( (CPU_TYPE != "TTGO") && (CPU_TYPE != "HELTEC") ) {
            if(ADC_VALUE > 2048 ) port_data = HIGH ;
            }
         else{   
            if(ADC_VALUE < 2048 ) port_data = HIGH ;  // TTGO has reversed polariry for the button...
            };
         };
      
      if (port_data == HIGH){  // process button pressed
         if(wait_default_config) {  // asserted only during the dedicated time window just at the end of setup...
            // we must load factory default configuration now...
            write_event_to_log("========== Load Default Configuration by GUI =============");
            if (LoadConfig( true ) == false) Serial.print("Forced default configuration\r\n");
            Serial.println(">>>>>>>>>>> Now Rebooting... \r");
            web_server.send ( 200, "text/html", DebugWaitAndReload ); 
            vTaskDelay(pdMS_TO_TICKS(5000));
            if(reboot_pin){
               hard_reboot();   
               }
            else{
               ESP.restart();
                };  
            } ;

         // if(PE_debug) 
         debugA("check_button: KEY PRESSED port_data = %x  button_press = %d \r",port_data, button_press);
         button_press = button_press + 100;
         if(button_press > 6300) button_press = 0;
         if(button_press > 4800) { display_event = 2 ; }  // display reboot screen
         else if(button_press > 3300) display_event = 5 ;  // display enable WiFi screen
         else if(button_press > 1800) display_event = 4 ;  // display disable WiFi screen
         else if(button_press > 300) { display_event = 3 ; };  // display send beacon now screen
         }
      else{  // process button release
         // if(PE_debug) debugA("check_button: KEY RELEASED port_data = %x  button_press = %d \r",port_data, button_press);
         if (button_press >= 5000){   // test for long press > 2000 msecs
            //if(PE_debug) 
            debugA("check_button: long press detected  button_press = %d\r",button_press);
            if(PE_debug) debugA("check_button: hard_reboot requested..  do it in few seconds... display_event = 2\r");
            display_event = 2 ;  // display Hard Reboot screen
            vTaskDelay(pdMS_TO_TICKS(2000));
            hard_reboot();
            }
         else if( button_press > 3300){
            //  if(PE_debug) 
            debugA("check_button: request to enable WiFi screen display_event = 5 \r\r\n");
            display_event = 5 ;  // display enable WiFi screen
            ESP_Config.wifi_enable = true;
            ESP_Config.standalone = false;
            WriteConfig();
            // DisplayConfig(); // display configuration data
            vTaskDelay(pdMS_TO_TICKS(2000)); 
            }
         else if( button_press > 1800){
            //if(PE_debug) 
            debugA("check_button: request to disable WiFi screen display_event = 4\r\r\n");
            display_event = 4 ;  // display disable WiFi screen
            ESP_Config.wifi_enable = false;
            ESP_Config.standalone = true;
            WriteConfig();
            // DisplayConfig(); // display configuration data         
            vTaskDelay(pdMS_TO_TICKS(2000));
            }
         else if( button_press > 300){
            if( wait_default_config  ) {
   
               }
            else{
#ifdef LORA_FUNCTIONS_ENABLE          
               // if(APRS_debug) 
               debugA("check_button: request to send APRS beacon immediately... display_event = 3\r");
               display_event = 3 ;  // display send beacon now screen
               requirePeriodicBeacon = true ; 
#endif         
               };   
            };     
         button_press = 0;  // reset button_press                
         };
      //    delay (50); 
      }; // end of fp_button
   return ;
}    // end of check_button

//******************************************************
#ifdef PWRS_MOD  

void V5_Out_enable(){
   Serial.printf("V5_Out_enable ... \r\n" );
   pcf8574.digitalWrite(P0, LOW);  // LOW to switch on the led
}

void V5_Out_disable(){
   Serial.printf("V5_Out_disable ... \r\n" );
   pcf8574.digitalWrite(P0, HIGH);  // LOW to switch on the led
}

void V24_1_Out_enable(){
   Serial.printf("V24_1_Out_enable ... \r\n" );
   pcf8574.digitalWrite(P1, LOW);  // LOW to switch on the led
}

void V24_1_Out_disable(){
   Serial.printf("V24_1_Out_disable ... \r\n" );
   pcf8574.digitalWrite(P1, HIGH);  // LOW to switch on the led
}

void V24_2_Out_enable(){
   Serial.printf("V24_2_Out_enable ... \r\n" );
   pcf8574.digitalWrite(P2, LOW);  // LOW to switch on the led
}

void V24_2_Out_disable(){
   Serial.printf("V24_2_Out_disable ... \r\n" );
   pcf8574.digitalWrite(P2, HIGH);  // LOW to switch on the led
}

void V24_3_Out_enable(){
   Serial.printf("V24_3_Out_enable ... \r\n" );
   pcf8574.digitalWrite(P3, LOW);  // LOW to switch on the led
}

void V24_3_Out_disable(){
   Serial.printf("V24_3_Out_disable ... \r\n" );
   pcf8574.digitalWrite(P3, HIGH);  // LOW to switch on the led
}

#endif


//*****************************************************

void buzzer_disable(){
#ifndef PWRS_MOD
   if(buzzer_pin){
      if(PE_debug) debugA("buzzer_disable\r");
      digitalWrite(buzzer_pin, LOW);    // testing buzzer attached to controller pin
      };
#else
   Serial.printf("buzzer_disable\r\n");
   digitalWrite(BUZZER, LOW);    // testing buzzer attached to controller pin
   Serial.printf("buzzer_disable\r\n");
#endif
  return ;  
}

void buzzer_enable(){
#ifndef PWRS_MOD  
   if(buzzer_pin){
      if(PE_debug) debugA("buzzer_enable\r");
      digitalWrite(buzzer_pin, HIGH);    // testing buzzer attached to controller pin
      };
 #else
   Serial.printf("buzzer_enable\r\n");
   digitalWrite(BUZZER, HIGH);    // testing buzzer attached to controller pin
   Serial.printf("buzzer_enable\r\n");
 #endif   
   return;   
}


void soundBuzzer(){
   // Serial.printf("soundBuzzer..... start ="); Serial.println(UTC.dateTime("l, d-M-y H:i:s.v T"));
   if( !I2C_Busy){
      I2C_Busy = true;
      buzzer_enable();
      vTaskDelay(pdMS_TO_TICKS(50));
      buzzer_disable();
      I2C_Busy = false;
      //        Serial.printf("soundBuzzer..... stop ="); Serial.println(UTC.dateTime("l, d-M-y H:i:s.v T"));
      }
   else{
      if(PE_debug) debugA("soundBuzzer..... skipping sound I2C_Busy = %d !!!!!!\r", I2C_Busy);       
      };  
   return;          
}

void soundBuzzer_long(){
   if( !I2C_Busy){
      I2C_Busy = true;
      buzzer_enable();
      vTaskDelay(pdMS_TO_TICKS(100));
      buzzer_disable();
      I2C_Busy = false;
      }
   else{
      if(PE_debug) debugA("skipping sound I2C_Busy = %d !!!!!!\r", I2C_Busy);       
      };
   return ;     
}

// red led indicates where the LoRa radio is actively transmitting...
void RED_LED_enable(){
   //  if(PE_debug) debugA("has_PCF8574=%d  PE_debug=%d\n", has_PCF8574, PE_debug );
   //if(syslog_available)  syslog.log(LOG_INFO, "RED_LED_ON");
   if( has_PCF8574){
      if( !(xSemaphoreTake(i2c_mutex_v, ( TickType_t ) 2) )){ if(PE_debug) debugA("RED_LED_enable: xSemaphoreTake FAILED: aborting "); return;  };
#ifndef PWRS_MOD
      pcf8574.digitalWrite(P7, LOW);  // LOW to switch on the led
#else
      pcf8574.digitalWrite(P6, LOW);  // LOW to switch on the led
#endif
      xSemaphoreGive(i2c_mutex_v);
      }
   else{
      if(red_led){   
         digitalWrite(red_led, HIGH);    // testing buzzer attached to controller pin
         };
      };
   if(PE_debug) debugA("RED_LED_enable at %d\r",millis());
   last_red_time = millis();
   return ;
}

void RED_LED_disable() {
   // if(PE_debug) debugA("has_PCF8574=%d  PE_debug=%d\n", has_PCF8574, PE_debug );
   // if(syslog_available) syslog.log(LOG_INFO, "RED_LED_OFF");
   if( has_PCF8574 ){
      if( !(xSemaphoreTake(i2c_mutex_v, ( TickType_t ) 2) )){ if(PE_debug) debugA("WriteConfig: xSemaphoreTake FAILED: aborting "); return;  };
#ifndef PWRS_MOD
      pcf8574.digitalWrite(P7, HIGH);
#else
      pcf8574.digitalWrite(P6, HIGH);
#endif      
      xSemaphoreGive(i2c_mutex_v);      
      }
   else{ 
      if(red_led){     
         digitalWrite(red_led, LOW);    // testing buzzer attached to controller pin
         };
      };
   if(PE_debug) debugA("RED_LED_disable at %d --> delta_red_time = %d\r",millis(), millis() - last_red_time );
#ifdef LORA_FUNCTIONS_ENABLE 
   if(LoRa_initialized){
      LoRa_OnAirTime=(4*LoRa_OnAirTime + millis() - last_red_time )/5 ;
      LoRa_OnAirTime_Tot = LoRa_OnAirTime_Tot + millis() - last_red_time ;
      if(LoRa_debug) debugA("LoRa_Stats:-----> TimeOnAir = %d\n", millis() - last_red_time );
      };
#endif //  LORA_FUNCTIONS_ENABLE         
   last_red_time = millis();
   return ;  
}


// green led provides a processor load indicator; on period indicates howlong the processor is in the active loop execution part; 
// it is sampled nearly every second; a printout on the serial console is produced if the measured time value > 900 msec or  < 200 msec
// it is assumed that this is an indication of ptocessor overload....
// the sampling criteria is as follow:  a record is keept for times when any loop run starts.. when there is a pass at least 1000 msecs
// after the last trigger a new green led triggers the ON period measurement.This is equivalent of a circa at least 1 sec sampling time

void GREEN_LED_enable(){
   //   if(PE_debug) debugA("GREEN_LED_enable in\r" );
   //   if(syslog_available) syslog.log(LOG_INFO, "GREEN_LED_ON");    
   if( has_PCF8574){
      if( !(xSemaphoreTake(i2c_mutex_v, ( TickType_t ) 2) )){ if(PE_debug) debugA("WriteConfig: xSemaphoreTake FAILED: aborting "); return;  };
#ifndef PWRS_MOD
      pcf8574.digitalWrite(P6, LOW);  // LOW to switch on the led
#else
      pcf8574.digitalWrite(P7, LOW);  // LOW to switch on the led
#endif        
      xSemaphoreGive(i2c_mutex_v);
      }
   else{
      if(green_led){      
         digitalWrite(GREEN_LED, HIGH);    // testing buzzer attached to controller pin
         };
      };
   // if(PE_debug) debugA("GREEN_LED_enable out\r" );
   uint32_t tresh =  millis() - last_green_time;
   if(PE_debug) if (tresh > 900) debugA("GREEN_LED OFF TOO LONG time at %d --> delta_green_time = %d\r",millis(), tresh);
   if(PE_debug)if (tresh < 200)  debugA("GREEN_LED OFF TOO SHORT time at %d --> delta_green_time = %d\r",millis(), tresh);
   last_green_time = millis();
   return ;  
}

void GREEN_LED_disable(){
   //      if(PE_debug) debugA("GREEN_LED_disable \r" );
   //    if(syslog_available) syslog.log(LOG_INFO, "GREEN_LED_OFF");      
   if( has_PCF8574 ){
      if( !(xSemaphoreTake(i2c_mutex_v, ( TickType_t ) 2) )){ if(PE_debug) debugA("WriteConfig: xSemaphoreTake FAILED: aborting "); return;  };
#ifndef PWRS_MOD
      pcf8574.digitalWrite(P6, HIGH);
#else
      pcf8574.digitalWrite(P7, HIGH);
#endif         
      xSemaphoreGive(i2c_mutex_v);
      }
   else{ 
      if(green_led){        
         digitalWrite(green_led, LOW);    // testing buzzer attached to controller pin
         };
      };
   //     if(PE_debug) Serial.printf("GREEN_LED_disable \r");
   uint32_t tresh =  millis() - last_green_time;
   if(PE_debug) if (tresh > 900) debugA("GREEN_LED ON TOO LONG time at millis=%d --> delta_green_time = %d\r",millis(), tresh );
   if(PE_debug) if (tresh < 200) debugA("GREEN_LED ON TOO SHORT time at at millis=%d --> delta_green_time = %d\r",millis(), tresh );
   last_green_time = millis();
   return;  
}

void red_flash(){
   RED_LED_enable();
   vTaskDelay(pdMS_TO_TICKS(100));
   RED_LED_disable();
   return ;
}

void red_flash_short(){
   RED_LED_enable();
   vTaskDelay(pdMS_TO_TICKS(100));
   RED_LED_disable();
   return ;
}

void green_flash() {
   GREEN_LED_enable();
   vTaskDelay(pdMS_TO_TICKS(1000));
   GREEN_LED_disable();
   return ;
}

void green_flash_short(){
   GREEN_LED_enable();
   vTaskDelay(pdMS_TO_TICKS(100));
   GREEN_LED_disable();
   return ;
}

void green_red_flash(){
   GREEN_LED_enable();RED_LED_enable();
   vTaskDelay(pdMS_TO_TICKS(100));
   GREEN_LED_disable();RED_LED_disable();
   RED_LED_enable();RED_LED_enable();
   vTaskDelay(pdMS_TO_TICKS(100));
   RED_LED_disable();RED_LED_disable();
   return ;  
}

//#endif       // PCF8574_MOD
