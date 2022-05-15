/* Standard library */
#include <stdint.h>
#include <stddef.h>
#include <stdarg.h>
#include <string.h>

/* Local headers */
#include <userstdlib.h>
#include <syscalls.h>

#define IS_DIGIT(x) (((x) >= '0' && (x) <= '9'))

int sprint(int fd, const char* str) {
    return sys_write(fd, str, strlen(str));
}

int putChar(int fd, char c) {
    return sys_write(fd, &c, 1);
}

int getChar() {
    char c;
    while (sys_read(STDIN, &c, 1) != 1);
    return c;
}

int readChar() {
    char c;
    sys_read(STDIN, &c, 1);
    return c;
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

// https://stackoverflow.com/questions/1735236/how-to-write-my-own-printf-in-c
void printf(const char* frmt, ...) {
    // Module 1: Initializing Myprintf's arguments using stdarg.h
    va_list arg;         // declares a variable which we use to manipulating the argument list contaning variable arugments
    va_start(arg, frmt); // initialize arg with function's last fixed argument, i.e. format

    const char* aux;

    uint64_t i;
    unsigned u;
    char* s;

    for (aux = frmt; *aux != '\0'; aux++) {
        while (*aux != '%') {
            if (*aux == '\0') {
                va_end(arg);
                return;
            }
            putChar(STDOUT, *aux);
            aux++;
        }
        aux++;

        // Module 2: Fetching and executing arguments
        // va_arg() fetches the next argument from the argument list, where the 2nd parameter is the data type expected
        // ONLY accept char*, unsigned int, int or double
        switch (*aux) {
        case 'c':
            i = va_arg(arg, int); // Fetch char argument
            putChar(STDOUT, i);
            break;

        case 'd':
            i = va_arg(arg, int); // Fetch Decimal/Integer argument
            if (i < 0) {
                i = -i;
                putChar(STDOUT, '-');
            }
            sprint(1, convert(i, 10));
            break;

        case 'o':
            i = va_arg(arg, unsigned int); // Fetch Octal representation
            sprint(1, convert(i, 8));
            break;

        case 's':
            s = va_arg(arg, char*); // Fetch string
            sprint(1, s == NULL ? "(NULL)" : s);
            break;

        case 'u':
            u = va_arg(arg, unsigned int); // Fetch Unsigned decimal integer
            sprint(1, convert(u, 10));
            break;

        case 'x':
            u = va_arg(arg, unsigned int); // Fetch Hexadecimal representation
            sprint(1, convert(u, 16));
            break;

        case '%':
            putChar(STDOUT, '%');
            break;
        }
    }

    // Module 3: Closing argument list to necessary clean-up
    va_end(arg);
}

char* convert(unsigned int num, unsigned int base) {
    static char Representation[] = "0123456789ABCDEF";
    static char buff[33];
    char* ptr;

    ptr = &buff[sizeof(buff) - 1];
    *ptr = '\0';

    do {
        *--ptr = Representation[num % base];
        num /= base;
    } while (num != 0);

    return ptr;
}
