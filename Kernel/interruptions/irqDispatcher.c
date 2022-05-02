#include <time.h>
#include <stdint.h>
#include <keyboard.h>

static void int_20();
static void int_21();

typedef void (*void_function)(void);

static void_function interruptions[255] = {&int_20, &int_21};

void int_20() {
    timer_handler();
}

void int_21() {
    keyboard_handler();
}

void irqDispatcher(uint64_t irq) {
    void_function interruption = interruptions[irq];
    if (interruption != 0) {
        interruption();
    }
}
