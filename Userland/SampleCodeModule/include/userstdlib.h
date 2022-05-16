#ifndef _USERSTDLIB_H_
#define _USERSTDLIB_H_

/* Standard library */
#include <stdint.h>
#include <stddef.h>

int getChar();

int putChar(char c);

int fgetChar(int fd);

int fputChar(int fd, char c);

int print(const char* str);

int fprint(int fd, const char* str);

void printf(const char* frmt, ...);

void fprintf(int fd, const char* frmt, ...);

int atoi(const char* str);

void invalidOp();

void divideByZero();

#endif
