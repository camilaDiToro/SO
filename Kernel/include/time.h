#ifndef _TIME_H_
#define _TIME_H_

/* Standard library */
#include <stdint.h>

#define TICKS_TO_SECONDS(x) ((x) * 5 / 91)
#define TICKS_TO_MILLISECONDS(x) ((x) * 5000 / 91)

void rtc_interruptHandler();

unsigned long rtc_getElapsedTicks();
unsigned long rtc_getElapsedSeconds();

uint8_t rtc_getCurrentSeconds();
uint8_t rtc_getCurrentMinutes();
uint8_t rtc_getCurrentHours();
uint8_t rtc_getCurrentDay();
uint8_t rtc_getCurrentMonth();
uint8_t rtc_getCurrentYear();
void rtc_getCurrentDateString(char* buffer);
void rtc_getCurrentTimeString(char* buffer);

#endif
