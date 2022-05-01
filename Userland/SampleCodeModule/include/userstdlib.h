#ifndef _USERSTDLIB_H_
#define _USERSTDLIB_H_

#include <stdint.h>

#define STDIN 0
#define STDOUT 1
#define STDERR 2

// stdio.h
int sprint(uint8_t fd, char * str);
void my_printf(const char * frmt, ...);
char *convert(unsigned int, int);

// string.h
char * my_strcpy(char * destination, char * source);
char * my_strncpy(char * destination, char * source, int size);
int _strlen(const char * str);
int strcmp(char * s1, char * s2);
int put_char(uint8_t fd, char c);
int get_char();
int read_char();

// time.h
void get_time(char * buffer);
void get_date(char * buffer);
int tick();

// stdlib.h
int printMem(uint64_t * value);
int atoi(char * str);
int mm_free(void* ptr);
void* mm_malloc(uint64_t size);

// screen.h
void clearScreen();
void restartCursor();
void divideWindow();
void uniqueWindow();
void setScreen(uint8_t id);
void infoReg();

#endif /* _USERSTDLIB_H_*/
