//RemoteDebuggerAux.cpp
// Master configuration directives: need to be customized depending on HW/SW configuratio of the device
#include "Master_Config.h"
#include "RemoteDebuggerAux.h"

// Function example to show a new auto function name of debug* macros

void foo() {
  uint8_t var = 1;
  debugV("this is a debug - var %u", var);
  debugV("This is a println");
  return ;
}

#ifndef DEBUG_DISABLED
#ifndef FORCE_SERIAL_DEBUG

// Process commands from RemoteDebug

void processCmdRemoteDebug() {
	String lastCmd = Debug.getLastCommand();
  if (lastCmd == "reboot") {
    RebootTag = 123 ;
    fram.write8(2015,RebootTag);
    vTaskDelay(pdMS_TO_TICKS(1000));
    RebootTag = 0;
    RebootTag = fram.read8(2015);
    if( RebootTag == 123) debugA("===> Reboot tag correctly written to FRAM ...");
    write_event_to_log("====== system reboot by Remote Debug ========");
    debugA("===> Rebooting...");
    pinMode(14, OUTPUT);      // reset for LoRa 
    digitalWrite(14, LOW);
    }
  else if (lastCmd == "show_gps") {
    show_gps_status();
    }
//  else if (lastCmd == "show_temperature") {
//    print_temperature();
//    }
  else if (lastCmd == "wifi_scan") {
    do_wifi_scan();
    }
  else if (lastCmd == "show_config") {
    DisplayConfig();
    }    
  else if (lastCmd == "i2c_scan") {
    // check I2C bus
    debugA("check I2C bus...\r");
    I2C_Scanner();                          // check I2C bus address in use
    debugA("I2C bus inventory...\r");
    I2C_Scanner_alt();
    }     
  else if (lastCmd == "fram_dump") {
    fram_dump();
    }  
     
#ifdef LORA_FUNCTIONS_ENABLE 
  else if (lastCmd == "show_dashboard") {
    show_dashboard();
    }
  else if (lastCmd == "show_stats") {
    show_stats();
    }       
  else if (lastCmd == "area_restart") {
    area_restart();
    }  
  else if (lastCmd == "send_magic") {
    sendMagicPacket();
    }   
     
#endif
    
  else if (lastCmd == "show_log") {
    log_display();
    }    
 
  else if (lastCmd == "show_events") {
    show_events();
    }
  else if (lastCmd == "fram_log_reset") {
    fram_log_reset();
    }
  else if (lastCmd == "fram_log_set") {
    fram_log_set();
    }
  else if (lastCmd == "config_enable") {
    config_enable();
    }
  else if (lastCmd == "config_disable") {
    config_disable();
    }  
      
	else {
		debugA("Command not jet implemented!\r");
	}
 return ;
}

#endif
#endif

// Benchmarks - simple

// Note: how it as called by SerialDebug, must be return type void and no args
// Note: Flash F variables is not used during the tests, due it is slow to use in loops

#define BENCHMARK_EXECS 10000

// Simple benckmark of integers

void benchInt() {
	int test = 0;
	for (int i = 0; i < BENCHMARK_EXECS; i++) {  // Some integer operations
		test++;
		test += 2;
		test -= 2;
		test *= 2;
		test /= 2;
	  };

	// Note: Debug always is used here
	debugA("*** Benchmark of integers. %u exec.", BENCHMARK_EXECS);
 return ;
}

// Simple benckmark of floats

void benchFloat() {
	float test = 0;
	for (int i = 0; i < BENCHMARK_EXECS; i++) {	// Some float operations
		test++;
		test += 2;
		test -= 2;
		test *= 2;
		test /= 2;
  	};
	debugA("*** Benchmark of floats, %u exec.", BENCHMARK_EXECS); // Note: Debug always is used here
  return ;
}

// Simple benckmark of GPIO

void benchGpio() {
//	const int execs = (BENCHMARK_EXECS / 10); // Reduce it
	const int execs = BENCHMARK_EXECS;
	for (int i = 0; i < execs; i++) {	// Some GPIO operations
//		digitalWrite(RED_LED, HIGH);
//		digitalRead(RED_LED);
//		digitalWrite(RED_LED, LOW);
//		analogRead(A0);
//		analogRead(A0);
//		analogRead(A0);
	   };
	debugA("*** Benchmark of GPIO. %u exec.", execs); // Note: Debug always is used here
  return ;
}

