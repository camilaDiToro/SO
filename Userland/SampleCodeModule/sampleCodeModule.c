/* Local headers */
#include <shell.h>
#include <userstdlib.h>
#include <syscalls.h>

int main(int argc, char* argv[]) {
    welcome_message();
    while (1) {
        putChar(STDERR, '>');
        wait_command();
    }
}