#include <stdint.h>
#include <userstdlib.h>

#define STDIN 0
#define STDOUT 1
#define STDERR 2

extern int  sys_write(uint64_t fd, char * buffer, uint64_t size);
extern int  sys_read(uint64_t fd, char * buffer, uint64_t size);
extern void sys_time(char * buffer);



int _strlen(const char * str){
    int i=0;
    while( *(str + (i++)) );
    return i-1;
}

int strcmp(char * s1, char * s2) {
    int i;
    for (i = 0; s1[i] && s1[i] == s2[i]; i++);
    return s1[i] - s2[i];
}

int sprint(uint8_t fd, char * str){
  return sys_write(fd, str, _strlen(str));
}

int put_char(uint8_t fd, char c){
  return sys_write(fd, &c, 1);
}

void get_time(char * buffer){
  sys_time(buffer);
}

int get_char(){
    char c;
    int returnval;
    do{
      returnval = sys_read(0, &c, 1);
    }while(returnval!=1);

	  return c;
}
