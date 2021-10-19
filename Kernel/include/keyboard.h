#ifndef KEYBOARD_H
#define KEYBOARD_H

#include <stdint.h>

unsigned int getScanCode();
unsigned char ctoi(unsigned char mChar);
char tryKeyboard();
void keyboard_handler();
int getChar();
void clear_buffer();

#endif
