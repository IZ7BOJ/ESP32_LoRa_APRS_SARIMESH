// https://hackaday.io/project/7330-beautiful-fonts-on-ili9341-tft-displays
// https://github.com/Bodmer/TFT_eSPI/blob/master/examples/Smooth%20Fonts/FLASH_Array/Print_Smooth_Font/Print_Smooth_Font.ino
// https://processing.org/download/
// https://github.com/Bodmer/TFT_eSPI/blob/master/Tools/Create_Smooth_Font/Create_font/Create_font.pde
// https://tomeko.net/online_tools/file_to_hex.php?lang=en
// https://github.com/Bodmer/TFT_eSPI/blob/master/examples/Smooth%20Fonts/FLASH_Array/Print_Smooth_Font/Print_Smooth_Font.ino
// https://hackaday.io/project/7330-beautiful-fonts-on-ili9341-tft-displays
// https://github.com/Bodmer/TFT_eSPI/blob/master/examples/480%20x%20320/TFT_Padding_demo/TFT_Padding_demo.ino
// https://github.com/greiman/SSD1306Ascii   usata per SSD1306  no buffering 

// Master configuration directives: need to be customized depending on HW/SW configuratio of the device
#include "Master_Config.h"

#include <Arduino.h>


extern uint32_t last_display_update ;
 
String StatusLine = String("WiFi=ON   GPS=3D-FIX"); 



#ifdef DEBUG_DISABLED
#define debugAl(fmt, ...) Serial.printf(fmt, ##__VA_ARGS__)
#define debugA(fmt, ...) Serial.printf(fmt, ##__VA_ARGS__)
#else
#ifndef FORCE_SERIAL_DEBUG
#include "RemoteDebug.h"     //https://github.com/JoaoLopesF/RemoteDebug
extern RemoteDebug Debug ;
#define debugA_loc(fmt, ...) if (Debug.isActive(Debug.ANY))    Debug.printf(fmt, ##__VA_ARGS__)
#define debugAl(fmt, ...) debugA_loc(fmt, ##__VA_ARGS__)
#else
#include "SerialDebug.h"     //https://github.com/JoaoLopesF/RemoteDebug
#define debugA_loc(fmt, ...) debugA(fmt, ##__VA_ARGS__)
#define debugAl(fmt, ...) Serial.printf(fmt, ##__VA_ARGS__)
#endif
#endif





extern float lora_snr ;
extern float lora_rssi;
extern float lora_frequencyError ;
extern String lora_payload ;
extern volatile bool BT_KISS_Mode ;
extern volatile bool Serial_KISS_Mode ;
extern volatile bool TCP_KISS_Mode ;
extern volatile uint32_t Kiss_rx_packets ;
extern volatile uint32_t Kiss_tx_packets ;

extern volatile bool Tracker_Mode ;
extern volatile bool IoT_enable ;
extern volatile bool iGate_Mode  ;         //   
extern volatile bool Beacon_Mode  ;         //   
extern volatile bool Admin_Mode  ;         //   
extern volatile bool is_at_defaults  ;


//Virtual display storage
String ui0_payload ;
String ui1_payload ;
String ui2_payload ;
String ui3_payload ;
String ui4_payload ;
String ui5_payload ;          // only available with TFT display
extern volatile uint8_t display_event ;  // triggers real display update
extern volatile uint16_t AdminTimeOutCounter ;

#include <Adafruit_ST77xx.h> 
#include <Adafruit_ST7735.h> // Hardware-specific library for ST7735^M
#include <Adafruit_ST7789.h> // Hardware-specific library ffor ST7789^M

extern Adafruit_ST7789 tft ;

//#include "SSD1306Ascii.h"
//#include "SSD1306AsciiWire.h"
//extern SSD1306AsciiWire display;
// #include "ezTime_loc.h"
// extern time_t ezt::now  ;

extern volatile uint8_t  oled_orient;



#ifdef OLED_MOD
  #ifdef USE_SSD1306_ASCII_LIB
    #include "SSD1306Ascii.h"
    #include "SSD1306AsciiWire.h"
    extern SSD1306AsciiWire display;
  #else                                // OLED display support via adafruit OLED library
    #include <Adafruit_GFX.h>    // Core graphics library
    #include <Adafruit_SSD1306.h>
    extern Adafruit_SSD1306 display ;

