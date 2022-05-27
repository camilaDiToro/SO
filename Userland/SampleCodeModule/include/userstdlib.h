#ifndef _USERSTDLIB_H_
#define _USERSTDLIB_H_

/* Standard library */
#include <stddef.h>
#include <stdint.h>

void sleep(unsigned long millis);

int getChar();

int putChar(char c);

int fgetChar(int fd);

int fputChar(int fd, char c);

int print(const char* str);

int fprint(int fd, const char* str);

void printf(const char* frmt, ...);

void fprintf(int fd, const char* frmt, ...);

int atoi(const char* str);

int getLine(char* buffer, int maxSize);

int fgetLine(int fd, char* buffer, int maxSize);

void invalidOp();

void divideByZero();

#endif
