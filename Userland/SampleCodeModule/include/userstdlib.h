#ifndef _USERSTDLIB_H_
#define _USERSTDLIB_H_

#include <stdint.h>

int put_char(uint8_t fd, char c);
int sprint(uint8_t fd, char * str);
int get_char();
int read_char();
void get_time(char * buffer);
void get_date(char * buffer);
int _strlen(const char * str);
int strcmp(char * s1, char * s2);
int tick();
void clearScreen();
void restartCursor();
void divideWindow();
void uniqueWindow();
void divideByZero();
void invalidOp();
void setScreen(uint8_t id);
void printMem();


/*
void my_printf(const char * frmt, ...);
char *convert(unsigned int, int);
int my_scanf(const char * frmt, ...);
int my_atoi (char* s);
*/

#endif /* _USERSTDLIB_H_*/