#include <Fonts/FreeMonoBoldOblique12pt7b.h>
#include <Fonts/FreeMono9pt7b.h>
#include "FONTS/Basic_Comical_Regular_NC12pt7b.h"
#include "FONTS/Basic_Comical_Regular_NC11pt7b.h"
#include "FONTS/Basic_Comical_Regular_NC10pt7b.h"
#include "FONTS/Basic_Comical_Regular_NC9pt7b.h"
#include "FONTS/Basic_Comical_Regular_NC8pt7b.h"
#include "FONTS/Basic_Comical_Regular_NC7pt7b.h"
#include "FONTS/Basic_Comical_Regular_NC6pt7b.h"


        //  #include "OLED_test_case.h"
  #endif
#endif   // OLED_MOD


extern String LastFunction ;
extern uint32_t activity_time ;
extern volatile float ppm_runtime;
extern String RX_Last_Spot_level ;
extern String RX_Last_Spot_payload ;
extern String RX_Last_Path_Report ;
extern volatile uint32_t IPC_lost_msgs ;
extern volatile uint32_t LoRa_lost_packets ;
extern volatile uint32_t LoRa_rx_packets ;
extern volatile uint32_t LoRa_tx_packets ;
extern volatile uint32_t AprsIS_rx_packets ;
extern volatile uint32_t AprsIS_tx_packets ;
extern volatile uint32_t AprsIS_relayed_packets ;
extern volatile uint32_t LoRa_rx_AX25_packets ;
extern volatile uint32_t LoRa_tx_AX25_packets ;
extern volatile uint32_t LoRa_rx_OEStyle_packets ;
extern volatile uint32_t LoRa_tx_OEStyle_packets ;
extern volatile uint32_t LoRa_rx_native_packets ;
extern volatile uint32_t LoRa_tx_native_packets ;
extern volatile uint32_t LoRa_ReSched_packets;
extern volatile uint32_t LoRa_CAD_errors;
extern volatile float ChanCong , LoRa_ENL;
extern volatile bool LoRa_initialized ;
extern bool wifi_enable ; 
extern bool wifi_disable ; 


extern volatile uint32_t AprsIS_dropped_packets ;
extern volatile uint32_t LoRa_CRC_errored_packets;
extern volatile uint32_t LoRa_CRC_errored_packets_BCN;

extern volatile uint32_t LoRa_UMN_errored_packets;

extern volatile uint32_t LoRa_OnAirTime ;
extern volatile float LoRa_DutyCycle  ;
extern volatile uint32_t LoRa_OnAirTime_Tot ;

extern String SW_Revision ; 

extern volatile float cpu_temp ;
extern String lat_now,lon_now;
extern volatile int op_status;

extern volatile int op_status;
extern volatile int32_t time_jitter ;

extern volatile bool I2C_Busy ;
extern SemaphoreHandle_t i2c_mutex_v;


extern volatile bool has_PCF8574 ;              // autodiscovery
extern volatile bool has_DS3231 ;
extern volatile bool has_DS3231_eeprom ;
extern volatile bool has_Si5351 ;
extern volatile bool has_SSD1306 ;
extern volatile bool has_SSD1306 ;
extern volatile bool has_BME280 ;
extern volatile bool has_Si7021 ;
extern volatile bool has_FM24W256 ;
extern bool has_ST77XX ;
extern bool mqtt_enable ;


//String lora_payload ;
extern volatile bool EnableRfToIs;
extern volatile bool EnableIsToRf;
extern volatile bool EnableRepeater ;
extern volatile bool BT_KISS_Mode ;
extern volatile bool Serial_KISS_Mode ;
extern volatile bool TCP_KISS_Mode ;

extern uint32_t activity_start ;

extern  int wifi_unavailable ;

#include "ezTime_loc.h"
extern Timezone UTC;
extern time_t now;

#include <TinyGPS++.h>
extern TinyGPSPlus gps;

void display_ssd1306_man(void);
void display_st7735_man(void) ;



//#include <fonts/Verdana12_bold.h>


void StatusLineUpdate(void){
     String wifi_status = "OFF" ;  String gps_st = String("      GPS=") + String("down") ;
     if(wifi_enable) wifi_status = "ON " ; 
     if(op_status == 3 ) gps_st = String("   GPS=") + String("3D-FIX") ;  
     StatusLine = String("WiFi=") + String(wifi_status) +  String(gps_st) ; 
}

