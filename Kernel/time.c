/* Standard library */
#include <stdint.h>

/* Local headers */
#include <time.h>
#include <lib.h>

#define SECONDS 0x00
#define MINUTES 0x02
#define HOURS 0x04
#define DAY 0x07
#define MONTH 0x08
#define YEAR 0x09

/** Defined in time.asm */
extern uint8_t rtc_readValue(uint8_t mode);

static unsigned long ticks = 0;

void rtc_interruptHandler() {
    ticks++;
}

unsigned long rtc_getElapsedTicks() {
    return ticks;
}

unsigned long rtc_getElapsedSeconds() {
    return TICKS_TO_SECONDS(ticks);
}

uint8_t rtc_getCurrentSeconds() {
    return bcdToDec(rtc_readValue(SECONDS));
}

uint8_t rtc_getCurrentMinutes() {
    return bcdToDec(rtc_readValue(MINUTES));
}

uint8_t rtc_getCurrentHours() {
    return bcdToDec(rtc_readValue(HOURS));
}

uint8_t rtc_getCurrentDay() {
    return bcdToDec(rtc_readValue(DAY));
}

uint8_t rtc_getCurrentMonth() {
    return bcdToDec(rtc_readValue(MONTH));
}

uint8_t rtc_getCurrentYear() {
    return bcdToDec(rtc_readValue(YEAR));
}

void rtc_getCurrentDateString(char* buffer) {
    char date[] = {'0', '0', '/', '0', '0', '/', '0', '0', 0};

    int t = rtc_getCurrentDay();
    date[0] += t / 10;
    date[1] += t % 10;
    t = rtc_getCurrentMonth();
    date[3] += t / 10;
    date[4] += t % 10;
    t = rtc_getCurrentYear();
    date[6] += t / 10;
    date[7] += t % 10;

    int i;
    for (i = 0; date[i]; i++) {
        buffer[i] = date[i];
    }
    buffer[i] = 0;
}

void rtc_getCurrentTimeString(char* buffer) {
    char time[] = {'0', '0', ':', '0', '0', ':', '0', '0', 0};

    int t = rtc_getCurrentHours();
    time[0] += t / 10;
    time[1] += t % 10;
    t = rtc_getCurrentMinutes();
    time[3] += t / 10;
    time[4] += t % 10;
    t = rtc_getCurrentSeconds();
    time[6] += t / 10;
    time[7] += t % 10;

    int i;
    for (i = 0; time[i]; i++) {
        buffer[i] = time[i];
    }
    buffer[i] = 0;
}