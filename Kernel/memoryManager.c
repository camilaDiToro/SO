#include "memoryManager.h"

static void* next;
static size_t remainingSize;

void mm_init(void* memoryStart, size_t memorySize) {
    // word-allig memoryStart by rounding up to a multiple of 8.
    next = (void*)(((size_t)memoryStart + 7) & (~(size_t)0x07));
    remainingSize = memorySize - (next - memoryStart);
}

void* mm_malloc(size_t size) {
    if (remainingSize < size)
        return NULL;
    
    // word-allig size by rounding up to a multiple of 8.
    size = (void*)(((size_t)size + 7) & (~(size_t)0x07));

    void* p = next;
    next += size;
    remainingSize -= size;
    return p;
}

int mm_free(void* ptr) {
    return 1;
}