void LocalDisplayUpdate(void){   // run from main loop to update the local displays
    //  local display management handler to draw display screens
    if (last_display_update !=  ezt::now()){             // just perform this check on start of seconds
       StatusLineUpdate();
       if( has_SSD1306 ) display_ssd1306_man();
       if( has_ST77XX ) display_st7735_man();

       // treat display_events 
       if(display_event == 8 ) {
         display_event = 8 ; // let's display please reboot message forever...
         }   
       if((display_event == 4 ) || (display_event == 5 )) {
         display_event = 8 ; // let's display "please reboot manually" message
         } 
       else if(display_event > 2 ) {
         display_event = 1 ; // let's restore the standard screen after a transient event display
         }              
       else if(display_event == 2 ) {
         display_event = 8 ; // let's display "please reboot"  message
         } 
       else {
         display_event = 0 ;  // do nothing
         };
       last_display_update= ezt::now(); 
       };
}

void display_ssd1306_man(void){  //  to  draw  SSD1306 local display

if( !(xSemaphoreTake(i2c_mutex_v, ( TickType_t ) 2) )){ 
    debugA("display_ssd1306_man: xSemaphoreTake FAILED: aborting display update"); 
    return;  };
    
#ifndef USE_SSD1306_ASCII_LIB


#ifdef OLED_MOD 

if((has_SSD1306) && (display_event == 1 )){   // this display has only 5 text lines
     if(( ! BT_KISS_Mode && ! Serial_KISS_Mode && ! TCP_KISS_Mode) &&(LoRa_initialized)){  // LoRa iGate/Tracker normal operation
       if( LoRa_rx_packets > 0 ){        
          String line_0 = String("RSSI:") + String(lora_rssi) + String("dBm");
          String line_1 = String("SNR:") + String(lora_snr) + String("dB") + "  ERR:" + String(int(lora_frequencyError)) + String("Hz");
          String line_2 = ui3_payload;   // original packet source call 
          String line_3 = ui4_payload ;  // first node report
          String line_4 = ui5_payload ;  // last node report

          // display LoRa TX/RX operation on SSD1306 OLED display 
          display.clearDisplay();
          display.setFont(&Basic_Comical_Regular_NC6pt7b);
          display.setCursor(0, 11); display.print(line_0); 
          display.setCursor(0, 24); display.print(line_1); 
          display.setCursor(0, 37); display.print(line_2); 
//        display.setCursor(0, 50); display.print(line_3);
//        display.setCursor(0, 63); display.print(line_4);
          display.setCursor(0, 53); display.print("________________________"); 
          display.setCursor(10, 63); display.print(StatusLine);  
          // display.ssd1306_command(0xFD);
          // display.ssd1306_command(1);                
          display.display();
           }
        else {        
          // display waiting for LoRa Spot 
          display.clearDisplay(); display.display(); 
          display.setFont(&Basic_Comical_Regular_NC6pt7b);
          display.setCursor(45, 20); display.print("WAITING"); 
          display.setCursor(25, 33); display.print("FOR LORA SPOTS"); 
          display.setCursor(0, 54); display.print("__________________________"); 
          display.setCursor(10, 63); display.print(StatusLine); 
          display.display();
          };  // end of LoRa_rx_packets 
       }
     else if ((BT_KISS_Mode || Serial_KISS_Mode || TCP_KISS_Mode || iGate_Mode) && ( ! LoRa_initialized)){
         display.clearDisplay(); 
         display.setFont(&Basic_Comical_Regular_NC6pt7b);
         if(BT_KISS_Mode){ display.setCursor(0, 20); display.print("Bluetooth KISS Client Mode");};  
         if(Serial_KISS_Mode) { display.setCursor(0, 20); display.print("Serial KISS Client Mode");};
         if(iGate_Mode) { display.setCursor(35, 20); display.print("iGate Mode"); } ;
         if(Tracker_Mode) { display.setCursor(40, 20); display.print("Tracker Mode"); } ;                        
         display.setCursor(0, 40); display.print("Admin mode: ");display.setCursor(70, 40);  
         display.print( String(AdminTimeOutCounter));
         display.setCursor(85, 40); display.print("/ "); display.setCursor(90, 40);  
         display.print(String(AdminTimeOut));
         display.display();     
         }
     if(( BT_KISS_Mode || Serial_KISS_Mode || TCP_KISS_Mode) &&(LoRa_initialized)){  // KISS normal operation

         ui3_payload= String("LoRa RX=") + LoRa_rx_packets + String(" TX=") + LoRa_tx_packets ;
         ui4_payload= String("KISS RX=") + Kiss_rx_packets + String(" TX=") + Kiss_tx_packets ;
         if( LoRa_rx_packets > 0 ){        
            String line_0 = String("   LoRa TNC KISS_Mode");
            String line_1 = String("RSSI:") + String(lora_rssi) + String("dBm");
            String line_2 = String("SNR:") + String(lora_snr) + String("dB") + "  ERR:" + String(int(lora_frequencyError)) + String("Hz");
            String line_3 = ui3_payload;   // original packet source call 
            String line_4 = ui4_payload ;  // first node report
        //  String line_5 = ui5_payload ;  // last node report

          // display LoRa TX/RX operation on SSD1306 OLED display 
          display.clearDisplay(); 
          display.setFont(&Basic_Comical_Regular_NC6pt7b);
          display.setCursor(0, 11); display.print(line_1); 
          display.setCursor(0, 24); display.print(line_2); 
          display.setCursor(0, 37); display.print(line_3); 
//        display.setCursor(0, 50); display.print(line_3);
//        display.setCursor(0, 63); display.print(line_4);
          display.setCursor(0, 53); display.print("________________________"); 
          display.setCursor(10, 63); display.print(StatusLine);  
          // display.ssd1306_command(0xFD);
          // display.ssd1306_command(1);                
          display.display();
           }
        else {        
          // display waiting for LoRa Spot 
          display.clearDisplay(); display.display(); 
          display.setFont(&Basic_Comical_Regular_NC6pt7b);
          display.setCursor(45, 20); display.print("WAITING"); 
          display.setCursor(25, 33); display.print("FOR KISS/LORA PKTS"); 
          display.setCursor(0, 54); display.print("__________________________"); 
          display.setCursor(10, 63); display.print(StatusLine); 
          display.display();
          };  // end of LoRa_rx_packets 
       }  
     else if(Admin_Mode ){
         display.clearDisplay(); 
         display.setFont(&Basic_Comical_Regular_NC6pt7b);
         display.setCursor(35, 20); display.print("Admin Mode");  
         display.setCursor(0, 40); display.print("Admin mode: "); 
         display.setCursor(70, 40); display.print( String(AdminTimeOutCounter));
         display.setCursor(85, 40); display.print("/ "); display.setCursor(90, 40); 
         display.print( String(AdminTimeOut));
         display.display();     
         }                 
     else if ((BT_KISS_Mode || Serial_KISS_Mode || TCP_KISS_Mode) && ( LoRa_initialized)){
//         String line_date = String(UTC.dateTime(now(), "d/n/Y H:i:s.v")); 
         String line_0 ="";
         if(BT_KISS_Mode){ String line_0 = String("Bluetooth KISS Client Mode");}
         else if(Serial_KISS_Mode){ String line_0 = String("Serial KISS Client Mode"); }
         else if(TCP_KISS_Mode){ String line_0 = String("TCP KISS Client Mode"); };
                 
         String line_1 = String("RSSI:") + String(lora_rssi) + String("dBm");
         String line_2 = String("SNR:") + String(lora_snr) + String("dB") ;
         String line_3 = String("  ERR:") + String(int(lora_frequencyError)) + String("Hz");

         display.clearDisplay(); 
         display.setFont(&Basic_Comical_Regular_NC6pt7b);
         display.setCursor(0, 11); display.print(line_0);
//         display.setCursor(0, 24); display.print(line_date);        
         display.setCursor(0, 37); display.print("Last RX msg report");  
         display.setCursor(0, 50); display.print(line_1); 
         display.setCursor(0, 63); display.print(line_2); 
         display.display();           }

  }  // end of has_SSD1306 display_event =1

 
else if((has_SSD1306) && (display_event == 3 )){   // this display has only 5 text lines
         display.clearDisplay(); display.display();
         display.setFont(&Basic_Comical_Regular_NC6pt7b);
         display.setCursor(20, 30); display.print("SENDING BEACON NOW !"); 
         display.setCursor(0, 53); display.print("____________________"); 
         display.setCursor(10, 63); display.print(StatusLine); 
         display.display();  
         };   // end of has_SSD1306 display_event =3
//  };


#endif   // OLED_MOD



#else    //                    USE_SSD1306_ASCII_LIB


#ifdef OLED_MOD    // OK 20220320
if((has_SSD1306) &&  (display_event == 1 )){   // this display has only 5 text lines
     if(( ! BT_KISS_Mode && ! Serial_KISS_Mode && ! TCP_KISS_Mode ) &&(LoRa_initialized)){  // LoRa iGate/Tracker normal operation
       if( LoRa_rx_packets > 0 ){        
          String line_0 = String("RSSI:") + String(lora_rssi) + String("dBm");
          String line_1 = String("SNR:") + String(lora_snr) + String("dB") + "  ERR:" + String(int(lora_frequencyError)) + String("Hz");
          String line_2 = ui3_payload;   // original packet source call 
          String line_3 = ui4_payload ;  // first node report
          String line_4 = ui5_payload ;  // last node report

          // display LoRa TX/RX operation on SSD1306 OLED display 
          display.setFont(Verdana12_bold); 
          display.clear();
          display.setLetterSpacing(1);
          display.setRow(0); 
          display.println(line_0); 
          display.println(line_1);
          display.println(line_2);
          display.println(line_3);
          display.println(StatusLine);
           }
        else {        
          display.setFont(Verdana12_bold); display.clear(); display.setLetterSpacing(1); 
          if(is_at_defaults){
          display.println("     WARNING !!!    ");
          display.println("  DEFAULT CONFIG");
          display.println("   ");
          display.println("____________________");          
          }
          else{
            display.println("        WAITING    ");
            display.println("   FOR LORA SPOTS");
            display.println("   ");
            display.println("____________________");
          } 
          display.println(StatusLine);
          };  // end of LoRa_rx_packets 
       }
    else if(( BT_KISS_Mode || Serial_KISS_Mode || TCP_KISS_Mode ) &&(LoRa_initialized)){  // KISS normal operation
         ui3_payload= String("LoRa RX=") + LoRa_rx_packets + String(" TX=") + LoRa_tx_packets ;
    //     ui4_payload= String("KISS RX=") + Kiss_rx_packets + String(" TX=") + Kiss_tx_packets ;
         if( LoRa_rx_packets > 0 ){        
            String line_0 = String("RSSI:") + String(lora_rssi) + String("dBm");
            String line_1 = String("SNR:") + String(lora_snr) + String("dB") + "  ERR:" + String(int(lora_frequencyError)) + String("Hz");
            String line_2 = ui3_payload;   // original packet source call 
            String line_3 = ui4_payload ;  // first node report
        //  String line_4 = ui5_payload ;  // last node report

          // display LoRa TX/RX operation on SSD1306 OLED display 
          display.setFont(Verdana12_bold); 
          display.clear();
          display.setLetterSpacing(1);
          display.setRow(0); 
          display.println(line_0); 
          display.println(line_1);
          display.println(line_2);
          display.println(line_3);
          display.println(StatusLine);
           }
        else {        
          display.setFont(Verdana12_bold); display.clear(); display.setLetterSpacing(1);          
          display.println("           WAITING    ");
          display.println(" FOR KISS/LORA PKTS");
          display.println("   ");
          display.println("____________________");
          display.println(StatusLine);
          };  // end of LoRa_rx_packets 

    }
       
  }  // end of has_SSD1306 display_event =1

else if((has_SSD1306) && (display_event == 2 )){   // this display has only 5 text lines
    display.setFont(Verdana12_bold); display.clear(); display.setLetterSpacing(1);          
    display.println("REBOOT NOW");
}  // end of has_SSD1306 display_event = 2

else if((has_SSD1306) &&  (display_event == 3 )){   // this display has only 5 text lines
    display.setFont(Verdana12_bold); display.clear(); display.setLetterSpacing(1);          
    display.println("SENDING BEACON NOW !");
}  // end of has_SSD1306 display_event = 3


else if((has_SSD1306) &&  (display_event == 4 )){   // this display has only 5 text lines
    display.setFont(Verdana12_bold); display.clear(); display.setLetterSpacing(1);          
    display.println("DISABLE WIFI");
    display.println("(Needs Reboot)");    
//    display_event = 2 ;  // request reboot
} // end of has_SSD1306 display_event = 4

else if((has_SSD1306) &&  (display_event == 9 )){   // this display has only 5 text lines
    display.setFont(Verdana12_bold); display.clear(); display.setLetterSpacing(1);          
    display.println("APRSIS DOWN !");
    display.println("(retrying)");    
    vTaskDelay(pdMS_TO_TICKS(2000));    
} // end of has_SSD1306 display_event = 4

else if((has_SSD1306) && (display_event == 5 )){   // this display has only 5 text lines
    display.setFont(Verdana12_bold); display.clear(); display.setLetterSpacing(1);          
    display.println(" ENABLE WIFI");
    display.println("(Needs Reboot)");      
//    display_event = 2 ;  // request reboot

} // end of has_SSD1306 display_event = 5

else if((has_SSD1306) && (display_event == 6 )){   // this display has only 5 text lines
    display.setFont(Verdana12_bold); display.clear(); display.setLetterSpacing(1);          
    display.println("SENDING BEACON !");
} // end of has_SSD1306 display_event = 6
else if((has_SSD1306) && (display_event == 16 )){   // this display has only 5 text lines
    display.setFont(Verdana12_bold); display.clear(); display.setLetterSpacing(1);          
    display.println("INVALID BEACON");
    display.println(" PAYLOAD !");

} // end of has_SSD1306 display_event = 16



else if((has_SSD1306) && (display_event == 7 )){   // this display has only 5 text lines
    display.setFont(Verdana12_bold); display.clear(); display.setLetterSpacing(1);          
    display.println("Waiting for LoRa Spots");
    display.println(" ");
} // end of has_SSD1306 display_event = 7

else if((has_SSD1306) && (display_event == 8 )){   // this display has only 5 text lines
    display.setFont(Verdana12); display.clear(); display.setLetterSpacing(1);          
    display.println("Now.. Please Reboot");
    display.println("     manually");
    
}; // end of has_SSD1306 display_event = 8



#endif   // OLED_MOD

#endif

  xSemaphoreGive(i2c_mutex_v);

}


