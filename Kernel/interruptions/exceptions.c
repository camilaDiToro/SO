/* Local headers */
#include <graphics.h>
#include <keyboard.h>
#include <exceptions.h>
#include <scheduler.h>
#include <process.h>
#include <kernel.h>
#include <interrupts.h>

typedef void (*TException)(void);

static void zero_division();
static void invalid_opcode();
static void general_protection();
static void page_fault();
static void excepHandler(int exception, const char* msg);

static TException exceptions[] = {
    /* 0x00 */ &zero_division, 0, 0, 0, 0, 0,
    /* 0x06 */ &invalid_opcode, 0, 0, 0, 0, 0, 0,
    /* 0x0D */ &general_protection,
    /* 0x0E */ &page_fault
};

void exceptionDispatcher(int exception) {
    TException ex = exceptions[exception];
    if (ex == 0) {
        excepHandler(exception, "Unknown exception");
    } else {
        ex();
    }
}

static void excepHandler(int exception, const char* msg) {
    TPid pid = sch_getCurrentPID();
    scr_print("PID ");
    scr_printDec(pid);
    scr_print(" CRASHED! ");
    scr_print(msg);
    scr_print(" (0x");
    scr_printHex(exception);
    scr_print(")\n");

    if (pid == 0) { // TO DO: check if shell PID is ALWAYS 0.
        scr_print("Press ENTER to restart the shell.");

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

static void zero_division() {
    excepHandler(0x00, "Zero Division Exception");
}

static void invalid_opcode() {
    excepHandler(0x06, "Invalid Opcode Exception");
}

static void general_protection() {
    excepHandler(0x0D, "General Protection Exception");
}

static void page_fault() {
    excepHandler(0x0E, "Page Fault Exception");
}