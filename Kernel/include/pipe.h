#ifndef _PIPE_H_
#define _PIPE_H_

/* Standard library */
#include <stdint.h>
#include <stddef.h>
#include <sys/types.h>

typedef struct TPipeInternal* TPipe;

/**
 * Creates a new pipe.
 */
TPipe pipe_create();

/**
 * Frees all resources used by a pipe. Using a pipe after it was
 * freed results in undefined behaviour.
 * 
 * Returns 0 if the operation was successful.
 */
int pipe_free(TPipe pipe);

/**
 * Writes up to count bytes from the given buffer into a pipe.
 * Returns the amount of bytes written, or -1 if an error occurred.
 */
ssize_t pipe_write(TPipe pipe, const void* buf, size_t count);

/**
 * Reads up to count bytes from a pipe into the given buffer.
 * Returns the amount of bytes read, or -1 if an error occurred.
 */
ssize_t pipe_read(TPipe pipe, void* buf, size_t count);

/**
 * Gets the amount of bytes available for reading through a pipe.
 */
size_t pipe_getRemainingBytes(TPipe pipe);

void pipe_printDebug(TPipe pipe);

#endif