// rdbg.h
#ifndef RDBG_H
#define RDBG_H

// Master configuration directives: need to be customized depending on HW/SW configuratio of the device
#include "Master_Config.h"

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

#endif
