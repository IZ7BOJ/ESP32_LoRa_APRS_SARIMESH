// File_Download_Upload.h
#ifndef FILES_UP_DOWN_H
#define FILES_UP_DOWN_H

#include "rdbg.h"
#include <Arduino.h>
#include "configuration_data_definition.h"
#include <FS.h>
#include <LittleFS.h>
#include "File_Download_Upload.h"
#include "JsonConfigFile.h"
#include <WebServer.h>  
#include "SD_Card_Support_Functions.h"

extern WebServer web_server;
extern String DeviceId ;
extern String webpage ;
extern volatile bool has_LittleFS ;           // autodiscovery
extern volatile bool has_FM24W256;              // autodiscovery
extern volatile bool Admin_Mode ;
extern strConfig ESP_Config ;

void UpdateLocalConfigData();
bool loadConfigurationFromFlash(const char *filename  , const strConfig &ESP_Config_str);  // Loads the configuration from a file
void saveConfigurationToFlash(const char *filename , const strConfig &ESP_Config_str);  // Saves the configuration to a file
void printFile(const char *filename);
void UpdateLocalConfigData();
void WriteConfig();
void dropAllFiles();
void SendHTML_Header();
void SendHTML_Content();
void SendHTML_Stop();
void SelectInput(String heading1, String command, String arg_calling_name);
void ReportLittleFSNotPresent();
void ReportFileNotPresent(String target);
void ReportFSNotPresent(void);
void ReportCouldNotCreateFile(String target);
String file_size(int bytes);
void LittleFS_file_download(String filename);
void File_Upload();
void File_Clear();
void handleFileUpload(); // upload a new file to the Filing system
void FilesHomePage();
void File_Download(); // This gets called twice, the first pass selects the input, the second pass then processes the command line arguments
void listAllFiles();
void dropAllFiles();
void printLocalTime();

#endif
