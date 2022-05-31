// This is a personal academic project. Dear PVS-Studio, please check it.
// PVS-Studio Static Code Analyzer for C, C++ and C#: http://www.viva64.com

/* Standard library */
#include <stdint.h>

/* Local headers */
#include <time.h>
#include <keyboard.h>

typedef void (*TVoidFunction)(void);

static TVoidFunction interruptions[255] = {&rtc_interruptHandler, &kbd_interruptHandler};

void irqDispatcher(uint64_t irq) {
    TVoidFunction interruption = interruptions[irq];
    if (interruption != 0) {
        interruption();
    }
}