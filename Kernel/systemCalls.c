/* Standard library */
#include <stdint.h>
#include <stddef.h>
#include <sys/types.h>

/* Local headers */
#include <graphics.h>
#include <systemCalls.h>
#include <keyboard.h>
#include <time.h>
#include <memoryManager.h>
#include <lib.h>

ssize_t sys_write(int fd, const char* buffer, size_t size) {
    if (buffer == 0 || size == 0 || fd > 2) {
        return -1;
    }

    const TColor* col = ((fd == STDERR) ? &RED : &WHITE);
    size_t i = 0;
    while (i < size && buffer[i])
        scr_printCharFormat(buffer[i++], col, &BLACK);

    return i;
}

ssize_t sys_read(int fd, char* buffer, size_t size) {
    if (buffer == 0 || size == 0 || fd != 0)
        return -1;

    uint8_t i = 0;
    int c;

    while (i < size && ((c = kbd_getChar()) != -1))
        buffer[i++] = c;

    return i;
}

void sys_date(char* buffer) {
    rtc_getCurrentDateString(buffer);     
}

void sys_time(char* buffer) {
    rtc_getCurrentTimeString(buffer);
}

int sys_hasTicked() {
    static unsigned long last_tick = 0;
    unsigned long current_tick = rtc_getElapsedTicks();
    if (last_tick == current_tick)
        return 0;

    last_tick = current_tick;
    return 1;
}

void sys_clearWindow() {
    scr_clear();
}

void sys_restartCursor() {
    scr_restartCursor();
}

void sys_setScreen(uint8_t id) {
    scr_print("no.\n");
}

void sys_divide() {
    scr_print("no.\n");
}

void sys_uniqueWindow() {
    scr_init();
}

int sys_printmem(void* address) {
    if ((uint64_t)address > (0x20000000 - 32))
        return -1;

    for (int i = 0; i < 32; ++i) {
        scr_printHex((uint64_t)address);
        scr_print(" = ");
        scr_printHex(*((uint8_t*)address));
        scr_printLine();
        address++;
    }

    return 0;
}

static char store[900];
void store_registers(uint64_t* start) {

    char* reg_text[] = {"RAX: 0x", "R15: 0x", "R14: 0x", "R13: 0x", "R12: 0x", "R11: 0x", "R10: 0x", "R9:  0x",
                        "R8:  0x", "RSI: 0x", "RDI: 0x", "RBP: 0x", "RDX: 0x", "RCX: 0x", "RBX: 0x", "RSP: 0x", 0};
    uint32_t j = 0; // store counter

    for (int i = 0; reg_text[i]; ++i) {
        // Agregamos el string al store
        int m = 0;
        while (reg_text[i][m])
            store[j++] = reg_text[i][m++];

        // Agregamos el nro al store
        uint64_t aux = start[i];
        int64_t count = 16;
        while (aux) {
            aux = aux >> 4;
            --count;
        }
        for (int k = 0; k < count; k++) {
            store[j++] = '0';
        }

        if (start[i]) {
            j += uintToBase(start[i], store + j, 16);
        }
        store[j++] = '\n';
    }
    store[j] = 0;
}

void sys_infoReg() {
    if (!store[0])
        scr_print("Debes presionar '-' para guardar el estado de los registros en un momento especifico \n");

    scr_print(store);
}

void* sys_malloc(size_t size) {
    return mm_malloc(size);
}

int sys_free(void* ptr) {
    return mm_free(ptr);
}

// Note: r10 & r8 are used for screen id and syscall id respectively.
uint64_t sysCallDispatcher(uint64_t rdi, uint64_t rsi, uint64_t rdx, uint64_t r10, uint64_t r8) {
    switch (r8) {
        case 0:
            return (uint64_t) sys_read(rdi, (char*)rsi, rdx);

        case 1:
            return (uint64_t) sys_write(rdi, (const char*)rsi, rdx);

        case 2:
            rtc_getCurrentTimeString((char*)rdi);
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
            return sys_printmem((void*)rdi);

        case 9:
            sys_setScreen(rdi);
            return 0;

        case 10:
            sys_date((char*)rdi);
            return 0;

        case 11:
            sys_infoReg();
            return 0;

        case 12:
            return (uint64_t)sys_malloc(rdi);

        case 13:
            return sys_free((void*)rdi);
    }

    return -1;
}