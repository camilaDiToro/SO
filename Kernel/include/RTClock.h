#ifndef _RTCLOCK_H
#define _RTCLOCK_H

unsigned int get_seconds();
unsigned int get_minutes();
unsigned int get_hours();
unsigned int get_day();
unsigned int get_month();
unsigned int get_year();
void get_date(char* buffer);
void get_time(char* buffer);

#endif