// Run all benchmarks

void benchAll() {
	benchInt();
	benchFloat();
	benchGpio();
	debugA("*** All Benchmark done.");  // Note: Debug always is used here
  return ;
}

// Example functions with argument (only 1) to call from serial monitor
// Note others types is not yet available in this version of SerialDebug

void funcArgStr (String str) {
	debugA("*** called with arg.: %s", str.c_str());
  return ;
}
void funcArgChar (char character) {
	debugA("*** called with arg.: %c", character);
  return ;
}


void funcArgInt (int number) {
	debugA("*** called with arg.: %d", number);
 return;
}




void WriteRecord(String *text){
  if(! has_FM24W256 ) return;
  char rec_array[63]; char crc_buf[9]; char strx[64]; uint8_t a[2] = {0, 0};
  if( !(xSemaphoreTake(i2c_mutex_v, ( TickType_t ) 2) )){ debugA("WriteRecord: xSemaphoreTake FAILED: aborting "); return;  };
  // we got mutex... so go on
  a[0] = fram.read8(fram_params);  a[1] = fram.read8(fram_params+1); log_head = word( a[1], a[0]) ;
  a[0] = fram.read8(fram_params+2);  a[1] = fram.read8(fram_params+3); log_len = word( a[1], a[0]) ;

  if (log_len < log_size){
     log_len++;  
     if(log_head < log_size - 1){ log_head++ ;} else { log_head = 0 ; };
     }
  else{
     if(log_head < log_size - 1){ log_head++ ;} else { log_head = 0 ; };    
//     if(log_head++ >= log_size) log_head = 0 ;    
     };
     
  strncpy(rec_array, (char *)((String *)text->c_str()),62); rec_array[62]='\0';
  fram_crc = CRC32.crc32((uint8_t *)rec_array, strlen(rec_array));
  sprintf(crc_buf, "%X",fram_crc); 
  fram.write(fram_base_log+72*log_head ,(uint8_t *)crc_buf, 9 );
  fram.write(fram_base_log+72*log_head + 9,(uint8_t *)rec_array, 63);
//  debugA("=====> WriteRecord: [%s] %s --> log_head=%d log_len=%d log_size=%d fram_base_log=%d \r", crc_buf, (String *)text->c_str(), log_head, log_len, log_size, fram_base_log );
//  delay (199);
//  fram_get_record( fram_base_log+72*log_head , (char *)strx) ;

  // update log parameters
  fram.write8(fram_params,lowByte(log_head));
  fram.write8(fram_params+1 ,highByte(log_head));
  fram.write8(fram_params+2,lowByte(log_len));
  fram.write8(fram_params+3 ,highByte(log_len));
  xSemaphoreGive(i2c_mutex_v);                    // release mutex
  return ;
}

void fram_get_record( int pntr , char *str){
       char record_crc[9] = "00000000"; char crc_buf[9] = "11111111";
       uint8_t pers = 0;
       uint32_t fram_crc = 0;
       if(! has_FM24W256 ) return;
       
       strcpy((char *)str,"---------------------------------------------------------------");
       if( !(xSemaphoreTake(i2c_mutex_v, ( TickType_t ) 2) )){ debugA("fram_get_record: xSemaphoreTake FAILED: aborting display update"); return;  };
       while(( strcmp(record_crc,crc_buf) ) && (pers < 5)) {
          fram.read(pntr, (uint8_t *)record_crc, 9);
          fram.read(pntr+9, (uint8_t *)str, 63 );
          fram_crc = CRC32.crc32((uint8_t *)str, strlen(str)); 
          sprintf(crc_buf, "%X",fram_crc); 
          pers++;
          };
        xSemaphoreGive(i2c_mutex_v);
        return; 
}

// reference values int log_head=0 , log_len=0, log_size=400, fram_base_log=2048, fram_params=2040 ;

