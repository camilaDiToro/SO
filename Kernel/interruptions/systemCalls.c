#include <stdint.h>
#include <naiveConsole.h>
#include <systemCalls.h>
#include <keyboard.h>

#define ERR_COLOR 0x04
#define STD_COLOR 0x0F

typedef int (*sys_function)(uint64_t,char *,uint64_t);
static sys_function syscalls[255]={&sys_read,&sys_write};

int sys_write(uint64_t fd, char * buffer, uint64_t size) {
  if (buffer == 0 || size == 0 || fd > 2)
      return -1;


  uint8_t fontColor = (fd == STDERR) ? ERR_COLOR : STD_COLOR;

  uint64_t i = 0;

  while(i < size && buffer[i])
    ncPrintCharFormat(buffer[i++], fontColor);

  return i;
}

int sys_read(uint64_t fd, char * buffer, uint64_t size) {
  if (buffer == 0 || size == 0 || fd != 0)
      return -1;

    uint8_t i = 0;
    int c;

    while( i<size  &&  ((c = getChar()) != -1)  ){
      buffer[i++] = c;
    }
    return i;
}


int sysCallDispatcher(uint64_t fd, char * buffer, uint64_t size, uint64_t syscall_id) {
    sys_function syscall = syscalls[syscall_id];
    if (syscall != 0)
        return syscall(fd, buffer, size);
    return -1;
}
