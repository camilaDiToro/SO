#ifndef _PIPE_H_
#define _PIPE_H_

/* Standard library */
#include <stdint.h>
#include <stddef.h>
#include <sys/types.h>

/* Local headers */
#include <process.h>

typedef struct TPipeInternal* TPipe;

/**
 * @brief Creates a new pipe.
 * 
 * @returns The newly created pipe, or NULL if the operation failed.
 */
TPipe pipe_create();

/**
 * @brief Frees all resources used by a pipe. Using a pipe after it was
 * freed results in undefined behaviour.
 * 
 * @returns 0 if the operation was successful.
 */
int pipe_free(TPipe pipe);

/**
 * @brief Writes up to count bytes from the given buffer into a pipe.
 * This is a non-blocking operation, and will returns 0 if the pipe was full.
 * 
 * @returns The amount of bytes written, or -1 if an error occurred.
 */
ssize_t pipe_write(TPipe pipe, const void* buf, size_t count);

/**
 * @brief Reads up to count bytes from a pipe into the given buffer.
 * This is a non-blocking operation, and will return 0 if the pipe was empty.
 * 
 * @returns The amount of bytes read, or -1 if an error occurred.
 */
ssize_t pipe_read(TPipe pipe, void* buf, size_t count);

/**
 * @brief Maps a pipe onto a process' I/O table.
 *  
 * @returns The file descriptor on which the pipe was mapped for the process,
 * or -1 if an error occurred.
 */
int pipe_mapToProcessFd(TPid pid, int fd, TPipe pipe, int allowRead, int allowWrite);

void pipe_printDebug(TPipe pipe);

#endif