// Master configuration directives: need to be customized depending on HW/SW configuratio of the device
#include "Master_Config.h"

#include <Arduino.h>





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


#include <Adafruit_ST77xx.h> 
#include <Adafruit_ST7735.h> // Hardware-specific library for ST7735^M
#include <Adafruit_ST7789.h> // Hardware-specific library ffor ST7789^M

extern Adafruit_ST7789 tft ;

void testfillcircles(uint8_t radius, uint16_t color) ;
void testfastlines(uint16_t color1, uint16_t color2) ;
void setup_tft_adafruit() ;
void Task3code( void * pvParameters );
void mediabuttons() ;
void testdrawrects(uint16_t color) ;
void testdrawtext(char *text, uint16_t color) ;
void tftPrintTest() ;
void testtriangles() ;
void testfillrects(uint16_t color1, uint16_t color2) ;
void testlines(uint16_t color) ;
void testdrawcircles(uint8_t radius, uint16_t color)  ;
void testroundrects() ;

extern volatile bool Task3_flag  ;
extern TaskHandle_t Task3; 



#ifdef TFT_MOD 
#ifdef USE_ADAFRUIT_TFT_LIB

float p = 3.1415926;

void loop_tft_adafruit(void) {
  Serial.println(F("Initialized"));
  tft.setRotation(1);
  
  uint16_t time = millis();
  tft.fillScreen(ST77XX_BLACK);
  time = millis() - time;
  Serial.printf("tft.fillScreen(ST77XX_BLACK) exec time (msec) ");
  Serial.println(time, DEC);
  vTaskDelay(pdMS_TO_TICKS(500));

  // large block of text
  Serial.println("testdrawtext test ....");
  tft.fillScreen(ST77XX_BLACK);
  
  testdrawtext((char *)"Lorem ipsum dolor sit amet, consectetur adipiscing elit. Curabitur adipiscing ante sed nibh tincidunt feugiat. Maecenas enim massa, fringilla sed malesuada et, malesuada sit amet turpis. Sed porttitor neque ut ante pretium vitae malesuada nunc bibendum. Nullam aliquet ultrices massa eu hendrerit. Ut sed nisi lorem. In vestibulum purus a tortor imperdiet posuere. ", ST77XX_WHITE);
  vTaskDelay(pdMS_TO_TICKS(1000));

  Serial.println("tftPrintTest test ....");
  // tft print function!
  tftPrintTest();
  vTaskDelay(pdMS_TO_TICKS(4000));

  Serial.println("graphic tests ....");
  // a single pixel
  tft.drawPixel(tft.width()/2, tft.height()/2, ST77XX_GREEN);
  vTaskDelay(pdMS_TO_TICKS(500));

  // line draw test
  testlines(ST77XX_YELLOW);
  vTaskDelay(pdMS_TO_TICKS(500));

  // optimized lines
  testfastlines(ST77XX_RED, ST77XX_BLUE);
  vTaskDelay(pdMS_TO_TICKS(500));

  testdrawrects(ST77XX_GREEN);
  vTaskDelay(pdMS_TO_TICKS(500));

  testfillrects(ST77XX_YELLOW, ST77XX_MAGENTA);
  vTaskDelay(pdMS_TO_TICKS(500));

  tft.fillScreen(ST77XX_BLACK);
  testfillcircles(10, ST77XX_BLUE);
  testdrawcircles(10, ST77XX_WHITE);
  vTaskDelay(pdMS_TO_TICKS(500));

  testroundrects();
  vTaskDelay(pdMS_TO_TICKS(500));

  testtriangles();
  vTaskDelay(pdMS_TO_TICKS(500));

  mediabuttons();
  vTaskDelay(pdMS_TO_TICKS(500));

  Serial.println("tests completed");
  vTaskDelay(pdMS_TO_TICKS(1000));
}

void loop_tft_adafruit_orig() {
  tft.invertDisplay(true);
  vTaskDelay(pdMS_TO_TICKS(500));
  tft.invertDisplay(false);
  vTaskDelay(pdMS_TO_TICKS(500));
}

