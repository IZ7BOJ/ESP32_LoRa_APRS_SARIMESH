// BCN_Structs.h
#ifndef BCN_STRUCTS_H
#define BCN_STRUCTS_H

#include "rdbg.h"

#define BCN_SCHED_LEN 9

struct BeaconRunTimeConfig {
  byte BCN_SeqNbr ;   // BCN_SeqNbr
  float LoraFreq ;        // ex 433.725
  float LoraBw ;          // ex 32.25 Khz
  byte LoraSf ;            // ex. 12
  byte LoraCodingRate ;    // 8
  byte LoraSync ;          // 0x34
  signed int LoraPower ;   // 22 dbm
  int LoraFreqCorr ;       // 0
  byte LoraPreambleLen ;   // 6
}  ;

#endif
