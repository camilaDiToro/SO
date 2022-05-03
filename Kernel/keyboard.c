/* Standard library */
#include <stdint.h>

/* Local headers */
#include <keyboard.h>

#define LEFT_SHIFT 0x2A
#define RIGHT_SHIFT 0x36
#define BUFFER_LENGHT 256
#define CAPTURE_REGISTERS '-'

// libasm.asm
extern void save_registers();
extern unsigned int kbd_readKey();

static uint8_t keyMapRow = 0;
static uint8_t buffer[BUFFER_LENGHT];

uint16_t buffer_start = 0;
uint16_t buffer_end = 0;
uint16_t buffer_current_size = 0;

// Us International QWERTY
// https://stanislavs.org/helppc/make_codes.html
// https://stanislavs.org/helppc/scan_codes.html

static uint8_t scancodeLToAscii[] = {
      0,   27, '1', '2', '3', '4', '5', '6', '7', '8', '9',  '0', '-', '=',
   '\b', '\t', 'q', 'w', 'e', 'r', 't', 'y', 'u', 'i', 'o', 'p',  '[', ']',
   '\n',    0, 'a', 's', 'd', 'f', 'g', 'h', 'j', 'k', 'l', ';', '\'', '`',
      0, '\\', 'z', 'x', 'c', 'v', 'b', 'n', 'm', ',', '.', '/',    0, '*',
      0,  ' ',   0,   0,   0,   0,   0,   0,   0,   0,   0,   0,    0,   0,
      0,    0,   38,   0, '-',   37,   0,   39, '+',   0,   40,   0,    0,   0,
      0,    0,   0,   0,   0,   0,   0,   0,  0,    0,   0,   0,    0,   0,
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

static uint8_t* keyMap[] = {scancodeLToAscii, scancodeUToAscii};

static void addBuffer(uint8_t c) {

    buffer[buffer_end++] = c;
    buffer_current_size++;

    if (buffer_end == BUFFER_LENGHT) {
        buffer_end = 0;
    }
    return;
}

void kbd_interruptHandler() {

    unsigned char code = kbd_readKey();
    if (code < 0x80) { // Key pressed
        if (code == LEFT_SHIFT || code == RIGHT_SHIFT) {
            keyMapRow |= 0x01;
        }
        // Inforeg - if it's the special key that save registers
        else if (keyMap[keyMapRow][code] == CAPTURE_REGISTERS) {
            save_registers();
            return;
        } else if (keyMap[keyMapRow][code] != 0) {
            addBuffer(keyMap[keyMapRow][code]);
        }

    } else { // Key released
        code -= 0x80;
        if (code == LEFT_SHIFT || code == RIGHT_SHIFT) {
            keyMapRow &= 0xFE;
        }
    }
    return;
}

void kbd_clearBuffer() {
    buffer_end = buffer_start = buffer_current_size = 0;
}

int kbd_getChar() {
    if (buffer_current_size == 0)
        return -1;

    buffer_current_size--;
    return buffer[buffer_start++];
}