/* Standard library */
#include <string.h>

/* Local headers */
#include <commands.h>
#include <kernelTypes.h>
#include <shell.h>
#include <syscalls.h>
#include <test.h>
#include <userstdlib.h>

#define MAX_COMMAND 128
#define MAX_ARGS 10

/*
typedef void (*TCommandFunction)(int stdin, int stdout, int stderr, int isForeground, int argc,
                                 const char* const argv[], TPid* createdProcces);

typedef struct {
    TCommandFunction commandFunction;
    const char* name;
    const char* description;
} TCommand;

static TCommand valid_commands[COUNT_COMMANDS] = {
    {(TCommandFunction)runHelp, "help", "Displays a list of all available commands.\n"},
    {(TCommandFunction)runClear, "clear", "Clear windows' shell.\n"},
    {(TCommandFunction)runTime, "time", "Displays the system's day and time.\n"},
    {(TCommandFunction)runDivideByZero, "dividezero", "Throws an exception caused by dividing by 0.\n"},
    {(TCommandFunction)runInvalidOp, "invalidop", "Throws an exception caused by an invalid operation.\n"},
    {(TCommandFunction)runMem, "mem", "Displays information about the memory status. \n"},
    {(TCommandFunction)runPs, "ps",  "Displays a list of all the processes with their properties: name, ID, priority, stack, base pointer and foreground.\n"},
    {(TCommandFunction)runLoop, "loop",  "Prints PID once every count of seconds. \n"},
    {(TCommandFunction)runKill, "kill", "Kill the process with the given ID. \n"},
    {(TCommandFunction)runNice, "nice", "Changes the priority of the process with the given ID. \n"},
    {(TCommandFunction)runBlock, "block", "Blocks the process with the given ID. \n"},
    {(TCommandFunction)runSem, "sem", "Displays a list of all the semaphores with their properties: state and the blocked processes in each.\n"},
    {(TCommandFunction)runCat, "cat", "Prints the standard input on the standard output. \n"},
    {(TCommandFunction)runWc, "wc", "Pritns the newlines' count the standard input on the standard output. \n"},
    {(TCommandFunction)runFilter, "filter", "Filters the vowels of the standard input. \n"},
    {(TCommandFunction)runPipe, "pipe", "Displays a list of all the pipes with their properties: state and the blocked processes in each.\n"},
    {(TCommandFunction)runPhylo, "phylo","Resolve the Dining philosophers problem.\n"},
}; */

static int getCommandIndex(char* str);
static void readCommand(char* buffer);
static TPid executeCommand(int fd_in, int fd_out, int fd_err, char* str);

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

int getCommandIndex(char* string) {
    for (int i = 0; i < COUNT_COMMANDS; i++) {
        if (!strcmp(string, valid_commands[i].name)) {
            return i;
        }
    }
    return -1;
}

TPid executeCommand(int fd_in, int fd_out, int fd_err, char* str) {

    char* argv[MAX_ARGS] = {NULL};
    int argc = parseCommand(str, argv);

    // Check if it's a valid command
    int index = getCommandIndex(str);
    if (index == -1) {
        fprint(STDERR, "Invalid command \n");
        return -1;
    }

    // If the commands is not built-in, puts the value of the created procces in "pid"
    TPid pid = -1;
    // 1 foreground ?
    if ((valid_commands[index].commandFunction(fd_in, fd_out, fd_err, 1, argc, argv, &pid)) < 0) {
        fprint(STDERR, "Error while executing command\n");
        return -1;
    }

    return pid;
}

void readCommand(char* str) {

    char* p = strchr(str, '|');

    // If there isn't a pipe
    if (p == NULL) {
        executeCommand(STDIN, STDOUT, STDERR, str);
        return;
    } else {

        // Only one pipe
        int pipe_fds[2];

        if ((sys_openPipe("pipe", pipe_fds)) < 0) {
            fprint(STDERR, "Error opening pipe \n");
            return;
        }

        *p = '\0';
        char* command1 = str;
        char* command2 = p + 1;

        TPid pid = executeCommand(pipe_fds[0], STDOUT, STDERR, command2);
        executeCommand(STDIN, pipe_fds[1], STDERR, command1);

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