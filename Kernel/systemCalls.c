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

typedef size_t (*TSyscallHandlerFunction)(size_t rdi, size_t rsi, size_t rdx, size_t r10, size_t r8);

ssize_t sys_write_handler(int fd, const char* buffer, size_t size) {
    if (buffer == 0 || size == 0 || fd > 2) {
        return -1;
    }

    const TColor* col = ((fd == STDERR) ? &RED : &WHITE);
    size_t i = 0;
    while (i < size && buffer[i])
        scr_printCharFormat(buffer[i++], col, &BLACK);

    return i;
}

ssize_t sys_read_handler(int fd, char* buffer, size_t size) {
    if (buffer == 0 || size == 0 || fd != 0)
        return -1;

    uint8_t i = 0;
    int c;

    while (i < size && ((c = kbd_getChar()) != -1))
        buffer[i++] = c;

    return i;
}

int sys_date_handler(char* buffer) {
    rtc_getCurrentDateString(buffer);
    return 0;    
}

int sys_time_handler(char* buffer) {
    rtc_getCurrentTimeString(buffer);
    return 0;
}

int sys_hasTicked_handler() {
    static unsigned long last_tick = 0;
    unsigned long current_tick = rtc_getElapsedTicks();
    if (last_tick == current_tick)
        return 0;

    last_tick = current_tick;
    return 1;
}

int sys_clearWindow_handler() {
    scr_clear();
    return 0;
}

int sys_restartCursor_handler() {
    scr_restartCursor();
    return 0;
}

int sys_setScreen_handler(uint8_t id) {
    scr_print("no.\n");
    return 0;
}

int sys_divide_handler() {
    scr_print("no.\n");
    return 0;
}

int sys_uniqueWindow_handler() {
    scr_init();
    return 0;
}

int sys_printmem_handler(void* address) {
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

int sys_infoReg_handler() {
    if (!store[0])
        scr_print("Debes presionar '-' para guardar el estado de los registros en un momento especifico \n");

    scr_print(store);
    return 0;
}

void* sys_malloc_handler(size_t size) {
    return mm_malloc(size);
}

int sys_free_handler(void* ptr) {
    return mm_free(ptr);
}

static TSyscallHandlerFunction syscallHandlers[] = {
    /*  0 */ (TSyscallHandlerFunction) sys_read_handler,
    /*  1 */ (TSyscallHandlerFunction) sys_write_handler,
    /*  2 */ (TSyscallHandlerFunction) sys_time_handler,
    /*  3 */ (TSyscallHandlerFunction) sys_hasTicked_handler,
    /*  4 */ (TSyscallHandlerFunction) sys_clearWindow_handler,
    /*  5 */ (TSyscallHandlerFunction) sys_restartCursor_handler,
    /*  6 */ (TSyscallHandlerFunction) sys_divide_handler,
    /*  7 */ (TSyscallHandlerFunction) sys_uniqueWindow_handler,
    /*  8 */ (TSyscallHandlerFunction) sys_printmem_handler,
    /*  9 */ (TSyscallHandlerFunction) sys_setScreen_handler,
    /* 10 */ (TSyscallHandlerFunction) sys_date_handler,
    /* 11 */ (TSyscallHandlerFunction) sys_infoReg_handler,
    /* 12 */ (TSyscallHandlerFunction) sys_malloc_handler,
    /* 13 */ (TSyscallHandlerFunction) sys_free_handler
};

size_t sysCallDispatcher(size_t rdi, size_t rsi, size_t rdx, size_t r10, size_t r8, size_t rax) {
    TSyscallHandlerFunction handler;
    if (rax < (sizeof(syscallHandlers)/sizeof(syscallHandlers[0])) && (handler = syscallHandlers[rax]) != NULL)
        return handler(rdi, rsi, rdx, r10, r8);
    return -1;
}