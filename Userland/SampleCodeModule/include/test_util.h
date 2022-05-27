#ifndef _TEST_UTIL_H_
#define _TEST_UTIL_H_

/* Standard library */
#include <stdint.h>
#include <stddef.h>

uint32_t GetUint();
uint32_t GetUniform(uint32_t max);
uint8_t memcheck(void* start, uint8_t value, uint32_t size);
int64_t satoi(char* str);
void bussy_wait(uint64_t n);
void endless_loop(uint64_t argc, char* argv[]);
void endless_loop_print(uint64_t argc, char* argv[]);
void* setmem(void* destiny, int32_t c, size_t length);

#endif