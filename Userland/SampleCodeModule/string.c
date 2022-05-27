/* Standard library */
#include <string.h>

size_t strlen(const char* s) {
    size_t l;
    for (l = 0; *s != 0; s++, l++)
        ;
    return l;
}

int strcmp(const char* s1, const char* s2) {
    int i;
    for (i = 0; s1[i] && s1[i] == s2[i]; i++)
        ;
    return s1[i] - s2[i];
}

char* strcpy(char* dest, const char* src) {
    char* w;
    for (w = dest; *src != '\0'; *(w++) = *(src++))
        ;
    *w = '\0';
    return dest;
}

char* strncpy(char* dest, const char* src, size_t size) {
    int i = 0;
    char* ret = dest;
    while (*src && i < size) {
        *(dest++) = *(src++);
        i++;
    }
    *dest = '\0';
    return ret;
}

int strchr(char* str, char token) {
    while (*str) {
        if (*str == token) {
            return 1;
        }
        str++;
    }
    return 0;
}
