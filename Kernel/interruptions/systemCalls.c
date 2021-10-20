#include <stdint.h>
#include <naiveConsole.h>
#include <graphicMode.h>
#include <systemCalls.h>
#include <keyboard.h>
#include <RTClock.h>

color_t STD = {0xFF,0xFF,0xFF};
color_t BG = {0x00,0x00,0x00};
color_t ERR = {0xFF,0x00,0x00};

typedef int (*sys_function)(uint64_t,char *,uint64_t);
static sys_function syscalls[255]={&sys_read,&sys_write};

int sys_write(uint64_t fd, char * buffer, uint64_t size) {
  if (buffer == 0 || size == 0 || fd > 2)
      return -1;

  color_t col = ((fd == STDERR) ? ERR : STD);

  uint64_t i = 0;
  while(i < size && buffer[i])
    printCharFormat(buffer[i++],&col, &BG);
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

void sys_date(char * buffer){
  get_date(buffer);
}

void sys_time(char * buffer){
  get_time(buffer);
}


int sysCallDispatcher(uint64_t fd, uint64_t buffer, uint64_t size, uint64_t syscall_id) {
  switch(syscall_id){
      case 0:
        return sys_read(fd,buffer,size);

      case 1:
        return sys_write(fd,buffer,size);

      case 2:
        get_time(fd);
        return 0;

  }
  return -1;
}
