
#ifndef SYSTEMCALLS_H_
#define SYSTEMCALLS_H_

#include <stdint.h>

#define STDIN  0
#define STDOUT 1
#define STDERR 2

int sys_write(uint64_t fd, char * buffer, uint64_t size, uint64_t screen_id);
int sys_read(uint64_t fd, char * buffer, uint64_t size);
void sys_date(char * buffer);
int sys_hasTicked();
void sys_time(char * buffer);
void sys_clearWindow(uint8_t id);
void sys_restartCursor(uint8_t id);
void sys_divide();
void sys_uniqueWindow();

#endif /* SYSTEMCALLS_H_ */
