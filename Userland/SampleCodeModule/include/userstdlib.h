#ifndef _USERSTDLIB_H_
#define _USERSTDLIB_H_

/* Standard library */
#include <stdint.h>
#include <stddef.h>

int getChar();

int putChar(char c);

int fgetChar(int fd);

void sleep(unsigned long millis);

int fputChar(int fd, char c);

int print(const char* str);

int fprint(int fd, const char* str);

void printf(const char* frmt, ...);

void fprintf(int fd, const char* frmt, ...);

int atoi(const char* str);

int getLine(char *buffer, int maxSize);

int fgetLine(int fd, char *buffer, int maxSize);

void invalidOp();

void divideByZero();

#endif
