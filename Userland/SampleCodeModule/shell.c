/* Local headers */
#include <commands.h>
#include <kernelTypes.h>
#include <shell.h>
#include <string.h>
#include <syscalls.h>
#include <test.h>
#include <userstdlib.h>

#define MAX_COMMAND 128
#define MAX_ARGS 10
#define PIPE_CHARACTER '|'
#define FOREGROUND_CHARACTER '&'
#define FOREGROUND 0
#define BACKGROUND 1

static void readCommand(char* buffer);
static TPid executeCommand(int fd_in, int fd_out, int fd_err, char* str, int isForeground);

void welcome_message() {
    print("Welcome to Userland\n");
    print("Type 'help' to see all available commands.\n");
}

void wait_command() {

    char command[MAX_COMMAND + 1];

    int ans = fgetLine(STDIN, command, MAX_COMMAND);

    if (ans != -1) {
        readCommand(command);
    } else {
        fprint(STDERR, "Invalid command \n");
    }
}

TPid executeCommand(int fd_in, int fd_out, int fd_err, char* str, int isForeground) {

    char* argv[MAX_ARGS] = {NULL};
    int argc = parseCommand(str, argv);
    TPid pid = -1;
    int mode;

    // Check if it's a valid command
    const TCommand* command = getCommandByName(argv[0]);
    if (command == NULL) {
        fprint(STDERR, "Invalid command \n");
        return -1;
    }

    if (isForeground) {
        mode = BACKGROUND;
    } else {
        if (argv[argc - 1][0] == FOREGROUND_CHARACTER) {
            mode = BACKGROUND;
        } else {
            mode = FOREGROUND;
        }
    }

    // If the command is not built-in, puts in 'pid' the pid of the created process
    if ((command->function(fd_in, fd_out, fd_err, mode, argc, argv, &pid)) < 0) {
        fprint(STDERR, "Error while executing command.\n");
        return -1;
    }

    return pid;
}

void readCommand(char* str) {

    char* p = strchr(str, PIPE_CHARACTER);

    // If there isn't a pipe
    if (p == NULL) {
        executeCommand(STDIN, STDOUT, STDERR, str, FOREGROUND);
        return;
    } else {
        // TO DO/FINISH.....
        // Idea with only one pipe

        *p = '\0';
        char* command1 = str;
        char* command2 = p + 1;

        int pipe_fds[2];

        if ((sys_openPipe("pipe", pipe_fds)) < 0) {
            fprint(STDERR, "Error opening pipe \n");
            return;
        }

        TPid pid = executeCommand(pipe_fds[0], STDOUT, STDERR, command2, BACKGROUND);
        executeCommand(STDIN, pipe_fds[1], STDERR, command1, FOREGROUND);

        sys_waitpid(pid);
        sys_unlinkPipe("pipe");

        /*
        int cantPipes = strchrCounter(str, '|');
        int cantComandos = cantPipes + 1;
        // Pipear: help algo | cat menem | cat gatito | filter nada
        int pipes[cantPipes * 2];

        for (int i = 0; i < cantPipes; i++) {
            sys_pipe(&pipes[i * 2]);
        }

        for (int i = 0; i < cantPipes; i++) {
            int fd_in = (i == 0 ? STDIN : pipes[(i - 1) * 2]);
            int fd_out = (i == (cantPipes - 1) ? STDOUT : pipes[i * 2 + 1]);
            //runLoquesea(fd_in, fd_out, -1, ...);
        }
        */
    }
}