void testlines(uint16_t color) {
  tft.fillScreen(ST77XX_BLACK);
  for (int16_t x=0; x < tft.width(); x+=6) {
    tft.drawLine(0, 0, x, tft.height()-1, color);
    vTaskDelay(pdMS_TO_TICKS(0));
  }
  for (int16_t y=0; y < tft.height(); y+=6) {
    tft.drawLine(0, 0, tft.width()-1, y, color);
    vTaskDelay(pdMS_TO_TICKS(0));
  }

  tft.fillScreen(ST77XX_BLACK);
  for (int16_t x=0; x < tft.width(); x+=6) {
    tft.drawLine(tft.width()-1, 0, x, tft.height()-1, color);
    vTaskDelay(pdMS_TO_TICKS(0));
  }
  for (int16_t y=0; y < tft.height(); y+=6) {
    tft.drawLine(tft.width()-1, 0, 0, y, color);
    vTaskDelay(pdMS_TO_TICKS(0));
  }

  tft.fillScreen(ST77XX_BLACK);
  for (int16_t x=0; x < tft.width(); x+=6) {
    tft.drawLine(0, tft.height()-1, x, 0, color);
    vTaskDelay(pdMS_TO_TICKS(0));
  }
  for (int16_t y=0; y < tft.height(); y+=6) {
    tft.drawLine(0, tft.height()-1, tft.width()-1, y, color);
    vTaskDelay(pdMS_TO_TICKS(0));
  }

  tft.fillScreen(ST77XX_BLACK);
  for (int16_t x=0; x < tft.width(); x+=6) {
    tft.drawLine(tft.width()-1, tft.height()-1, x, 0, color);
    delay (0);
  }
  for (int16_t y=0; y < tft.height(); y+=6) {
    tft.drawLine(tft.width()-1, tft.height()-1, 0, y, color);
    delay (0);
  }
}

void testdrawtext(char *text, uint16_t color) {
  tft.setCursor(0, 0);
  tft.setTextColor(color);
  tft.setTextWrap(true);
  tft.print(text);
}

void testfastlines(uint16_t color1, uint16_t color2) {
  tft.fillScreen(ST77XX_BLACK);
  for (int16_t y=0; y < tft.height(); y+=5) {
    tft.drawFastHLine(0, y, tft.width(), color1);
  }
  for (int16_t x=0; x < tft.width(); x+=5) {
    tft.drawFastVLine(x, 0, tft.height(), color2);
  }
}

void testdrawrects(uint16_t color) {
  tft.fillScreen(ST77XX_BLACK);
  for (int16_t x=0; x < tft.width(); x+=6) {
    tft.drawRect(tft.width()/2 -x/2, tft.height()/2 -x/2 , x, x, color);
  }
}

void testfillrects(uint16_t color1, uint16_t color2) {
  tft.fillScreen(ST77XX_BLACK);
  for (int16_t x=tft.width()-1; x > 6; x-=6) {
    tft.fillRect(tft.width()/2 -x/2, tft.height()/2 -x/2 , x, x, color1);
    tft.drawRect(tft.width()/2 -x/2, tft.height()/2 -x/2 , x, x, color2);
  }
}

void testfillcircles(uint8_t radius, uint16_t color) {
  for (int16_t x=radius; x < tft.width(); x+=radius*2) {
    for (int16_t y=radius; y < tft.height(); y+=radius*2) {
      tft.fillCircle(x, y, radius, color);
    }
  }
}

void testdrawcircles(uint8_t radius, uint16_t color) {
  for (int16_t x=0; x < tft.width()+radius; x+=radius*2) {
    for (int16_t y=0; y < tft.height()+radius; y+=radius*2) {
      tft.drawCircle(x, y, radius, color);
    }
  }
}

void testtriangles() {
  tft.fillScreen(ST77XX_BLACK);
  uint16_t color = 0xF800;
  int t;
  int w = tft.width()/2;
  int x = tft.height()-1;
  int y = 0;
  int z = tft.width();
  for(t = 0 ; t <= 15; t++) {
    tft.drawTriangle(w, y, y, x, z, x, color);
    x-=4;
    y+=4;
    z-=4;
    color+=100;
  }
}

