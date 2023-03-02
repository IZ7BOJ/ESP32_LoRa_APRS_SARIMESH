// SD_Card_Support_Functions.cpp
// Master configuration directives: need to be customized depending on HW/SW configuratio of the device
#include "Master_Config.h"

#include <Arduino.h>
extern "C" char __data_start[];    // start of SRAM data
extern "C" char _end[];     // end of SRAM data (used to check amount of SRAM this program's variables use)
extern "C" char __data_load_end[];  // end of FLASH (used to check amount of Flash this program's code and data uses)
// Libraries for SD card
#include "FS.h"
#include "SD.h"
#include "SD_Card_Support_Functions.h"

char has_SD_Card[10];
char SD_Card_Size[10];
void setup_SD_Card();  // proto 

void setup_SD_Card(){

#ifdef SD_MOD
  if(!SD.begin(SD_CS)){
     Serial.println("Card Mount Failed");
        strcpy(has_SD_Card ,"Not Found" );        
     return;
     };

    Serial.println("ST7735 SPI display + SD card detected...");
//    has_ST77XX = true ;

    uint8_t cardType = SD.cardType();

    if(cardType == CARD_NONE){
        Serial.println("No SD card attached");
        strcpy(has_SD_Card ,"No SD card" );        
        return;
    }

    Serial.print("SD Card Type: ");
    if(cardType == CARD_MMC){
        Serial.println("MMC");
        strcpy(has_SD_Card ,"MMC" );        
    } else if(cardType == CARD_SD){
        Serial.println("SDSC");
        strcpy(has_SD_Card ,"SDSC" );        
    } else if(cardType == CARD_SDHC){
        Serial.println("SDHC");
        strcpy(has_SD_Card ,"SDHC" );        
    } else {
        Serial.println("UNKNOWN");
        strcpy(has_SD_Card ,"UNKNOWN" );        
    }

    uint64_t cardSize = SD.cardSize() / (1024 * 1024);
    Serial.printf("SD Card Size: %lluMB\n", cardSize);
    sprintf(SD_Card_Size,"%lluMB", cardSize);        

    listDir(SD, "/", 0);
    createDir(SD, "/mydir");
    listDir(SD, "/", 0);
    removeDir(SD, "/mydir");
    listDir(SD, "/", 2);
    writeFile(SD, "/hello.txt", "Hello ");
    appendFile(SD, "/hello.txt", "World!\n");
    readFile(SD, "/hello.txt");
    deleteFile(SD, "/foo.txt");
    renameFile(SD, "/hello.txt", "/foo.txt");
    readFile(SD, "/foo.txt");
    testFileIO(SD, "/test.txt");
    Serial.printf("Total space: %lluMB\n", SD.totalBytes() / (1024 * 1024));
    Serial.printf("Used space: %lluMB\n", SD.usedBytes() / (1024 * 1024));
#else
     Serial.println("SD Card not available");
#endif
}