// https://github.com/Bodmer/TFT_eSPI    https://programmer.ink/think/color-setting-and-text-display-esp32-learning-tour-arduino-version.html
//  https://github.com/loboris/ESP32_TFT_library  alternativo
// https://github.com/m5stack/M5Stack/blob/master/examples/Advanced/Display/Free_Font_Demo/Free_Font_Demo.ino


void display_st7735_man(void){   // to draw local TFT color display
//  tft.setFont(&FreeMono9pt7b);
    
#ifdef TFT_MOD

if((has_ST77XX) && (display_event == 1 ) ){   // this display has 6 text lines
    
#ifdef USE_ADAFRUIT_TFT_LIB
//      tft.setCursor(0, 0);  tft.print("LoRa Beacon Vr.1.0 ");
      tft.setTextSize(1);

      if(( ! BT_KISS_Mode && ! Serial_KISS_Mode && ! TCP_KISS_Mode) &&(LoRa_initialized)){  // LoRa iGate/Tracker normal operation
         if( LoRa_rx_packets > 0 ){        
            String line_0 = String("GPS: ") + String(lat_now) + String(" ") + String(lon_now);
            String line_1 = String("RSSI:") + String(lora_rssi) + String("dBm");
            String line_2 = String("SNR:") + String(lora_snr) + String("dB") + "  ERR:" + String(int(lora_frequencyError)) + String("Hz");
            String line_3 = ui3_payload;   // original packet source call 
            String line_4 = ui4_payload ;  // first node report
            String line_5 = ui5_payload ;  // last node report

            // display LoRa TX/RX operation on ST77xx OLED display 
            tft.fillScreen(ST77XX_BLACK);
//          tft.setTextPadding(240); // Pad next print() text to full width to over-write old text
            tft.setCursor(0, 0); 
//          tft.setTextFont(GFXFF);
//          tft.setTextColor(TFT_WHITE, TFT_BLACK);
   
            tft.setTextColor(ST77XX_GREEN);
            tft.setCursor(0, 20); tft.print(line_0);
            tft.setCursor(0, 40); tft.print(line_1);
            tft.setCursor(0, 60); tft.print(line_2);
            tft.setCursor(0, 80); tft.print(line_3);
            tft.setTextColor(ST77XX_RED);
            tft.setCursor(0, 100); tft.print(line_4);
            tft.setTextColor(ST77XX_WHITE);
            tft.setCursor(0, 110); tft.print("____________________");
            tft.setTextColor(ST77XX_YELLOW);
           tft.setCursor(0, 130); tft.print(StatusLine);
           }
        else {
           tft.setTextSize(1);
          // display waiting for LoRa Spot 
          tft.fillScreen(ST77XX_BLACK);
          tft.setTextColor(ST77XX_WHITE);
          if(is_at_defaults){
              tft.setCursor(0, 40); tft.print("       WARNING !!!!");
              tft.setCursor(0, 70); tft.print("     DEFAULT CONFIG");
          }
          else{
              tft.setCursor(0, 40); tft.print("            WAITING");
              tft.setCursor(0, 70); tft.print("       FOR LORA SPOTS");
          };
          tft.setTextColor(ST77XX_WHITE);
          tft.setCursor(0, 110); tft.print("____________________");
          tft.setTextColor(ST77XX_YELLOW);
          tft.setCursor(0, 130); tft.print(StatusLine);
          };
       }
     else if ((BT_KISS_Mode || Serial_KISS_Mode || TCP_KISS_Mode|| iGate_Mode) && ( ! LoRa_initialized)){
         tft.fillScreen(ST77XX_BLACK);
         tft.setCursor(0, 0); 
         tft.setTextColor(ST77XX_GREEN);        
     //    if(BT_KISS_Mode){ tft.setCursor(0, 20); tft.print("Bluetooth KISS Client Mode");}; 
         if(Serial_KISS_Mode) { tft.setCursor(0, 20); tft.print("Serial KISS Client Mode");};
         if(TCP_KISS_Mode) { tft.setCursor(0, 20); tft.print("TCP KISS Client Mode");};
         
         if(iGate_Mode) { tft.setCursor(60, 40); tft.print("iGate Mode"); } ;
         if(Tracker_Mode) { tft.setCursor(50, 40); tft.print("Tracker Mode"); } ;                        
         tft.setCursor(50, 70); tft.print("Admin mode: "); 
         tft.setCursor(50, 100); tft.print( String(AdminTimeOutCounter));
         tft.setCursor(100, 100); tft.print("/ "); tft.setCursor(120, 100); tft.print( String(AdminTimeOut));
         }

     else if ((BT_KISS_Mode || Serial_KISS_Mode || TCP_KISS_Mode|| iGate_Mode) && ( LoRa_initialized)){
         ui3_payload= String("LoRa RX=") + LoRa_rx_packets + String(" TX=") + LoRa_tx_packets ;
    //     ui4_payload= String("KISS RX=") + Kiss_rx_packets + String(" TX=") + Kiss_tx_packets ;
         if( LoRa_rx_packets > 0 ){        
            String line_0 = String("   LoRa TNC KISS_Mode");
            String line_1 = String("RSSI:") + String(lora_rssi) + String("dBm");
            String line_2 = String("SNR:") + String(lora_snr) + String("dB") + "  ERR:" + String(int(lora_frequencyError)) + String("Hz");
            String line_3 = ui3_payload;   // original packet source call 
            String line_4 = ui4_payload ;  // first node report
            String line_5 = ui5_payload ;  // last node report

            // display LoRa TX/RX operation on ST77xx OLED display 
            tft.fillScreen(ST77XX_BLACK);
//          tft.setTextPadding(240); // Pad next print() text to full width to over-write old text
            tft.setCursor(0, 0); 
//          tft.setTextFont(GFXFF);
//          tft.setTextColor(TFT_WHITE, TFT_BLACK);
   
            tft.setTextColor(ST77XX_GREEN);
            tft.setCursor(0, 20); tft.print(line_0);
            tft.setCursor(0, 40); tft.print(line_1);
            tft.setCursor(0, 60); tft.print(line_2);
            tft.setTextColor(ST77XX_YELLOW);      
            tft.setCursor(0, 80); tft.print(line_3);
            tft.setTextColor(ST77XX_RED);
            tft.setCursor(0, 100); tft.print(line_4);
            tft.setTextColor(ST77XX_WHITE);
            tft.setCursor(0, 110); tft.print("____________________");
            tft.setTextColor(ST77XX_YELLOW);
           tft.setCursor(0, 130); tft.print(StatusLine);
           }
        else {
           tft.setTextSize(1);
          // display waiting for LoRa Spot 
          tft.fillScreen(ST77XX_BLACK);
          tft.setTextColor(ST77XX_WHITE);
          tft.setCursor(0, 40); tft.print("            WAITING");
          tft.setCursor(0, 70); tft.print("     FOR KISS/LORA PKTS");
          tft.setTextColor(ST77XX_WHITE);
          tft.setCursor(0, 110); tft.print("____________________");
          tft.setTextColor(ST77XX_YELLOW);
          tft.setCursor(0, 130); tft.print(StatusLine);
          };
      }
 
     else if(Admin_Mode ){
         tft.fillScreen(ST77XX_BLACK);
         tft.setCursor(0, 0); 
         tft.setTextColor(ST77XX_GREEN);       
         tft.setCursor(50, 40); tft.print("Admin Mode");  
      // tft.setCursor(50, 40); tft.print("Admin mode: "); 
         tft.setCursor(50, 60); tft.print( String(AdminTimeOutCounter));
         tft.setCursor(80, 60); tft.print("/ "); tft.setCursor(100, 60); tft.print( String(AdminTimeOut));
         }                 
     else if ((BT_KISS_Mode || Serial_KISS_Mode || TCP_KISS_Mode ) && ( LoRa_initialized)){
//         String line_date = String (UTC.dateTime( ezt::now(), "d/n/Y H:i:s.v")); 
         String line_0 ="";
         if(BT_KISS_Mode){ String line_0 = String("Bluetooth KISS Client Mode");}
         else if(Serial_KISS_Mode){ String line_0 = String("Serial KISS Client Mode"); };        
         String line_1 = String("RSSI:") + String(lora_rssi) + String("dBm");
         String line_2 = String("SNR:") + String(lora_snr) + String("dB") ;
         String line_3 = String("  ERR:") + String(int(lora_frequencyError)) + String("Hz");
         
         tft.fillScreen(ST77XX_BLACK);
         tft.setCursor(0, 0); 
         tft.setTextColor(ST77XX_GREEN);      
         tft.setCursor(0, 11); tft.print(line_0);
//         tft.setCursor(0, 24); tft.print(line_date);        
         tft.setCursor(0, 37); tft.print("Last RX msg report");  
         tft.setCursor(0, 50); tft.print(line_1); 
         tft.setCursor(0, 63); tft.print(line_2); 
         }


#endif
  }  // end of has_ST77XX
else if((has_ST77XX) && (display_event == 2 ) ){
      tft.setTextSize(1);
      // display send beacon 
      tft.fillScreen(ST77XX_BLACK);
      tft.setCursor(0, 20); 
      tft.setTextColor(ST77XX_RED);
      tft.setCursor(40, 60); tft.print("REBOOT NOW");
}
else if((has_ST77XX) && (display_event == 3 ) ){
      tft.setTextSize(1);
      // display send beacon 
      tft.fillScreen(ST77XX_BLACK);
      tft.setTextColor(ST77XX_WHITE);
      tft.setCursor(20, 60); tft.print("SEND BEACON NOW");
}
else if((has_ST77XX) &&  (display_event == 4 ) ){
      tft.setTextSize(1);
      // display send beacon 
      tft.fillScreen(ST77XX_BLACK);
      tft.setTextColor(ST77XX_RED);
      tft.setCursor(40, 60); tft.print("DISABLE WIFI");
      tft.setTextColor(ST77XX_ORANGE);
      tft.setCursor(40, 80); tft.print("(Needs Reboot)");      
 //     display_event = 2 ;  // request reboot
}
else if((has_ST77XX) && (display_event == 5 ) ){
      tft.setTextSize(1);
      // display send beacon 
      tft.fillScreen(ST77XX_BLACK);
      tft.setTextColor(ST77XX_GREEN);
      tft.setCursor(40, 60); tft.print("ENABLE WIFI");
      tft.setTextColor(ST77XX_ORANGE);
      tft.setCursor(35, 80); tft.print("(Needs Reboot)");      
 //     display_event = 2 ;  // request reboot
}
else if((has_ST77XX) &&  (display_event == 6 ) ){
      tft.setTextSize(1);
      // display send beacon 
      tft.fillScreen(ST77XX_BLACK);
      tft.setTextColor(ST77XX_GREEN);
      tft.setCursor(30, 60); tft.print("SENDING BEACON !");
}
else if((has_ST77XX) &&  (display_event == 16 ) ){
      tft.setTextSize(1);
      // display send beacon 
      tft.fillScreen(ST77XX_BLACK);
      tft.setTextColor(ST77XX_GREEN);
      tft.setCursor(30, 60); tft.print("INVALID BEACON");
      tft.setCursor(30, 80); tft.print("  PAYLOAD !");
}
else if((has_ST77XX) &&  (display_event == 7 ) ){
      tft.setTextSize(1);
      // display waiting for LoRa Spot 
      tft.fillScreen(ST77XX_BLACK);
      tft.setCursor(0, 20); 
      tft.setTextColor(ST77XX_RED);
      tft.setCursor(0, 60); tft.print("Waiting for LoRa Spots");

}

else if((has_ST77XX) && (display_event == 9 ) ){
      tft.setTextSize(1);
      // display send beacon 
      tft.fillScreen(ST77XX_BLACK);
      tft.setTextColor(ST77XX_RED);
      tft.setCursor(30, 60); tft.print("APRSIS DOWN...");
      vTaskDelay(pdMS_TO_TICKS(2000));    
}

else if((has_ST77XX) && (display_event == 8 ) ){
      tft.setTextSize(1);
      // display waiting for LoRa Spot 
      tft.fillScreen(ST77XX_BLACK);
      tft.setCursor(0, 20); 
      tft.setTextColor(ST77XX_RED);
      tft.setCursor(30, 60); tft.print("Now.. Please Reboot");
      tft.setCursor(80, 80); tft.print("manually");
      delay(5000);

};

#endif  // TFT_MOD
}
