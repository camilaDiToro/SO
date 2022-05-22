/* Standard library */
#include <stdint.h>
#include <stddef.h>
#include <stdarg.h>
#include <string.h>

/* Local headers */
#include <userstdlib.h>
#include <syscalls.h>

#define IS_DIGIT(x) (((x) >= '0' && (x) <= '9'))

int getChar() {
    char c;
    sys_read(STDIN, &c, 1);
    return c;
}

int putChar(char c) {
    return sys_write(STDOUT, &c, 1);
}

int fgetChar(int fd) {
    char c;
    sys_read(fd, &c, 1);
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

void sleep(unsigned long millis) {
    unsigned long start = sys_millis();
    do {
         sys_yield();
    } while (sys_millis() - start < millis);
}

int getLine(char* buffer, int maxSize) {
    return fgetLine(STDIN, buffer, maxSize);
}

int fgetLine(int fd, char* buffer, int maxSize) {
    int count = 0;
    char c;

    while ((c = fgetChar(fd)) != '\n') {
        if (c != -1) {
            if (count < maxSize) {
                if (c == '\b') { // Backspace
                    if (count == 0) {
                        continue;
                    }
                    count--;
                } else {
                    buffer[count++] = c;
                }
                putChar(c);
            } else {
                if (c == '\b') {
                    count--;
                } else {
                    count++;
                }
                putChar(c);
            }
        }
    }

    putChar(c);

    if (count > maxSize) {
        buffer[maxSize] = '\0';
        return -1;
    }

    buffer[count] = '\0';
    return count;
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

// https://stackoverflow.com/questions/1735236/how-to-write-my-own-printf-in-c
void fprintf(int fd, const char* frmt, ...) {
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
            fputChar(fd, *aux);
            aux++;
        }
        aux++;

        // Module 2: Fetching and executing arguments
        // va_arg() fetches the next argument from the argument list, where the 2nd parameter is the data type expected
        // ONLY accept char*, unsigned int, int or double
        switch (*aux) {
        case 'c':
            i = va_arg(arg, int); // Fetch char argument
            fputChar(fd, i);
            break;

        case 'd':
            i = va_arg(arg, int); // Fetch Decimal/Integer argument
            if (i < 0) {
                i = -i;
                fputChar(fd, '-');
            }
            fprint(fd, convert(i, 10));
            break;

        case 'o':
            i = va_arg(arg, unsigned int); // Fetch Octal representation
            fprint(fd, convert(i, 8));
            break;

        case 's':
            s = va_arg(arg, char*); // Fetch string
            fprint(fd, s == NULL ? "(NULL)" : s);
            break;

        case 'u':
            u = va_arg(arg, unsigned int); // Fetch Unsigned decimal integer
            fprint(fd, convert(u, 10));
            break;

        case 'x':
            u = va_arg(arg, unsigned int); // Fetch Hexadecimal representation
            fprint(fd, convert(u, 16));
            break;

        case '%':
            fputChar(fd, '%');
            break;
        }
    }

    // Module 3: Closing argument list to necessary clean-up
    va_end(arg);
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
            fputChar(STDOUT, *aux);
            aux++;
        }
        aux++;

        // Module 2: Fetching and executing arguments
        // va_arg() fetches the next argument from the argument list, where the 2nd parameter is the data type expected
        // ONLY accept char*, unsigned int, int or double
        switch (*aux) {
        case 'c':
            i = va_arg(arg, int); // Fetch char argument
            fputChar(STDOUT, i);
            break;

        case 'd':
            i = va_arg(arg, int); // Fetch Decimal/Integer argument
            if (i < 0) {
                i = -i;
                fputChar(STDOUT, '-');
            }
            fprint(STDOUT, convert(i, 10));
            break;

        case 'o':
            i = va_arg(arg, unsigned int); // Fetch Octal representation
            fprint(STDOUT, convert(i, 8));
            break;

        case 's':
            s = va_arg(arg, char*); // Fetch string
            fprint(STDOUT, s == NULL ? "(NULL)" : s);
            break;

        case 'u':
            u = va_arg(arg, unsigned int); // Fetch Unsigned decimal integer
            fprint(STDOUT, convert(u, 10));
            break;

        case 'x':
            u = va_arg(arg, unsigned int); // Fetch Hexadecimal representation
            fprint(STDOUT, convert(u, 16));
            break;

        case '%':
            fputChar(STDOUT, '%');
            break;
        }
    }

    // Module 3: Closing argument list to necessary clean-up
    va_end(arg);
}