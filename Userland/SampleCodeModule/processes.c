/* Local headers */
#include <processes.h>
#include <syscalls.h>
#include <userstdlib.h>

#define IS_VOWEL(c) ((c) == 'a' || (c) == 'A' || (c) == 'e' || (c) == 'E' || (c) == 'i' || (c) == 'I' || (c) == 'o' || (c) == 'O' || (c) == 'u' || (c) == 'U')

void loopProcess(int argc, char* argv[]) {
    int ms = 3000;

    if (argc > 0) {
        int tmp = atoi(argv[0]);
        if (tmp > 0)
            ms = tmp;
    }

    TPid pid = sys_getPid();
    while (1) {
        printf("Hi! My PID is: %d \n", pid);
        sleep(ms);
    }
}

void catProcess(int argc, char* argv[]) {
    int c;
    while ((c = getChar()) >= 0)
        putChar(c);
}

void wcProcess(int argc, char* argv[]) {
    int c;
    int cantLines = 0;
    while ((c = getChar()) >= 0) {
        if (c == '\n')
            cantLines++;
    }
    printf("%d", cantLines);
}

void filterProcess(int argc, char* argv[]) {
    int c;
    while ((c = getChar()) >= 0) {
        if (IS_VOWEL(c))
            putChar(c);
    }
}