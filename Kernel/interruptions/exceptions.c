/* Standard library */
#include <stdint.h>

/* Local headers */
#include <graphics.h>
#include <keyboard.h>
#include <scheduler.h>
#include <process.h>
#include <kernel.h>
#include <interrupts.h>

static const char* exceptionMessages[] = {
    /* 0x00 */ "Divide by Zero",
    /* 0x01 */ 0,
    /* 0x02 */ 0,
    /* 0x03 */ 0,
    /* 0x04 */ 0,
    /* 0x05 */ 0,
    /* 0x06 */ "Invalid Opcode",
    /* 0x07 */ 0,
    /* 0x08 */ 0,
    /* 0x09 */ 0,
    /* 0x0A */ 0,
    /* 0x0B */ 0,
    /* 0x0C */ 0,
    /* 0x0D */ "General Protection",
    /* 0x0E */ "Page Fault",
};

static const char* registerNames[18] = {
    "RAX", "RBX", "RCX", "RDX", "RSI", "RDI", "RBP", "RSP", "R8 ", "R9 ", "R10", "R11", "R12", "R13", "R14", "R15", "RIP", "RFLAGS"
};

void exceptionDispatcher(uint64_t exception, const uint64_t regdata[17]) {
    TPid pid = sch_getCurrentPID();
    scr_print("PID ");
    scr_printDec(pid);
    scr_print(" CRASHED! Unhandled exception: (0x");
    scr_printHex(exception);
    scr_print(") ");
    scr_print(exception < (sizeof(exceptionMessages) / sizeof(exceptionMessages[0])) ? exceptionMessages[exception] : "Unknown");
    scr_printLine();

    for (int i = 0; i < (sizeof(registerNames) / sizeof(registerNames[0])); i++) {
        scr_print(registerNames[i]);
        scr_print(": ");
        scr_printRegisterFormat(regdata[i]);
        scr_print(i % 4 == 3 ? "\n" : "    ");
    }

    if (pid == 0) { // TO DO: check if shell PID is ALWAYS 0.
        scr_print("\nPress ENTER to restart the shell.");

        // TODO: This can't be handled like this, hlt would stop the entire system.
        // Consider: finding a way to stop just this process, or just kill all processes
        // and restart the whole system when the shell crashes.
        kbd_clearBuffer();
        int c;
        do {
            _hlt(); // halts the central processing unit until the next external interrupt is fired.
            _cli();
        } while ((c = kbd_getChar()) != '\n');

        prc_kill(pid);
        scr_clear();
        initializeShell();
    } else {
        prc_kill(pid);
    }

    sch_yieldProcess();
}