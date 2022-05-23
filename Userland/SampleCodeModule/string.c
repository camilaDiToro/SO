/* Standard library */
#include <string.h>

size_t strlen(const char* s) {
    size_t l;
    for (l = 0; *s != 0; s++, l++);
    return l;
}

int strcmp(const char* s1, const char* s2) {
    int i;
    for (i = 0; s1[i] && s1[i] == s2[i]; i++);
    return s1[i] - s2[i];
}

char* strcpy(char* dest, const char* src) {
    char* w;
    for (w = dest; *src != '\0'; *(w++) = *(src++));
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

int strdiv(char* str, char** other, char token) {
    while (*str) {
        if (*str == token) {
            *str = '\0';
            *other = str + 1;
            return 0;
        }
        str++;
    }
    return -1;
}

char* strchr(char* str, char token) {
    while (*str) {
        if (*str == token) {
            return str;
        }
        str++;
    }
    return NULL;
}

int strchrCounter(char* str, char token) {
    int i = 0;
    while (*str) {
        if (*str == token) {
            i++;
        }
        str++;
    }
    return i;
}

int parseCommand(char* str, char* argv[]) {

    int argLen = 0;
    int argc = 0;
    int i = 0;
    char* arg = str;

    //Ignore initial spaces 
    while (str[i] == ' ') {
        i++;
        arg++;
    }

    for (; str[i] && argc < MAX_ARGS-1; i++) {
        if (str[i] != ' ') {
            argLen++;
        } else {
            while (str[i] == ' ') {
                i++;
            }
            i--;
            argv[argc] = arg;
            argv[argc][argLen] = '\0';
            argLen = 0;
            arg = str + i + 1;
            argc++;
        }
    }

    if ((str[i-1] != '\0') && (str[i-1] != ' ')) {
        argv[argc] = arg;
        argv[argc][argLen] = '\0';
        argc++;
    }

    argv[argc] = NULL;
    return argc;
}
