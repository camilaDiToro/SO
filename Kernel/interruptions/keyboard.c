#include <keyboard.h>
#include <naiveConsole.h>

#define LEFT_SHIFT  0x2A
#define RIGHT_SHIFT 0x36

#define LEFT_SHIFT_FLAG 0b00000001
#define RIGHT_SHIFT_FLAG 0b00000010

#define BUFFER_LENGHT 256

static char keyMapRow = 0;
static uint8_t buffer[BUFFER_LENGHT];
extern unsigned int sys_readKey();

uint16_t start = 0;
uint16_t end = 0;

// Us International QWERTY

static uint8_t scancodeLToAscii[] = {

      0,   27, '1', '2', '3', '4', '5', '6', '7', '8', '9',  '0', '-', '=',
   '\b', '\t', 'q', 'w', 'e', 'r', 't', 'y', 'u', 'i', 'o', 'p',  '[', ']',
   '\n',    0, 'a', 's', 'd', 'f', 'g', 'h', 'j', 'k', 'l', ';', '\'', '`',
      0, '\\', 'z', 'x', 'c', 'v', 'b', 'n', 'm', ',', '.', '/',    0, '*',
      0,  ' ',   0,   0,   0,   0,   0,   0,   0,   0,   0,   0,    0,   0,
      0,    0,   0,   0, '-',   0,   0,   0, '+',   0,   0,   0,    0,   0,
      0,    0,   0,   0,   0,   0

};

static uint8_t scancodeUToAscii[] = {

      0,   27, '!', '@', '#', '$', '%', '^', '&', '*', '(', ')', '_', '+',
   '\b', '\t', 'Q', 'W', 'E', 'R', 'T', 'Y', 'U', 'I', 'O', 'P', '{', '}',
   '\n',    0, 'A', 'S', 'D', 'F', 'G', 'H', 'J', 'K', 'L', ':', '"', '~',
      0, '|', 'Z', 'X', 'C', 'V', 'B', 'N', 'M', '<', '>', '?',   0, '*',
      0, ' ',    0,   0,   0,   0,   0,   0,   0,   0,   0,   0,   0,   0,
      0,   0,    0,   0, '-',   0,   0,   0, '+',   0,   0,   0,   0,   0,
      0,   0,    0,   0,   0,   0

};

static uint8_t * keyMap[] = {scancodeLToAscii, scancodeUToAscii};



unsigned int getScanCode(){
  return sys_readKey();
}

// Funcion provisoria para ver si funca la conversion
unsigned char ctoi(unsigned char mChar){
    return keyMap[0][mChar];
}

// El buffer va a ser una lista circular implementada con arrays como vimos en eda
// Ahora es una pedorrada pero por eso hay un start y un end
// Hay que ver que pasa cuando se llena el buffer
void addBuffer(uint8_t c){

  buffer[end++] = c;

  if(end == BUFFER_LENGHT)
    end = 0;
  return;
}

// Implementacion en proceso para detectar combinacion de shift y tecla.
// Por ahora leemos 20 cosas, en un futuro vamos a ir leyendo y llenando el buffer
// Usando una interrupcion
void keyboard_handler()
{
	unsigned char code = sys_readKey();
	if(code < 0x80){    // Key pressed
    if(code == LEFT_SHIFT || code == RIGHT_SHIFT){
      keyMapRow|=0x01;
    }
    else if(keyMap[keyMapRow][code]!=0){
      ncPrintChar(keyMap[keyMapRow][code]);
      addBuffer(keyMap[keyMapRow][code]);
    }

	} else {               // Key released
    code-=0x80;
    if(code == LEFT_SHIFT || code == RIGHT_SHIFT){
      keyMapRow&=0xFE;
    }
	}
  return;
}

uint16_t copy_from_buffer(char * buf, uint16_t count) {

  uint16_t i = 0;

  while (i < count && i< end)  // TODO: Check if we keep the last \n
    buf[i] = buffer[i++];

  return i;
}
