#ifndef _TIME_H_
#define _TIME_H_

/* Standard library */
#include <stdint.h>

/* Constants */
#define TICKS_TO_SECONDS(x) ((x) * 5 / 91)
#define TICKS_TO_MILLISECONDS(x) ((x) * 5000 / 91)

/**
 * @brief Increments ticks
 */
void rtc_interruptHandler();

/**
 * @brief Returns the amount of ticks
 */
unsigned long rtc_getElapsedTicks();

/**
 * @brief Returns the current seconds
 */
uint8_t rtc_getCurrentSeconds();

/**
 * @brief Returns the current minutes.
 */
uint8_t rtc_getCurrentMinutes();

/**
 * @brief Returns the current hour.
 */
uint8_t rtc_getCurrentHours();

/**
 * @brief Returns the current day.
 */
uint8_t rtc_getCurrentDay();

/**
 * @brief Returns the current month.
 */
uint8_t rtc_getCurrentMonth();

/**
 * @brief Returns the current year.
 */
uint8_t rtc_getCurrentYear();

/**
 * @brief Returns the current date in format "DD/MM/YY"
 */
void rtc_getCurrentDateString(char* buffer);

/**
 * @brief Returns the current hour in format "HH:MM:SS"
 */
void rtc_getCurrentTimeString(char* buffer);

#endif
