/* Local headers */
#include <shell.h>
#include <userstdlib.h>
#include <syscalls.h>

void main(int argc, char* argv[]) {
    welcome_message();
    while (1) {
        fputChar(STDERR, '>');
        wait_command();
    }
}