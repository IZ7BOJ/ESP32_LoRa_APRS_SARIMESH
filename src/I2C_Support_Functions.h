//I2C_Support_Functions.h
#ifndef I2C_SUPPORT_H
#define I2C_SUPPORT_H

#include "rdbg.h"
#include <Arduino.h>
#include "Wire.h"

extern volatile bool has_INA219_5 ;               // autodiscovery A0 --> 0x41
extern volatile bool has_INA219_24 ;              // autodiscovery A1 --> 0x44
extern volatile bool has_PCF8574 ;               // autodiscovery
extern volatile bool has_DS3231 ;                // autodiscovery
extern volatile bool has_DS3231_eeprom ;         // autodiscovery
extern volatile bool has_Si5351 ;                // autodiscovery
extern volatile bool has_SSD1306 ;               // autodiscovery
extern volatile bool has_BME280 ;                // autodiscovery
extern volatile bool has_Si7021 ;                // autodiscovery
extern volatile bool has_FM24W256 ;              // autodiscovery
extern volatile bool I2C_Busy ;

uint8_t I2C_Scanner_alt();
uint8_t I2C_Scanner();

#endif
