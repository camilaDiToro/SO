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
#include <process.h>
#include <scheduler.h>
#include <pipe.h>

typedef size_t (*TSyscallHandlerFunction)(size_t rdi, size_t rsi, size_t rdx, size_t r10, size_t r8);

ssize_t sys_write_handler(int fd, const char* buffer, size_t count) {
    return prc_handleWriteFd(sch_getCurrentPID(), fd, buffer, count);
}

ssize_t sys_read_handler(int fd, char* buffer, size_t count) {
    return prc_handleReadFd(sch_getCurrentPID(), fd, buffer, count);
}

int sys_date_handler(char* buffer) {
    rtc_getCurrentDateString(buffer);
    return 0;
}

int sys_time_handler(char* buffer) {
    rtc_getCurrentTimeString(buffer);
    return 0;
}

int sys_close_handler(int fd) {
    return prc_unmapFd(sch_getCurrentPID(), fd);
}

int sys_clearScreen_handler() {
    scr_clear();
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

void* sys_realloc_handler(void* ptr, size_t size) {
    return mm_realloc(ptr, size);
}

int sys_pipe_handler(int pipefd[2]) {
    TPid pid = sch_getCurrentPID();

    TPipe pipe;
    int readFd = -1, writeFd = -1;

    if ((pipe = pipe_create()) == NULL || (readFd = pipe_mapToProcessFd(pid, -1, pipe, 1, 0)) < 0 || (writeFd = pipe_mapToProcessFd(pid, -1, pipe, 0, 1)) < 0) {
        if (pipe != NULL)
            pipe_free(pipe);
        if (readFd >= 0)
            prc_unmapFd(pid, readFd);
        return 1;
    }

    pipefd[0] = readFd;
    pipefd[1] = writeFd;
    return 0;
}

int sys_killProcess_handler(TPid pid) {
    int result = prc_kill(pid);
    if (pid == sch_getCurrentPID())
        sch_yieldProcess();
    return result;
}

int sys_blockProcess_handler(TPid pid) {
    int result = sch_blockProcess(pid);
    if (pid == sch_getCurrentPID())
        sch_yieldProcess();
    return result;
}

int sys_unblockProcess_handler(TPid pid) {
    return sch_unblockProcess(pid);
}

TPid sys_createProcess_handler(TProcessEntryPoint entryPoint, int argc, const char* const argv[]) {
    TPid pid = prc_create(entryPoint, argc, argv);

    // TODO: Map them to somewhere else!!
    if (pid >= 0) {
        kbd_mapToProcessFd(pid, STDIN);          // Map STDIN
        scr_mapToProcessFd(pid, STDOUT, &GREEN); // Map STDOUT
        scr_mapToProcessFd(pid, STDERR, &BLUE);   // Map STDERR
    }

    return pid;
}

void sys_yield_handler() {
    sch_yieldProcess();
}

TPid sys_getCurrentPid_handler() {
    return sch_getCurrentPID();
}

int sys_nice_handler(TPid pid, TPriority priority) {
    return sch_setProcessPriority(pid, priority);
}

int sys_exit_handler() {
    prc_kill(sch_getCurrentPID());
    sch_yieldProcess();
    return 1;
}

static TSyscallHandlerFunction syscallHandlers[] = {
    /* 0x00 */ (TSyscallHandlerFunction)sys_read_handler,
    /* 0x01 */ (TSyscallHandlerFunction)sys_write_handler,
    /* 0x02 */ (TSyscallHandlerFunction)sys_time_handler,
    /* 0x03 */ (TSyscallHandlerFunction)sys_close_handler,
    /* 0x04 */ (TSyscallHandlerFunction)sys_clearScreen_handler,
    /* 0x05 */ (TSyscallHandlerFunction)sys_killProcess_handler,
    /* 0x06 */ (TSyscallHandlerFunction)sys_blockProcess_handler,
    /* 0x07 */ (TSyscallHandlerFunction)sys_unblockProcess_handler,
    /* 0x08 */ (TSyscallHandlerFunction)sys_printmem_handler,
    /* 0x09 */ (TSyscallHandlerFunction)sys_createProcess_handler,
    /* 0x0A */ (TSyscallHandlerFunction)sys_date_handler,
    /* 0x0B */ (TSyscallHandlerFunction)sys_infoReg_handler,
    /* 0x0C */ (TSyscallHandlerFunction)sys_malloc_handler,
    /* 0x0D */ (TSyscallHandlerFunction)sys_free_handler,
    /* 0x0E */ (TSyscallHandlerFunction)sys_realloc_handler,
    /* 0x0F */ (TSyscallHandlerFunction)sys_yield_handler,
    /* 0x10 */ (TSyscallHandlerFunction)sys_getCurrentPid_handler,
    /* 0x11 */ (TSyscallHandlerFunction)sys_nice_handler, // ((nice))
    /* 0x12 */ (TSyscallHandlerFunction)sys_exit_handler,
    /* 0x13 */ (TSyscallHandlerFunction)NULL,
    /* 0x14 */ (TSyscallHandlerFunction)NULL,
    /* 0x15 */ (TSyscallHandlerFunction)NULL,
    /* 0x16 */ (TSyscallHandlerFunction)sys_pipe_handler
};

size_t sysCallDispatcher(size_t rdi, size_t rsi, size_t rdx, size_t r10, size_t r8, size_t rax) {
    TSyscallHandlerFunction handler;
    if (rax < (sizeof(syscallHandlers) / sizeof(syscallHandlers[0])) && (handler = syscallHandlers[rax]) != NULL)
        return handler(rdi, rsi, rdx, r10, r8);
    return -1;
}