/* Standard library */
#include <stdint.h>
#include <stddef.h>
#include <sys/types.h>

/* Local headers */
#include <keyboard.h>
#include <lib.h>
#include <process.h>
#include <scheduler.h>
#include <waitQueueADT.h>

#define LEFT_SHIFT 0x2A
#define RIGHT_SHIFT 0x36
#define BUFFER_MAX_SIZE 256
#define CAPTURE_REGISTERS '-'

// libasm.asm
extern void save_registers();
extern unsigned int kbd_readKey();

static uint8_t keyMapRow = 0;
static uint8_t buffer[BUFFER_MAX_SIZE];

static uint16_t bufferStart = 0;
static uint16_t bufferSize = 0;

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

static TWaitQueue processReadWaitQueue;

static ssize_t fdReadHandler(TPid pid, int fd, void* resource, char* buf, size_t count);
static int fdCloseHandler(TPid pid, int fd, void* resource);

void kbd_init() {
    processReadWaitQueue = wq_new();
}

void kbd_interruptHandler() {
    unsigned char code = kbd_readKey();
    if (code < 0x80) { // Key pressed
        if (code == LEFT_SHIFT || code == RIGHT_SHIFT) {
            keyMapRow |= 0x01;
        } else {
            uint8_t keyChar = keyMap[keyMapRow][code];
            if (keyChar == CAPTURE_REGISTERS) {
                // Inforeg - if it's the special key that save registers
                save_registers();
            } else if (keyChar != 0 && bufferSize < BUFFER_MAX_SIZE) {
                uint16_t bufferEnd = (bufferStart + bufferSize) % BUFFER_MAX_SIZE;
                buffer[bufferEnd] = keyChar;
                bufferSize++;
                wq_unblockSingle(processReadWaitQueue);
            }
        }
    } else { // Key released
        code -= 0x80;
        if (code == LEFT_SHIFT || code == RIGHT_SHIFT) {
            keyMapRow &= 0xFE;
        }
    }
}

unsigned int kbd_readChars(char* buf, unsigned int count) {
    unsigned int charsToRead = bufferSize;
    if (charsToRead > count)
        charsToRead = count;

    if (charsToRead == 0)
        return 0;

    unsigned int firstReadSize = BUFFER_MAX_SIZE - bufferStart;
    if (firstReadSize > charsToRead)
        firstReadSize = charsToRead;
    memcpy(buf, buffer + bufferStart, firstReadSize);

    if (firstReadSize < charsToRead)
        memcpy(buf + firstReadSize, buffer, charsToRead - firstReadSize);

    bufferSize -= charsToRead;
    bufferStart = (bufferStart + charsToRead) % BUFFER_MAX_SIZE;
    return charsToRead;
}

int kbd_getChar() {
    if (bufferSize == 0)
        return -1;

    int c = buffer[bufferStart];
    bufferSize--;
    bufferStart = (bufferStart + 1) % BUFFER_MAX_SIZE;
    return c;
}

void kbd_clearBuffer() {
    bufferStart = 0;
    bufferSize = 0;
}

int kbd_mapToProcessFd(TPid pid, int fd) {
    int r = prc_mapFd(pid, fd, (void*)1, &fdReadHandler, NULL, &fdCloseHandler);
    if (r < 0)
        return r;

    // TODO: process tracking? "Who is using the keyboard" so we can print them?

    return r;
}

static ssize_t fdReadHandler(TPid pid, int fd, void* resource, char* buf, size_t count) {
    // Only foreground processes are allowed to read from the keyboard.
    if (!prc_isForeground(pid))
        return -1;

    if (count == 0)
        return 0;

    if (count > BUFFER_MAX_SIZE)
        count = BUFFER_MAX_SIZE;

    int read;
    while ((read = kbd_readChars(buf, count)) == 0) {
        wq_add(processReadWaitQueue, pid);
        sch_blockProcess(pid);
        sch_yieldProcess();
    }

    return read;
}

static int fdCloseHandler(TPid pid, int fd, void* resource) {
    wq_remove(processReadWaitQueue, pid);
    return 0;
}