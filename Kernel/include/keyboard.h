#ifndef KEYBOARD_H
#define KEYBOARD_H

#include <stdint.h>

unsigned int getScanCode();
unsigned char ctoi(unsigned char mChar);
char tryKeyboard();
void keyboard_handler();
uint16_t copy_from_buffer(char * buf, uint16_t count);

#endif
