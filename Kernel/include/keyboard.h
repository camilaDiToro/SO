#ifndef _KEYBOARD_H_
#define _KEYBOARD_H_

/* Local headers */
#include <kernelTypes.h>

void kbd_init();

/**
 * @brief Called by the interrupt dispatcher whenever a keyboard interrupt occurs.
 */
void kbd_interruptHandler();

/**
 * @brief Attempts to read up to count chars from the keyboard buffer, and write
 * them to the given buffer.
 * 
 * @returns The amount of chars read.
 */
unsigned int kbd_readChars(char* buf, unsigned int count);

/**
 * @brief Attempts to read a single char from the keyboard buffer.
 * 
 * @returns The char read from the buffer, or -1 if the buffer was empty.
 */
int kbd_getChar();

/**
 * @brief Clears the keyboard buffer, setting it to empty until new keys arrive.
 */
void kbd_clearBuffer();

/**
 * @brief Maps the keyboard (as text input) onto a process' I/O table.
 * 
 * @returns The file descriptor on which the keyboard was mapped for the
 * process, or -1 if an error occurred.
 */
int kbd_mapToProcessFd(TPid pid, int fd);

#endif
