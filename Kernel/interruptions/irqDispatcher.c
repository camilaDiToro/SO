// This is a personal academic project. Dear PVS-Studio, please check it.
// PVS-Studio Static Code Analyzer for C, C++ and C#: http://www.viva64.com

/* Standard library */
#include <stdint.h>

/* Local headers */
#include <time.h>
#include <keyboard.h>

typedef void (*TVoidFunction)(void);

static const TVoidFunction interruptions[] = {&rtc_interruptHandler, &kbd_interruptHandler};

void irqDispatcher(uint64_t irq) {
    TVoidFunction intFunction;
    if (irq < (sizeof(interruptions) / sizeof(interruptions[0])) && (intFunction = interruptions[irq]) != NULL) {
        intFunction();
    }
}