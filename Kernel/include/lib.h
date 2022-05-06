#ifndef _LIB_H_
#define _LIB_H_

/* Standard library */
#include <stdint.h>
#include <stddef.h>

/**
 * @brief Word aligns the given integer value down to the nearest multiple of 8.
 */
#define WORD_ALIGN_DOWN(value) ((value) & (~(size_t)0x07))

/**
 * @brief Word aligns the given integer value up to the nearest multiple of 8.
 */
#define WORD_ALIGN_UP(value) (WORD_ALIGN_DOWN((size_t)(value) + 7))

size_t strlen(const char* s);

/**
 * @brief fills the first n bytes of the memory area pointed to by s with the constant byte c
 *
 * @param s memory area
 * @param c constant byte
 * @param n bytes to fill
 *
 * @return a pointer to the memory area s
 */
void* memset(void* s, int32_t c, size_t n);

/**
 * @brief copies n bytes from memory area source to memory area destination. The memory areas must not overlap
 *
 * @param src memory area source
 * @param dest memory area destination
 * @param n bytes to copy
 *
 * @return a pointer to dest
 */
void* memcpy(void* dest, const void* src, size_t n);

/**
 * @brief Returns he output from executing the cpuid instruction (a 12-character representation of the processor’s manufacturer)
 */
char* cpuVendor(char* result);

uint32_t uintToBase(uint64_t value, char* buffer, uint32_t base);

uint8_t bcdToDec(uint8_t time);

#endif
