/* Local headers */
#include <commands.h>
#include <kernelTypes.h>
#include <shell.h>
#include <string.h>
#include <syscalls.h>
#include <test.h>
#include <userstdlib.h>

#define MAX_COMMAND 128
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
    int argc = parseCommandArgs(str, argv);
    TPid pid = -1;
    int mode;

    // Check if it's a valid command
    const TCommand* command = getCommandByName(argv[0]);
    if (command == NULL) {
        fprint(STDERR, "Invalid command \n");
        return -1;
    }

    if (!isForeground) {
        mode = BACKGROUND;
    } else {
        mode = FOREGROUND;
    }

    // If the command is not built-in, puts in 'pid' the pid of the created process
    if ((command->function(fd_in, fd_out, fd_err, mode, argc, argv, &pid)) < 0) {
        fprint(STDERR, "Error while executing command.\n");
        return -1;
    }

    return pid;
}

void readCommand(char* str) {

    // Check if the command is foreground or background
    size_t lenght = strlen(str);
    int isForeground = (str[lenght - 1] != FOREGROUND_CHARACTER);
    if (!isForeground) {
        str[lenght - 1] = '\0';
    }

    int pipe = strchr(str, PIPE_CHARACTER);

    // If there isn't a pipe
    if (!pipe) {
        executeCommand(STDIN, STDOUT, STDERR, str, isForeground);
        return;
    } else {

        char* commands[MAX_PIPES + 1] = {NULL};
        int cantCommands = parseCommandPipes(str, commands, PIPE_CHARACTER);
        int cantPipes = cantCommands - 1;

        // Check if are all valid commands
        for (int i = 0; i < cantPipes; i++) {
            if (!checkCommand(commands[i])) {
                fprintf(STDERR, "Invalid command %s \n", commands[i]);
                return;
            }
        }

        // Open the pipes
        int pipes[cantPipes * 2];
        for (int i = 0; i < cantPipes; i++) {
            if (sys_pipe(&pipes[i * 2]) < 0) {
                fprint(STDERR, "Error opening pipe \n");
                for (int j = 0; j < i * 2; j++) {
                    sys_close(pipes[j]);
                }
                return;
            }
        }

        TPid pidToWait[MAX_PIPES + 1];
        for (int i = 0; i < cantPipes; i++) {
            int fd_in = (i == 0 ? STDIN : pipes[(i - 1) * 2]);
            int fd_out = (i == (cantPipes - 1) ? STDOUT : pipes[i * 2 + 1]);
            pidToWait[i] = executeCommand(fd_in, fd_out, STDERR, commands[i], isForeground); // TO DO: check foreground value
        }

        // Close the pipes
        for (int i = 0; i < cantPipes * 2; i++) {
            sys_close(pipes[i]);
        }

        for (int i = 0; i < cantPipes; i++) {
            sys_waitpid(pidToWait[i]);
        }
    }
}