#include <RTClock.h>

#define SECONDS 0x00
#define MINUTES 0x02
#define HOURS 0x04
#define DAY 0x07
#define MONTH 0x08
#define YEAR 0x09
#define TIMEZONE -3

static unsigned int bcdToDec(unsigned char time);
extern unsigned int sys_RTClock(unsigned char mode);

unsigned int get_seconds(){
    return bcdToDec(sys_RTClock(SECONDS)) ;
}

unsigned int get_minutes(){
    return bcdToDec(sys_RTClock(MINUTES));
}

unsigned int get_hours(){
    return (bcdToDec(sys_RTClock(HOURS)) + 24 + TIMEZONE) % 24;
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

void get_date(char * buffer){
  char date[] = {'0','0','/','0','0','/','0','0',0};

  int t = get_day();
  date[0] += t/10;
  date[1] += t%10;
  t = get_month();
  date[3] += t/10;
  date[4] += t%10;
  t = get_year();
  date[6] += t/10;
  date[7] += t%10;

  int i;
  for(i=0 ; date[i] ; i++){
    buffer[i] = date[i];
  }
  buffer[i] = 0;
}

void get_time(char * buffer){
  char time[] = {'0','0',':','0','0',':','0','0',0};

  int t = get_hours();
  time[0] += t/10;
  time[1] += t%10;
  t = get_minutes();
  time[3] += t/10;
  time[4] += t%10;
  t = get_seconds();
  time[6] += t/10;
  time[7] += t%10;

  int i;
  for(i=0 ; time[i] ; i++){
    buffer[i] = time[i];
  }
  buffer[i] = 0;
}

// Retrieved from https://stackoverflow.com/questions/28133020/how-to-convert-bcd-to-decimal
static unsigned int bcdToDec(unsigned char time){
    return (time >> 4) * 10 + (time & 0x0F);
}
