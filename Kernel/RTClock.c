#include <RTClock.h>

#define SECONDS 0x00
#define MINUTES 0x02
#define HOURS 0x04
#define DAY 0x07
#define MONTH 0x08
#define YEAR 0x09
#define TIMEZONE 0

static unsigned int bcdToDec(unsigned char time);
extern unsigned int sys_RTClock(unsigned char mode);

unsigned int get_seconds(){
    return bcdToDec(sys_RTClock(SECONDS)) ;
}

unsigned int get_minutes(){
    return bcdToDec(sys_RTClock(MINUTES));
}

unsigned int get_hours(){
    return (bcdToDec(sys_RTClock(HOURS)) + TIMEZONE) % 24;
}

unsigned int get_day(){

   unsigned int current_hour = bcdToDec(sys_RTClock(HOURS));

   if(current_hour + TIMEZONE < 0){
     return bcdToDec(sys_RTClock(DAY)) - 1;
   }else if (current_hour + TIMEZONE > 23 ){
     return bcdToDec(sys_RTClock(DAY)) + 1;
   }else{
     return bcdToDec(sys_RTClock(DAY));
   }

}

unsigned int get_month(){
   return bcdToDec(sys_RTClock(MONTH));
}

unsigned int get_year(){
    return bcdToDec(sys_RTClock(YEAR));
}

// https://stackoverflow.com/questions/28133020/how-to-convert-bcd-to-decimal
static unsigned int bcdToDec(unsigned char time){
    return (time >> 4) * 10 + (time & 0x0F);
}
