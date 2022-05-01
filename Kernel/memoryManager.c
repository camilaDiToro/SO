#include "memoryManager.h"


static void* next;
static uint64_t remainingSize;

void mm_init(void* memoryStart, uint64_t memorySize) {
    next = memoryStart;
    remainingSize = memorySize;
}

void * mm_malloc(uint64_t size) {
    if (remainingSize < size)
        return NULL;
    
    // Word allignment (64 bits)
    next = (void*)(((uint64_t)next + 7) & (~(uint64_t)0x07));

    void* p = next;
    next += size;
    remainingSize -= size;
    return p;
}

int mm_free(void* ptr) {
    return 1;
}