#ifndef _SYSCALLS_H_
#define _SYSCALLS_H_

/* Standard library */
#include <stdint.h>
#include <stddef.h>
#include <sys/types.h>

#define STDIN 0
#define STDOUT 1
#define STDERR 2

ssize_t sys_read(int fd, char* buffer, size_t size);

ssize_t sys_write(int fd, const char* buffer, size_t size);

void sys_time(char* buffer);

int sys_tick();

void sys_clear();

int sys_printmem(void* mem_address);

void sys_date(char* buffer);

void sys_inforeg();

void* sys_malloc(size_t size);

int sys_free(void* ptr);

void* sys_realloc(void* ptr, size_t size);

#endif