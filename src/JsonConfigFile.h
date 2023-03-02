//JsonConfigFile.h
#ifndef JSON_CONfIG_H
#define JSON_CONfIG_H

#include <ArduinoJson.h>
#include <LittleFS.h>
#include "configuration_data_definition.h"

extern strConfig  ESP_Config ;

bool loadConfigurationFromFlash(const char *filename); // Loads the configuration from a file
void saveConfigurationToFlash(const char *filename);  // Saves the configuration to a file
void printFile(const char *filename) ;
void setup_LittleFS_Config( void );

#endif
