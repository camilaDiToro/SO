#ifndef _USERSTDLIB_H_
#define _USERSTDLIB_H_

#include <stdint.h>

int put_char(uint8_t fd, char c);
int get_char();
void get_time(char * buffer);
int _strlen(const char * str);
int sprint(uint8_t fd, char * str);
int strcmp(char * s1, char * s2);

#endif /* _USERSTDLIB_H_*/
