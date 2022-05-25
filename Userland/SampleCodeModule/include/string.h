#ifndef _STRING_H_
#define _STRING_H_

/* Standard library */
#include <stddef.h>

#define MAX_ARGS 10
#define MAX_PIPES 10

size_t strlen(const char* s);

char* strcpy(char* dest, const char* src);

char* strncpy(char* dest, const char* src, size_t size);

int strcmp(const char* s1, const char* s2);

int strchr(char* str, char c);

int parseCommandArgs(char* str, char* argv[]);

int parseCommandPipes(char* str, char* array[], char token);

#endif