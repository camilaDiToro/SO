#ifndef _SYSTEM_CALLS_H_
#define _SYSTEM_CALLS_H_

/* Standard library */
#include <stdint.h>
#include <stddef.h>
#include <sys/types.h>

#define STDIN 0
#define STDOUT 1
#define STDERR 2

ssize_t sys_write(int fd, const char* buffer, size_t size);
ssize_t sys_read(int fd, char* buffer, size_t size);
int sys_hasTicked();
void sys_time(char* buffer);
void sys_date(char* buffer);
void sys_clearWindow();
void sys_restartCursor();
void sys_divide();
void sys_uniqueWindow();
int sys_printmem(void* mem_address);
void sys_setScreen(uint8_t id);
void sys_infoReg();

void store_registers(uint64_t* start);

#endif
