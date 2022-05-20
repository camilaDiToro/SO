#ifndef _SYSCALLS_H_
#define _SYSCALLS_H_

/* Standard library */
#include <stddef.h>
#include <sys/types.h>

/* Local headers */
#include <kernelTypes.h>

#define STDIN 0
#define STDOUT 1
#define STDERR 2

ssize_t sys_read(int fd, char* buffer, size_t size);
ssize_t sys_write(int fd, const char* buffer, size_t size);
int sys_close(int fd);

void sys_clearScreen();

unsigned long sys_millis();
void sys_time(char* buffer);
void sys_date(char* buffer);

void* sys_malloc(size_t size);
int sys_free(void* ptr);
void* sys_realloc(void* ptr, size_t size);

TPid sys_getPid();
int sys_createProcess(int stdinMapFd, int stdoutMapFd, int stderrMapFd, const TProcessCreateInfo* createInfo);
int sys_exit();
int sys_block(TPid pid);
int sys_unblock(TPid pid);
void sys_yield();
int sys_killProcess(TPid pid);
int sys_nice(TPid pid, TPriority newPriority);
int sys_listProcesses(TProcessInfo* array, int maxProcesses);
int sys_waitpid(TPid pid);

int sys_pipe(int pipefd[2]);
int sys_openPipe(const char* name, int pipefd[2]);
int sys_unlinkPipe(const char* name);
int sys_listPipes(TPipeInfo* array, int maxPipes);

int sys_openSem(const char* name, TSem* sem, unsigned int value);
int sys_closeSem(TSem* sem);
int sys_unlinkSem(const char* name);
int sys_postSem(TSem* sem);
int sys_waitSem(TSem* sem);
int sys_listSemaphores(TSemaphoreInfo* array, int maxSemaphores);

#endif