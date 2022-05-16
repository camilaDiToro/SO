/* Standard library */
#include <stdint.h>
#include <stddef.h>
#include <string.h>

/* Local headers */
#include <lib.h>
#include <moduleLoader.h>
#include <graphics.h>
#include <interrupts.h>
#include <idtLoader.h>
#include <memoryManager.h>
#include <process.h>
#include <scheduler.h>
#include <keyboard.h>
#include <systemCalls.h>
#include <kernelTypes.h>
#include <kernel.h>

extern uint8_t text;
extern uint8_t rodata;
extern uint8_t data;
extern uint8_t bss;
extern uint8_t endOfKernelBinary;
extern uint8_t endOfKernel;

static const uint64_t PageSize = 0x1000;
static void* const sampleCodeModuleAddress = (void*)0x400000;
static void* const sampleDataModuleAddress = (void*)0x500000;
static void* const startHeapAddres = (void*)0xF00000;
static void* const endHeapAddres = (void*)0x2000000;

void clearBSS(void* bssAddress, uint64_t bssSize) {
    memset(bssAddress, 0, bssSize);
}

void* getStackBase() {
    return (void*)((uint64_t)&endOfKernel + PageSize * 8 // The size of the stack itself, 32KiB
                   - sizeof(uint64_t)                    // Begin at the top of the stack
    );
}

void* initializeKernelBinary() {
    void* moduleAddresses[] = {
        sampleCodeModuleAddress,
        sampleDataModuleAddress
    };

    loadModules(&endOfKernelBinary, moduleAddresses);

    clearBSS(&bss, &endOfKernel - &bss);

    return getStackBase();
}

void initializeShell() {
    TPid pid = prc_create("shell", (TProcessEntryPoint)sampleCodeModuleAddress, 0, NULL);

    kbd_mapToProcessFd(pid, STDIN);          // Map STDIN
    scr_mapToProcessFd(pid, STDOUT, &WHITE); // Map STDOUT
    scr_mapToProcessFd(pid, STDERR, &RED);   // Map STDERR
}

int main() {

    // Disable interrupts
    _cli();

    load_idt();
    scr_init();
    mm_init(startHeapAddres, (size_t)(endHeapAddres - startHeapAddres));
    kbd_init();
    sch_init();

    initializeShell();

    // Enable interrupts
    _sti();
    sch_yieldProcess();

    return 0;
}