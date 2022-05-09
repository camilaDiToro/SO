/* Standard library */
#include <stdint.h>
#include <stddef.h>
#include <string.h>

/* Local headers */
#include <lib.h>
#include <moduleLoader.h>
#include <graphics.h>
#include <idtLoader.h>
#include <memoryManager.h>
#include <process.h>
#include <scheduler.h>
#include <keyboard.h>

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
        sampleDataModuleAddress};

    loadModules(&endOfKernelBinary, moduleAddresses);

    clearBSS(&bss, &endOfKernel - &bss);

    return getStackBase();
}

int main() {
    load_idt();
    scr_init();
    mm_init(startHeapAddres, (size_t)(endHeapAddres - startHeapAddres));
    sch_init();

    // ((TProcessEntryPoint)sampleCodeModuleAddress)(0, NULL);
    TPid pid = prc_create((TProcessEntryPoint)sampleCodeModuleAddress, 0, NULL);

    kbd_mapToProcessFd(pid); // Map STDIN
    scr_mapToProcessFd(pid, &WHITE); // Map STDOUT
    scr_mapToProcessFd(pid, &RED); // Map STDERR

    sch_correrCucuruchitos(pid);


    return 0;
}