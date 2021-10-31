#include <stdint.h>
#include <naiveConsole.h>
#include <graphicMode.h>
#include <systemCalls.h>
#include <keyboard.h>
#include <time.h>
#include <RTClock.h>

color_t STD = {0xFF,0xFF,0xFF};
color_t BG = {0x00,0x00,0x00};
color_t ERR = {0xFF,0x00,0x00};

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




int sys_hasTicked(){
    static unsigned long last_tick = 0;
    unsigned long current_tick = ticks_elapsed();
    if(last_tick == current_tick)
        return 0;
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
  printHex(480);
  printChar(' ');
  printHex((uint64_t)mem_address);
  printChar(' ');
  printDec((uint64_t)mem_address);
  printChar('\n');


  // Podemos leer hasta 512MB = 2^9 * 2^20 = 2 * 2^28 = 20000000h
  // Arranca en 0 entonces podemos acceder hasta 20000000h - 1

  if( (uint64_t) mem_address  > (0x20000000 - 32) ){
    return -1;
  }

  // Primero les digo lo que vi cuando lo corri:
  // Esta raro el resultado que sale. Se supone que cada linea tiene que
  // imprimir 1byte y hay nros de muchos digitos hexa
  // (solo deberiamos ver 1 byte por renglon i.e 2 digitos hexa maximo)
  // Por que? porque queremos imprimir 32 bytes y deberia haber
  // 32 renglones de 1byte.

  // El problema de este codigo esta en el manejo de punteros. Esta todo
  // perfecto pero no se tiene en cuenta que es sumarle 1 a un puntero.
  // Digo, que es sumarle 1 a mem_address? Para responder a esto hay q
  // Tener en cuenta que el tipo de dato de mem_address es un int de 8bytes
  // Entonces, sumarle 1 es avanzar 8 posiciones de memoria y leer su valor
  // es leer 8 posiciones de memoria (por eso se ven tantos datos)

  // Esto no es lo que queremos ya que en total estariamos leyendo 8*32 = 256B
  // cuando nos piden un volcado de 32B

  // La solucion a este problema es hacer que mem_address trabaje con la memoria de a
  // "pedazos" que midan un byte, es decir, que lea un byte y que avance de a un byte

  // Esto lo podemos lograr haciendo un casteo a uint8_t

  // Hice ese cambio y no me andaba pero era porque cambiaron la implementacion
  // de la conversion de string hexa a decimal
  // Volvi a dejar la implementacion que estaba antes. Si les confunden los decalajes
  // vemos de cambiarlos y poner mult pero estamos pasando de base 16 a base 10
  // Entones, hay q ir multplicando por 16 no por 10. Decalar a derecha 4 posiciones es lo mismo
  // que multiplicar por 16.

  uint8_t * aux = mem_address;

  for(int i=0; i<32 ; ++i){
      printHex((uint64_t)aux);
      print(" = ");
      printHex(*aux);
      newLine();
      ++aux;
  }

  return 0;
}

// Note: r10 & r8 are used for screend_id and syscall_id respectively.
int sysCallDispatcher(uint64_t rdi, uint64_t rsi, uint64_t rdx, uint64_t r10, uint64_t r8) {
  switch(r8){
      case 0:
        return sys_read(rdi,rsi,rdx);

      case 1:
        return sys_write(rdi,rsi,rdx);

      case 2:
        get_time(rdi);
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
       return sys_printmem(rdi);
        return 0;

      case 9:
        sys_setScreen(rdi);
        return 0;

      case 10:
        sys_date(rdi);
        return 0;

  }
  return -1;
}
