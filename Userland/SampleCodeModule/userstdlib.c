#include <stdint.h>
#include <userstdlib.h>

#define STDIN 0
#define STDOUT 1
#define STDERR 2

extern int sys_write(uint64_t fd, char * buffer, uint64_t size);
extern int sys_read(uint64_t fd, char * buffer, uint64_t size);

int _strlen(const char * str){
    int i=0;
    while( *(str + (i++)) );
    return i-1;
}

int sprint(uint8_t fd, char * str){
  sys_write(fd, str, _strlen(str));
}

int put_char(uint8_t fd, char c){
  sys_write(fd, &c, 1);
}

int get_char(){
    char c;
    int returnval;
    do{
      returnval = sys_read(0, &c, 1);
    }while(returnval!=1);

	  return c;
}
