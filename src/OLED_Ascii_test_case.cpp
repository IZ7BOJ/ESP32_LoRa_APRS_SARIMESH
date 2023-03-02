// Master configuration directives: need to be customized depending on HW/SW configuratio of the device
#include "Master_Config.h"

// OLED_Ascii_test_case.ino
// last updated 20201004 by MF

#ifdef SSD1306_ASCII_LIB


void setup_SDD1306_Ascii() {  
  debugA("\r\n\r\nStarting SSD1306_Ascii display test case\r\n");

  const char* label[] = {"Voltage:", "Load:", "Runtime:"};
  const char* units[] = {"volts", "watts", "mins"};   
//  Wire.begin();
//  Wire.setClock(400000L);

#if RST_PIN >= 0
  display.begin(&Adafruit128x64, I2C_ADDRESS, RST_PIN);
#else // RST_PIN >= 0
  display.begin(&Adafruit128x64, I2C_ADDRESS);
#endif // RST_PIN >= 0

  display.setFont(Callibri15);
// display.setFont(Arial14);
// display.setFont(Callibri11_bold);
// display.setFont(TimesNewRoman13);

  // Increase space between letters.
  display.setLetterSpacing(2);
  
  display.clear();

  // Setup form and find longest label.
  for (uint8_t i = 0; i < 3; i++) {
    display.println(label[i]);
    uint8_t w = display.strWidth(label[i]);
    col0 = col0 < w ? w : col0; 
  }
  // Three pixels after label.
  col0 += 3;
  // Allow two or more pixels after value.
  col1 = col0 + display.strWidth("99.9") + 2;
  // Line height in rows.
  rows = display.fontRows();

  // Print units.  
  for ( uint8_t i = 0; i < 3; i++) {
    display.setCursor(col1 + 1, i*rows);
    display.print(units[i]);
  }

}

#endif
