#ifndef _USERSTDLIB_H_
#define _USERSTDLIB_H_

/* Standard library */
#include <stdint.h>
#include <stddef.h>

int getChar();

int putChar(int fd, char c);

int readChar();

int sprint(int fd, const char* str);

void printf(const char* frmt, ...);

char* convert(unsigned int num, unsigned int base);

int atoi(const char* str);

void invalidOp();

void divideByZero();

#endif
