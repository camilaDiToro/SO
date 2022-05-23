#ifndef _STRING_H_
#define _STRING_H_

/* Standard library */
#include <stddef.h>

#define MAX_ARGS 10

size_t strlen(const char* s);

char* strcpy(char* dest, const char* src);

char* strncpy(char* dest, const char* src, size_t size);

int strcmp(const char* s1, const char* s2);

int strdiv(char* str, char** other, char token);

char *strchr(char *str, char c);

int strchrCounter(char* str, char token);

int parseCommand(char *str, char *argv[]);

#endif