void fram_dump(void){
  if(! has_FM24W256 ) return;

  int pntr=fram_base_log; uint8_t str[72]; 
  String my_spot;  uint16_t cntr=0;
  uint8_t a[2] = {0, 0};

  if( !(xSemaphoreTake(i2c_mutex_v, ( TickType_t ) 2) )){ debugA("fram_dump: xSemaphoreTake FAILED: aborting display update"); return;  };
  a[0] = fram.read8(fram_params);  a[1] = fram.read8(fram_params+1); log_head = word( a[1], a[0]) ;
  a[0] = fram.read8(fram_params+2);  a[1] = fram.read8(fram_params+3); log_len = word( a[1], a[0]) ;
  xSemaphoreGive(i2c_mutex_v);
  
  debugA("FRAM Log parameters: log_head=%d log_len=%d log_size=%d fram_base_log=%d \r",  log_head, log_len, log_size, fram_base_log );
  debugA("================== FRAM Log Start ==========================\r");
  while ( pntr < fram_base_log + 72*log_size){
     fram_get_record( pntr, (char *)str) ;
     debugAl("%d [%d] ==> str=[%s]\n\r",  cntr++ , pntr ,str );
     my_spot=(char *)str;
     pntr=pntr+72 ;
     };
  debugA("================== FRAM Log End ==========================\r");
  return ;
}

void log_display(void){
  
  if(! has_FM24W256 ) return;
  int pntr=fram_base_log; 
  char str[64]; 
  String my_spot;  
  uint16_t cntr=0;

  uint8_t a[2] = {0, 0};
  if( !(xSemaphoreTake(i2c_mutex_v, ( TickType_t ) 2) )){ debugA("log_display: xSemaphoreTake FAILED: aborting display update"); return;  };
  a[0] = fram.read8(fram_params);  a[1] = fram.read8(fram_params+1); log_head = word( a[1], a[0]) ;
  a[0] = fram.read8(fram_params+2);  a[1] = fram.read8(fram_params+3); log_len = word( a[1], a[0]) ;
  xSemaphoreGive(i2c_mutex_v);

  if(log_len > 400){ 
    fram_log_set();
 //   debugA("FRAM Log parameters are corrupted\r"); 
 //   return;
    };
  debugA("FRAM Log parameters: log_head=%d log_len=%d log_size=%d fram_base_log=%d \r",  log_head, log_len, log_size, fram_base_log );
  debugA("================== Log Start ==========================\r");
  if(log_len > 0){
   cntr=log_len - 1;
   float sp_lat, sp_lon;
   bool escape = false;
   while (( cntr >=0 )&& ( ! escape)){
     int offset= log_head - cntr; if(offset < 0) offset=offset + log_size;
     pntr=fram_base_log + 72*offset;
     fram_get_record( pntr, (char *)str) ;
     my_spot=(char *)str;
     String tm=my_spot.substring(0,10); unsigned long t=tm.toInt();
     String event_tag = my_spot.substring(11, 16) ;
     
     String mytime= Italy.dateTime(t, "Ymd H:i:s ");
//     debugAl("==> %s cntr=%d [pntr=%d]===> [%s] ", mytime.c_str(), cntr , pntr , my_spot.c_str() );
     debugAl("%s cntr=%d - [%s] ", mytime.c_str(), cntr , my_spot.c_str() );
     
     String Spot_Lat = my_spot.substring(11, 21) ;
     String Spot_Lon = my_spot.substring(22, 33) ;
     sp_lat = Spot_Lat.toFloat();
     sp_lon = Spot_Lon.toFloat();
     String mylat_int = Loc_Lat.substring(0, 2) ; String  mylat_frac =Loc_Lat.substring(2, 7) ; float mylat_norm = mylat_int.toFloat() + mylat_frac.toFloat() /60.0 ;
     if ( Loc_Lat.substring(7) == "S") {mylat_norm = - mylat_norm ;};
     String mylon_int = Loc_Lon.substring(0, 3) ; String  mylon_frac =Loc_Lon.substring(3, 8) ; float mylon_norm = mylon_int.toFloat() + mylon_frac.toFloat() /60.0 ;
     if ( Loc_Lon.substring(8) == "W") {mylon_norm = - mylon_norm ;};
//     unsigned long distancemTo = (unsigned long)TinyGPSPlus::distanceBetween( sp_lat, sp_lon, mylat_norm, mylon_norm);
//     debugA("=====  DISTANCE ====[%f,%f]====[%f,%f]=======> distancemTo = %lu\r", sp_lat, sp_lon, mylat_norm, mylon_norm , distancemTo);
//     debugAl("d=%lu\n\r" , distancemTo);
     debugAl("\n\r");
     if(cntr > 0) { cntr--; } else { escape = true ; };   
     };

  };   
  debugA("================== Log End ==========================\r");
  return ;
}

void area_restart(void){
   LoRa_DutyCycle = 10000.00 ;   // force area restart by simulating an overload situation... will force all nodes to restart
   return ;
}

