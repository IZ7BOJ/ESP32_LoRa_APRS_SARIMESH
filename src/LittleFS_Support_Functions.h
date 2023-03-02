// LittleFS_Support_Functions.h
#ifndef UT_LITTLEFS_H
#define UT_LITTLEFS_H

#include "rdbg.h"
#include "FS.h"
#include <LittleFS.h>

#define FORMAT_LITTLEFS_IF_FAILED true

void createDir(fs::FS &fs, const char * path);
void removeDir(fs::FS &fs, const char * path);
void readFile(fs::FS &fs, const char * path);
void appendFile(fs::FS &fs, const char * path, const char * message);
void listDir(fs::FS &fs, const char * dirname, uint8_t levels) ;
void writeFile(fs::FS &fs, const char * path, const char * message) ;
void renameFile(fs::FS &fs, const char * path1, const char * path2);
void deleteFile(fs::FS &fs, const char * path) ;
void writeFile2(fs::FS &fs, const char * path, const char * message);
void deleteFile2(fs::FS &fs, const char * path) ;
void testFileIO(fs::FS &fs, const char * path) ;
void ut_littlefs_setup();

#endif
