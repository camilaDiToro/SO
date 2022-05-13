#ifndef _SYSCALLS_H_
#define _SYSCALLS_H_

/* Standard library */
#include <stddef.h>
#include <sys/types.h>

#define STDIN 0
#define STDOUT 1
#define STDERR 2

// TO DO: Solve TPid & TProcessEntryPoint include

ssize_t sys_read(int fd, char* buffer, size_t size);

ssize_t sys_write(int fd, const char* buffer, size_t size);

void sys_time(char* buffer);

int sys_close(int fd);

void sys_clear();

// int sys_kill(TPid pid);

// int sys_block(TPid pid);

// int sys_unblock(TPid pid);

int sys_printmem(void* mem_address);

// int sys_create(TProcessEntryPoint entryPoint, int argc, const char* const argv[]);

void sys_date(char* buffer);

void sys_inforeg();

void* sys_malloc(size_t size);

int sys_free(void* ptr);

void* sys_realloc(void* ptr, size_t size);

void sys_yield();

// TPid sys_getPid();

// int sys_nice(TPid pid, TPriority newPriority);

int sys_exit();

int sys_pipe(int pipefd[2]);

#endif