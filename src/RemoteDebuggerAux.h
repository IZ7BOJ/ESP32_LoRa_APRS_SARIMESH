// RemoteDebuggerAux.h
#ifndef RDA_H
#define RDA_H

#include "rdbg.h"
#include "configuration_data_definition.h"
extern strConfig ESP_Config ;  
extern SemaphoreHandle_t i2c_mutex_v;

#define BENCHMARK_EXECS 10000

extern volatile bool has_FM24W256;
#include "FRAM.h"
extern FRAM fram;
#include <FastCRC.h>
extern FastCRC32 CRC32;
uint32_t fram_crc;

extern volatile bool save_config_enable ;
#include <ezTime_loc.h>
extern Timezone Italy ;
extern Timezone UTC;
extern volatile uint8_t RebootTag ;           // RebootTag
int log_head=0 , log_len=0, log_size=400, fram_base_log=2048, fram_params=2000 ;

void benchInt() ;
void benchFloat() ;
void benchGpio() ;
void benchAll() ;
void funcArgStr (String str) ;
void funcArgChar (char character) ;
void funcArgInt (int number) ;
void sendMagicPacket();
void write_event_to_log(String my_record);
void show_events(void);
void show_gps_status();
void do_wifi_scan();
bool DisplayConfig(void );
void fram_dump(void);
void show_dashboard(void);
void show_stats(void) ;
void area_restart(void);
void log_display(void);
void fram_log_reset(void);
void fram_log_set(void);
void config_enable(void);
void config_disable(void);
void WriteRecord(String *text);

extern String SW_Revision ;
extern volatile float LoRa_DutyCycle;     // our own transmission duty cycle

uint8_t I2C_Scanner_alt();
uint8_t I2C_Scanner();

extern String Loc_Lat;
extern String Loc_Lon ;
extern volatile float cpu_temp ;


#endif
