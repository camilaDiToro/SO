#include <stdint.h>
#include <naiveConsole.h>
#include <graphicMode.h>
#include <systemCalls.h>
#include <keyboard.h>
#include <time.h>
#include <RTClock.h>


int sys_write(uint64_t fd, char * buffer, uint64_t size) {
    if (buffer == 0 || size == 0 || fd > 2){
        return -1;
    }

    color_t col = ((fd == STDERR) ? RED : WHITE);
    uint64_t i = 0;
    while(i < size && buffer[i]){
      printCharFormat(buffer[i++],&col, &BLACK);
    }
    return i;
}

int sys_read(uint64_t fd, char * buffer, uint64_t size) {
    if (buffer == 0 || size == 0 || fd != 0){
      return -1;
    }
    uint8_t i = 0;
    int c;

    while(i<size && ((c = getChar()) != -1)){
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

int sys_hasTicked(){
    static unsigned long last_tick = 0;
    unsigned long current_tick = ticks_elapsed();
    if(last_tick == current_tick){
        return 0;
    }
    last_tick = current_tick;
    return 1;
}

void sys_clearWindow(){
    clearAll();
}

void sys_restartCursor(){
    restartCursor();
}

void sys_setScreen(uint8_t id){
    setScreen(id);
}

void sys_divide(){
    initDividedWindow();
}

void sys_uniqueWindow(){
    initUniqueWindow();
}

int sys_printmem(uint64_t * mem_address){
    if((uint64_t) mem_address > (0x20000000 - 32)){
      return -1;
    }

    uint8_t * aux = (uint8_t *) mem_address;
    for(int i=0; i < 32 ; ++i){
        printHex((uint64_t) aux);
        print(" = ");
        printHex(*aux);
        newLine();
        ++aux;
    }
    return 0;
}

// Note: r10 & r8 are used for screen id and syscall id respectively.
int sysCallDispatcher(uint64_t rdi, uint64_t rsi, uint64_t rdx, uint64_t r10, uint64_t r8) {
  switch(r8){
      case 0:
        return sys_read(rdi, (char *)rsi, rdx);   

      case 1:
        return sys_write(rdi, (char *)rsi, rdx); 

      case 2:
        get_time((char *)rdi);   
        return 0;

      case 3:
        return sys_hasTicked();

      case 4:
        sys_clearWindow();
        return 0;

      case 5:
        sys_restartCursor();
        return 0;

      case 6:
        sys_divide();
        return 0;

      case 7:
        sys_uniqueWindow();
        return 0;

      case 8:
        return sys_printmem((uint64_t *) rdi);  

      case 9:
        sys_setScreen(rdi);
        return 0;

      case 10:
        sys_date((char *)rdi);   
        return 0;
  }
  return -1;
}