void testroundrects() {
  tft.fillScreen(ST77XX_BLACK);
  uint16_t color = 100;
  int i;
  int t;
  for(t = 0 ; t <= 4; t+=1) {
    int x = 0;
    int y = 0;
    int w = tft.width()-2;
    int h = tft.height()-2;
    for(i = 0 ; i <= 16; i+=1) {
      tft.drawRoundRect(x, y, w, h, 5, color);
      x+=2;
      y+=3;
      w-=4;
      h-=6;
      color+=1100;
    }
    color+=100;
  }
}

void tftPrintTest() {
  tft.setTextWrap(false);
  tft.fillScreen(ST77XX_BLACK);
  tft.setCursor(0, 30);
  tft.setTextColor(ST77XX_RED);
  tft.setTextSize(1);
  tft.println("Hello World!");
  tft.setTextColor(ST77XX_YELLOW);
  tft.setTextSize(2);
  tft.println("Hello World!");
  tft.setTextColor(ST77XX_GREEN);
  tft.setTextSize(3);
  tft.println("Hello World!");
  tft.setTextColor(ST77XX_BLUE);
  tft.setTextSize(4);
  tft.print(1234.567);
  vTaskDelay(pdMS_TO_TICKS(1500));
  tft.setCursor(0, 0);
  tft.fillScreen(ST77XX_BLACK);
  tft.setTextColor(ST77XX_WHITE);
  tft.setTextSize(0);
  tft.println("Hello World!");
  tft.setTextSize(1);
  tft.setTextColor(ST77XX_GREEN);
  tft.print(p, 6);
  tft.println(" Want pi?");
  tft.println(" ");
  tft.print(8675309, HEX); // print 8,675,309 out in HEX!
  tft.println(" Print HEX!");
  tft.println(" ");
  tft.setTextColor(ST77XX_WHITE);
  tft.println("Sketch has been");
  tft.println("running for: ");
  tft.setTextColor(ST77XX_MAGENTA);
  tft.print(millis() / 1000);
  tft.setTextColor(ST77XX_WHITE);
  tft.print(" seconds.");
}

void mediabuttons() {
  // play
  tft.setRotation(2);
  tft.fillScreen(ST77XX_BLACK);
  tft.fillRoundRect(25, 10, 78, 60, 8, ST77XX_WHITE);
  tft.fillTriangle(42, 20, 42, 60, 90, 40, ST77XX_RED);
  vTaskDelay(pdMS_TO_TICKS(500));
  // pause
  tft.fillRoundRect(25, 90, 78, 60, 8, ST77XX_WHITE);
  tft.fillRoundRect(39, 98, 20, 45, 5, ST77XX_GREEN);
  tft.fillRoundRect(69, 98, 20, 45, 5, ST77XX_GREEN);
  vTaskDelay(pdMS_TO_TICKS(500));
  // play color
  tft.fillTriangle(42, 20, 42, 60, 90, 40, ST77XX_BLUE);
  vTaskDelay(pdMS_TO_TICKS(50));
  // pause color
  tft.fillRoundRect(39, 98, 20, 45, 5, ST77XX_RED);
  tft.fillRoundRect(69, 98, 20, 45, 5, ST77XX_RED);
  // play color
  tft.fillTriangle(42, 20, 42, 60, 90, 40, ST77XX_GREEN);
}


void Task3code( void * pvParameters ){
  int counter1 =0;
  Serial.printf("Task3 (TFT_eSPI) starting on core ");  Serial.println(xPortGetCoreID()); 

  for(;;){
     if( Task3_flag == true) { 
         // execute TFT_adafruit loop 
         loop_tft_adafruit();
         vTaskDelay(pdMS_TO_TICKS(15000));
 //        Task3_flag = false ;
         };
     counter1++;
     } 
}


void setup_tft_adafruit(){
// init already done in SD_test_case  
//  tft.setRotation(1);
//  tft.init();            // initialize LCD
 //create a task that will be executed in the Task3code() function, with priority 1 and executed on core 0
  xTaskCreatePinnedToCore(
                    Task3code,   // Task function. 
                    "Task3",     // name of task. 
                    10000,       // Stack size of task 
                    NULL,        // parameter of the task 
                    1,           // priority of the task 
                    &Task3,      // Task handle to keep track of created task 
                    0);          // pin task to core 0                   

}



#endif // end of #ifdef USE_ADAFRUIT_TFT_LIB
#endif // end of #ifdef TFT_MOD
