/* Local headers */
#include <graphics.h>
#include <keyboard.h>
#include <exceptions.h>

typedef void (*TException)(void);

static void zero_division();
static void invalid_opcode();
static void excepHandler(char* msg);
extern void give_control_to_user();
extern void _hlt();

//								Exception 0						Exception 6
static TException exceptions[] = {&zero_division, 0, 0, 0, 0, 0, &invalid_opcode};
static char* message[] = {"Zero Division Exception", "Invalid Opcode Exception"};

void exceptionDispatcher(int exception) {
    TException ex = exceptions[exception];
    if (ex != 0) {
        ex();   
    }
}

static void excepHandler(char* msg) {
    scr_print(msg);
    scr_printLine();     
    scr_print("Presione enter para continuar");
    int c;
    do {
        _hlt(); // halts the central processing unit until the next external interrupt is fired.
    } while ((c = kbd_getChar()) != '\n');
    scr_clear();
    give_control_to_user();
}

static void zero_division() {
    excepHandler(message[0]);
}

static void invalid_opcode() {
    excepHandler(message[1]);
}