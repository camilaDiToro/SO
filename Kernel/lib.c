// This is a personal academic project. Dear PVS-Studio, please check it.
// PVS-Studio Static Code Analyzer for C, C++ and C#: http://www.viva64.com

/* Local headers */
#include <lib.h>

void* memset(void* destination, int32_t c, size_t length) {
    uint8_t chr = (uint8_t)c;
    char* dst = (char*)destination;

    while (length--)
        dst[length] = chr;

    return destination;
}

void* memcpy(void* destination, const void* source, size_t length) {

    uint64_t i;

    if ((uint64_t)destination % sizeof(uint32_t) == 0 &&
        (uint64_t)source % sizeof(uint32_t) == 0 &&
        length % sizeof(uint32_t) == 0) {
        uint32_t* d = (uint32_t*)destination;
        const uint32_t* s = (const uint32_t*)source;

        for (i = 0; i < length / sizeof(uint32_t); i++)
            d[i] = s[i];
    } else {
        uint8_t* d = (uint8_t*)destination;
        const uint8_t* s = (const uint8_t*)source;

        for (i = 0; i < length; i++)
            d[i] = s[i];
    }

    return destination;
}

uint32_t uintToBase(uint64_t value, char* buffer, uint32_t base) {
    char* p = buffer;
    char* p1;
    char* p2;
    uint32_t digits = 0;

    do {
        uint32_t remainder = value % base;
        *p++ = (remainder < 10) ? remainder + '0' : remainder + 'A' - 10;
        digits++;
    } while (value /= base);

    *p = 0;
    
    p1 = buffer;
    p2 = p - 1;
    while (p1 < p2) {
        char tmp = *p1;
        *p1 = *p2;
        *p2 = tmp;
        p1++;
        p2--;
    }
    return digits;
}

// Retrieved from https://stackoverflow.com/questions/28133020/how-to-convert-bcd-to-decimal
uint8_t bcdToDec(uint8_t time) {
    return (time >> 4) * 10 + (time & 0x0F);
}