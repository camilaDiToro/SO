#include <mychrono.h>

/* 18 ticks = 1 sec */
#define TICKS_PER_SECOND 18

uint8_t modify_chrono(char * chrono, uint8_t ms_ticks){
  if(ms_ticks == TICKS_PER_SECOND){
    chrono[8] = '0';
    if(chrono[6] == '9'){
      chrono[6] = '0';
      if(chrono[5] == '5'){
        chrono[5] = '0';
        if(chrono[3] == '9'){
          chrono[3] = '0';
          if(chrono[2] == '5'){
            chrono[2] = '0';
            chrono[0]++;
          }else{
            chrono[2]++;
          }
        }else{
          chrono[3]++;
        }
      }else{
        chrono[5]++;
      }
    }else{
      chrono[6]++;
    }
  }else{
    chrono[8] = '0' + (ms_ticks * 10) / TICKS_PER_SECOND;
  }
  return ms_ticks % TICKS_PER_SECOND;
}

void restart(char * chrono){
  chrono[0] = chrono[2] = chrono[3] = '0';
  chrono[5] = chrono[6] = chrono[8] = '0';
}