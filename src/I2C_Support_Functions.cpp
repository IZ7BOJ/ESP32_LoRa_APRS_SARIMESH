// I2C_Support_Functions.cpp
#include "I2C_Support_Functions.h"

void OLED_Reset(void){
  if(oled_rst){
    pinMode(oled_rst,OUTPUT);
    digitalWrite(oled_rst, LOW); // i.e. set GPIO16 low to reset HELTEC OLED
    delay(50);
    digitalWrite(oled_rst, HIGH);
    delay(50);
    };

  if(O_RST){
    pinMode(O_RST,OUTPUT);
    digitalWrite(O_RST, LOW); // i.e. set GPIO16 low to reset HELTEC OLED
    delay(50);
    digitalWrite(O_RST, HIGH);
    delay(50);
    };    
}


#ifdef FORCE_SERIAL_DEBUG    

uint8_t I2C_Scanner(void){
debugA("I2C_Scanner: Scanning I2C Addresses\r");
//if( !(xSemaphoreTake(i2c_mutex_v, ( TickType_t ) 2) )){ debugA("I2C_Scanner: xSemaphoreTake FAILED: aborting "); return 0 ;  };

uint8_t cnt=0;
for(uint8_t i=1;i<128;i++){
  Wire.beginTransmission(i);
  uint8_t ec=Wire.endTransmission(true);
  if(ec==0){
    if(i<16)debugAl("0");
    debugAl("%02X",i);
    cnt++;
  }
  else debugAl("..");
  debugAl(" ");
  if ((i&0x0f)==0x0f)debugAl("\r");
  }
debugA("I2C_Scanner: Scan Completed, "); debugAl("%d",cnt); debugAl(" I2C Devices found.\r");
//xSemaphoreGive(i2c_mutex_v);
return(cnt);
}

#else
uint8_t I2C_Scanner(void){
debugA("I2C_Scanner: Scanning I2C Addresses\r");
//if( !(xSemaphoreTake(i2c_mutex_v, ( TickType_t ) 2) )){ debugA("I2C_Scanner: xSemaphoreTake FAILED: aborting "); return 0 ;  };

uint8_t cnt=0;
for(uint8_t i=1;i<128;i++){
  Wire.beginTransmission(i);
  uint8_t ec=Wire.endTransmission(true);
  if(ec==0){
    if(i<16)Serial.printf("0");
    Serial.printf("%02X",i);
    cnt++;
  }
  else Serial.printf("..");
  Serial.printf(" ");
  if ((i&0x0f)==0x0f)Serial.printf("\r");
  }
debugA("I2C_Scanner: Scan Completed, "); Serial.printf("%d",cnt); Serial.printf(" I2C Devices found.\r");
//xSemaphoreGive(i2c_mutex_v);
return(cnt);
}

#endif


#ifdef FORCE_SERIAL_DEBUG    

uint8_t I2C_Scanner_alt(void){
  byte error, address;
  int nDevices;
//  if( !(xSemaphoreTake(i2c_mutex_v, ( TickType_t ) 2) )){ debugA("I2C_Scanner_alt: xSemaphoreTake FAILED: aborting "); return 0;  };
  debugA("\r\nI2C_Scanner_alt: Scanning I2C bus ...\r");

  nDevices = 0;
  for(address = 1; address < 128; address++ ) 
  {
    // The i2c_scanner uses the return value ofthe Write.endTransmisstion to see if a device did acknowledge to the address.
    Wire.beginTransmission(address);
    error = Wire.endTransmission();

    if (error == 0) {
      debugAl("\r\nI2C device found at address %02X ", address);
      if(address == 0x21){debugAl(" PCF8574 Bus Expander\r"); has_PCF8574 = true;}
      else if(address == 0x57){debugAl(" DS3231 RTC \r"); has_DS3231 = true;}
      else if(address == 0x68){debugAl(" DS3231 EEPROM \r\n"); has_DS3231_eeprom = true;}
      else if(address == 0x60){debugAl(" Si5351 VFO \r\n"); has_Si5351 = true;}
      else if(address == 0x3c){debugAl(" SSD1306 OLED Display \r"); has_SSD1306 = true;} 
      else if(address == 0x76){debugAl(" BME280 Sensor 0x76\r\n"); has_BME280 = true;}
      else if(address == 0x77){debugAl(" BME280 Sensor 0x77\r\n"); has_BME280 = true;}
      else if(address == 0x04){debugAl(" Si7021 Sensor \r\n"); has_Si7021 = true;}
      else if(address == 0x41){debugAl(" INA219 - 1 \r\n"); has_INA219_5 = true;}
      else if(address == 0x44){debugAl(" INA219 - 4 \r\n"); has_INA219_24 = true;}      
      else if(address == 0x50){debugAl(" FM24W256 FRAM \r"); has_FM24W256 = true;}
      nDevices++;
      }
    else if (error==4)  {
      debugA("Unknow error at address %02X \r\n", address);
      }    
  }
  if (nDevices == 0){
    debugA("No I2C devices found\r\n");
  }
  else
    debugA("done\r\n");
  return(nDevices);
}

#else
uint8_t I2C_Scanner_alt(void){
  byte error, address;
  int nDevices;
//  if( !(xSemaphoreTake(i2c_mutex_v, ( TickType_t ) 2) )){ debugA("I2C_Scanner_alt: xSemaphoreTake FAILED: aborting "); return 0;  };
  Serial.print("Scanning I2C bus ...\n\r");

  nDevices = 0;
  for(address = 1; address < 128; address++ ) 
  {
    // The i2c_scanner uses the return value of
    // the Write.endTransmisstion to see if
    // a device did acknowledge to the address.
    Wire.beginTransmission(address);
    error = Wire.endTransmission();

    if (error == 0) {
      Serial.printf("I2C device found at address %02X ", address);
      if(address == 0x21){Serial.print(" PCF8574 Bus Expander\r"); has_PCF8574 = true;}
      else if(address == 0x57){Serial.print(" DS3231 RTC \n\r"); has_DS3231 = true;}
      else if(address == 0x68){Serial.print(" DS3231 EEPROM \n\r"); has_DS3231_eeprom = true;}
      else if(address == 0x60){Serial.print(" Si5351 VFO \n\r"); has_Si5351 = true;}
      else if(address == 0x3c){Serial.print(" SSD1306 OLED Display \n\r"); has_SSD1306 = true;} 
      else if(address == 0x76){Serial.print(" BME280 Sensor 0x76\n\r"); has_BME280 = true;}
      else if(address == 0x77){Serial.print(" BME280 Sensor 0x77\n\r"); has_BME280 = true;}
      else if(address == 0x04){Serial.print(" Si7021 Sensor \n\r"); has_Si7021 = true;}
      else if(address == 0x41){Serial.print(" INA219 - 1 \r\n"); has_INA219_5 = true;}
      else if(address == 0x44){Serial.print(" INA219 - 4 \r\n"); has_INA219_24 = true;}      
      else if(address == 0x50){Serial.print(" FM24W256 FRAM \n\r"); has_FM24W256 = true;}
      nDevices++;
      }
    else if (error==4)  {
      Serial.printf("Unknow error at address %02X \r\n", address);
      }    
  }
  if (nDevices == 0)
    Serial.print("No I2C devices found\n\r");
  else
    Serial.print("done\r\n");
//  xSemaphoreGive(i2c_mutex_v);
  return(nDevices);
}

#endif
