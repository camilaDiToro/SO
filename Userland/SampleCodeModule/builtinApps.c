/* Local headers */
#include <builtinApps.h>
#include <syscalls.h>
#include <userstdlib.h>

#define IS_VOCAL(c) ((c) == 'a' || (c) == 'A' || (c) == 'e' || (c) == 'E' || (c) == 'i' || \
                     (c) == 'I' || (c) == 'o' || (c) == 'O' || (c) == 'u' || (c) == 'U')

void loopProcess(uint64_t argc, char* argv[]) {
    TPid pid = sys_getPid();
    while (1) {
        printf("Hi! My PID is: %d \n", pid);
        sleep(3500);
    }
}

void catProcess(uint64_t argc, char* argv[]) {
    int c;
    while ((c = getChar()) >= 0) {
        putChar(c);
    }
}

void wcProcess(uint64_t argc, char* argv[]) {
    int c;
    int cantLines = 0;
    while ((c = getChar()) >= 0) {
        if (c == '\n') {
            cantLines++;
        }
    }
    printf("%d", cantLines);
}

void filterProcess(uint64_t argc, char* argv[]) {
    int c;
    while ((c = getChar()) >= 0) {
        if (!IS_VOCAL(c)) {
            putChar(c);
        }
    }
}