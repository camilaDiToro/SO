#ifndef _PROCESS_H_
#define _PROCESS_H_

/* Standard library */
#include <stdint.h>

/* Local headers */
#include <pipe.h>

#define PROCESS_STACK_SIZE 4096
#define MAX_PROCESSES 16

typedef int TPid;

typedef void (*TProcessEntryPoint)(int argc, const char* argv[]);

/**
 * @brief Creates a new process from a given entry point and arguments list.
 *
 * @returns The newly created TProcess, or NULL if process creation failed.
 */
TPid prc_create(TProcessEntryPoint entryPoint, int argc, const char* argv[]);

/**
 * @brief Kills a process and frees associated resources. Using a process after
 * it has been killed results in undefined behaviour.
 *
 * @returns 0 if the operation was successful.
 */
int prc_kill(TPid pid);

/**
 * @brief Maps a pipe onto a process' I/O table.
 *
 * @returns The file descriptor on which the pipe was mapped for the process,
 * or -1 if an error occurred.
 */
int prc_mapFd(TPid pid, TPipe pipe, int isReadEnd);

/**
 * @brief Unmaps a file descriptor from a process' I/O table.
 *
 * @returns 0 if the operation succeeded.
 */
int prc_unmapFd(TPid pid, int fd);

/**
 * @brief Unmaps a file descriptor from a process' I/O table.
 *
 * @returns The pipe mapped to the given file descriptor, or NULL
 * if the operation failed.
 */
TPipe prc_getFdMapping(TPid pid, int fd, int* outIsReadEnd);

#endif