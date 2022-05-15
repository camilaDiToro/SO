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
    if (prc_isForeground(pid)) {
        scr_print(msg);
        scr_print(" (0x");
        scr_printDec(exception);
        scr_printChar(')');
        scr_printLine();
        scr_print("Presione enter para continuar");

        kbd_clearBuffer();
        int c;
        do {
            _hlt(); // halts the central processing unit until the next external interrupt is fired.
        } while ((c = kbd_getChar()) != '\n');
        _cli();
    }

    prc_kill(pid);

    if (pid == 0) { // TO DO: check if shell PID is ALWAYS 0.
        scr_clear();
        initializeShell();
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