// This is a personal academic project. Dear PVS-Studio, please check it.
// PVS-Studio Static Code Analyzer for C, C++ and C#: http://www.viva64.com

/* Standard library */
#include <stdarg.h>
#include <stddef.h>
#include <stdint.h>
#include <string.h>

/* Local headers */
#include <syscalls.h>
#include <userstdlib.h>

#define IS_DIGIT(x) (((x) >= '0' && (x) <= '9'))

void sleep(unsigned long millis) {
    unsigned long start = sys_millis();
    do {
        sys_yield();
    } while (sys_millis() - start < millis);
}

int getChar() {
    char c;
    if (sys_read(STDIN, &c, 1) <= 0) {
        return -1;
    }
    return c;
}

int putChar(char c) {
    return sys_write(STDOUT, &c, 1);
}

int fgetChar(int fd) {
    char c;
    if (sys_read(fd, &c, 1) <= 0) {
        return -1;
    }
    return c;
}

int fputChar(int fd, char c) {
    return sys_write(fd, &c, 1);
}

int print(const char* str) {
    return sys_write(STDOUT, str, strlen(str));
}

int fprint(int fd, const char* str) {
    return sys_write(fd, str, strlen(str));
}

int atoi(const char* str) {
    int answer = 0;
    int neg = 1;

    if (*str == '-') {
        neg = -1;
        str++;
    }

    while (*str) {
        if (!IS_DIGIT(*str)) {
            return 0;
        }
        answer = 10 * answer + (*str - '0');
        str++;
    }
    return neg * answer;
}

int getLine(char* buffer, int maxSize) {
    return fgetLine(STDIN, buffer, maxSize);
}

int fgetLine(int fd, char* buffer, int maxSize) {
    int count = 0;
    char c;

    while ((c = fgetChar(fd)) >= 0 && c != '\n') {
        if (c == '\b') {
            if (count > 0) {
                count--;
                putChar(c);
            }
        } else if (count < maxSize) {
            buffer[count++] = c;
            putChar(c);
        }
    }

    buffer[count] = '\0';
    return count;
}

char* convert(unsigned int num, unsigned int base, char* buff) {
    const char* representation = "0123456789ABCDEF";
    char* ptr;

    ptr = &buff[sizeof(buff) - 1];
    *ptr = '\0';

    do {
        *--ptr = representation[num % base];
        num /= base;
    } while (num != 0);

    return ptr;
}

// Retrieved from: https://stackoverflow.com/questions/1735236/how-to-write-my-own-printf-in-c
void fprintf(int fd, const char* frmt, ...) {
    va_list arg;         
    va_start(arg, frmt); 

    const char* aux;

    int i;
    unsigned int u;
    char* s;
    char tmpBuff[33];

    for (aux = frmt; *aux != '\0'; aux++) {
        while (*aux != '%') {
            if (*aux == '\0') {
                va_end(arg);
                return;
            }
            fputChar(fd, *aux);
            aux++;
        }
        aux++;

        switch (*aux) {
            case 'c':
                i = va_arg(arg, int); 
                fputChar(fd, i);
                break;

            case 'd':
                i = va_arg(arg, int); 
                if (i < 0) {
                    i = -i;
                    fputChar(fd, '-');
                }
                fprint(fd, convert(i, 10, tmpBuff));
                break;

            case 'o':
                u = va_arg(arg, unsigned int);
                fprint(fd, convert(u, 8, tmpBuff));
                break;

            case 's':
                s = va_arg(arg, char*);
                fprint(fd, s == NULL ? "(NULL)" : s);
                break;

            case 'u':
                u = va_arg(arg, unsigned int);
                fprint(fd, convert(u, 10, tmpBuff));
                break;

            case 'x':
                u = va_arg(arg, unsigned int); 
                fprint(fd, convert(u, 16, tmpBuff));
                break;

            case '%':
                fputChar(fd, '%');
                break;
        }
    }

    va_end(arg);
}

// Retrieved from: https://stackoverflow.com/questions/1735236/how-to-write-my-own-printf-in-c
void printf(const char* frmt, ...) {
    va_list arg;        
    va_start(arg, frmt); 

    const char* aux;

    int i;
    unsigned int u;
    char* s;
    char tmpBuff[33];

    for (aux = frmt; *aux != '\0'; aux++) {
        while (*aux != '%') {
            if (*aux == '\0') {
                va_end(arg);
                return;
            }
            fputChar(STDOUT, *aux);
            aux++;
        }
        aux++;

        switch (*aux) {
            case 'c':
                i = va_arg(arg, int); 
                fputChar(STDOUT, i);
                break;

            case 'd':
                i = va_arg(arg, int); 
                if (i < 0) {
                    i = -i;
                    fputChar(STDOUT, '-');
                }
                fprint(STDOUT, convert(i, 10, tmpBuff));
                break;

            case 'o':
                u = va_arg(arg, unsigned int);
                fprint(STDOUT, convert(u, 8, tmpBuff));
                break;

            case 's':
                s = va_arg(arg, char*); 
                fprint(STDOUT, s == NULL ? "(NULL)" : s);
                break;

            case 'u':
                u = va_arg(arg, unsigned int); 
                fprint(STDOUT, convert(u, 10, tmpBuff));
                break;

            case 'x':
                u = va_arg(arg, unsigned int); 
                fprint(STDOUT, convert(u, 16, tmpBuff));
                break;

            case '%':
                fputChar(STDOUT, '%');
                break;
        }
    }
    
    va_end(arg);
}