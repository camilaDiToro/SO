#ifndef _STRING_H_
#define _STRING_H_

/* Standard library */
#include <stddef.h>

size_t strlen(const char* s);

char* strcpy(char* dest, const char* src);

char* strncpy(char* dest, const char* src, size_t size);

int strcmp(const char* s1, const char* s2);

char* strcat(char* dest, const char* src);

#endif