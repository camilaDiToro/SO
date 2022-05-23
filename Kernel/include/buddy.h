#ifndef _BUDDY_H_
#define _BUDDY_H_

void bd_init(void* memoryStart, size_t memorySize);
void* bd_malloc(size_t size);
int bd_free(void* ptr);
void* bd_realloc(void* ptr, size_t size);
void bd_printDebug();

#endif