void fram_log_reset(void){
  if(! has_FM24W256 ) return;
  
  char rec_array[71 + 1];
  debugA("Start fram log reset\r\r");
  int pntr=fram_base_log - 1;
  if( !(xSemaphoreTake(i2c_mutex_v, ( TickType_t ) 2) )){ debugA("fram_log_reset: xSemaphoreTake FAILED: aborting "); return;  };
  while ( pntr++ < fram_base_log + 72*log_size){
     fram.write8(pntr,0x00);
     debugAl("--> clearing fram_address=%d\r",pntr);
    }; 
  log_head=0;  log_len=0;
  // update log parameters
  fram.write8(fram_params,lowByte(log_head));
  fram.write8(fram_params+1 ,highByte(log_head));
  fram.write8(fram_params+2,lowByte(log_len));
  fram.write8(fram_params+3 ,highByte(log_len));
  xSemaphoreGive(i2c_mutex_v);
  debugA("Complete\r");
  return ;
}

void fram_log_set(void){
  if(! has_FM24W256 ) return;
  debugA("fixing fram_log --> before: log_head=%d log_len=%d\r",log_head, log_len);
  char rec_array[71 + 1];
  log_len=log_head;
  debugA("fixing fram_log --> after: log_head=%d log_len=%d\r",log_head, log_len);

  // update log parameters
  if( !(xSemaphoreTake(i2c_mutex_v, ( TickType_t ) 2) )){ debugA("fram_log_set: xSemaphoreTake FAILED: aborting "); return;  };
  fram.write8(fram_params,lowByte(log_head));
  fram.write8(fram_params+1 ,highByte(log_head));
  fram.write8(fram_params+2,lowByte(log_len));
  fram.write8(fram_params+3 ,highByte(log_len));
  xSemaphoreGive(i2c_mutex_v);
  return;
}

void config_enable(void){
    save_config_enable = true ;
    debugA("save_config_enable in now true\r");
}

void config_disable(void){
    save_config_enable = false ;
    debugA("save_config_enable in now false\r");
}


void print_temperature(){
  float tp = temperatureRead(); 
  time_t t = UTC.now();  // time now
  debugAl("print_temperature:  time_now=%ld ", t);  
  cpu_temp = (4.0*cpu_temp + tp)/5.0;
  debugAl(" --> ESP32 chip temperature indication = %6.2f\r", tp);
  return; 
}


void show_events(void){
  if(! has_FM24W256 ) return;
  int pntr=fram_base_log; 
  char str[64]; 
  String my_spot;  
  uint16_t cntr=0;
  uint8_t a[2] = {0, 0};
  if( !(xSemaphoreTake(i2c_mutex_v, ( TickType_t ) 2) )){ debugA("show_events: xSemaphoreTake FAILED: aborting "); return;  };
  a[0] = fram.read8(fram_params);  a[1] = fram.read8(fram_params+1); log_head = word( a[1], a[0]) ;
  a[0] = fram.read8(fram_params+2);  a[1] = fram.read8(fram_params+3); log_len = word( a[1], a[0]) ;
  xSemaphoreGive(i2c_mutex_v);
  if(log_len > 400){ debugA("FRAM Log parameters are corrupted\r"); return;};
  debugA("FRAM Log parameters: log_head=%d log_len=%d log_size=%d fram_base_log=%d \r",  log_head, log_len, log_size, fram_base_log );
  debugA("================== Events Log Start ==========================\r");
  cntr=log_len - 1;
  float sp_lat, sp_lon;
  while ( cntr >0 ){
     int offset= log_head - cntr; if(offset < 0) offset=offset + log_size;
     pntr=fram_base_log + 72*offset;
     fram_get_record( pntr, (char *)str) ;
     my_spot=(char *)str;
     String tm=my_spot.substring(0,10); unsigned long t=tm.toInt();
     String event_tag = my_spot.substring(11, 16) ;
     if( event_tag == "EVENT"){
        String mytime= Italy.dateTime(t, "Ymd H:i:s.v ");
//        debugAl("==> %s cntr=%d [pntr=%d]===> [%s]\n\r", mytime.c_str(),  cntr , pntr , my_spot.c_str());
        debugAl("==> %s cntr=%d ==> [%s]\n\r", mytime.c_str(),  cntr , my_spot.c_str());
        };
     cntr--;
     };
  debugA("================== Events Log End ==========================\r");
  return;
}
