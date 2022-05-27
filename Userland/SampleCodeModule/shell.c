/* Local headers */
#include <commands.h>
#include <kernelTypes.h>
#include <shell.h>
#include <string.h>
#include <syscalls.h>
#include <test.h>
#include <userstdlib.h>

#define MAX_COMMAND_LENGTH 128
#define MAX_ARGS 8
#define MAX_COMMANDS 8

#define PIPE_CHARACTER '-'
#define BACKGROUND_CHARACTER '&'

static void interpretCommand(char* buffer);
static int parseCommandArgs(char* str, int i, int* argc, char* argv[]);

void runShell() {
    print("Welcome to the Shell!\n");
    print("Type 'help' to see a list of all available commands.\n");

    char command[MAX_COMMAND_LENGTH + 1];
    int length;

    fputChar(STDERR, '>');
    while ((length = fgetLine(STDIN, command, MAX_COMMAND_LENGTH)) >= 0) {
        putChar('\n');
        interpretCommand(command);
        fprint(STDERR, "\n>");
    }

    fprint(STDERR, "Error: shell failed to read from STDIN. Am I running in foreground?\n");
}

static int advanceWhileWhitespace(char* str, int i) {
    while (str[i] == ' ')
        i++;
    return i;
}

static int advanceUntilWhitespace(char* str, int i) {
    while (str[i] != ' ' && str[i] != '\0' && str[i] != BACKGROUND_CHARACTER && str[i] != PIPE_CHARACTER)
        i++;
    return i;
}

static void interpretCommand(char* str) {
    int commandCount = 0;
    const TCommand* commands[MAX_COMMANDS];
    int commandArgcs[MAX_COMMANDS];
    char* commandArgvs[MAX_COMMANDS][MAX_ARGS];

    int i = 0;
    while (1) {
        i = advanceWhileWhitespace(str, i);
        if (str[i] == '\0' || str[i] == BACKGROUND_CHARACTER)
            break;

        if (commandCount != 0) {
            if (str[i] != PIPE_CHARACTER) {
                fprintf(STDERR, "Expected pipe character '%c' at character '%c'.", PIPE_CHARACTER, str[i]);
                return;
            }

            i = advanceWhileWhitespace(str, i + 1);
            if (str[i] == '\0') {
                fprintf(STDERR, "Unexpected end of line: command expected after pipe character '%c'.", PIPE_CHARACTER);
                return;
            }
        }

        if (commandCount == MAX_COMMANDS) {
            fprintf(STDERR, "Error: Shell may only execute up to %d piped commands.", MAX_COMMANDS);
            return;
        }

        char* commandNameStart = &str[i];
        i = advanceUntilWhitespace(str, i);
        char prevChar = str[i];
        str[i] = '\0';
        if ((commands[commandCount] = getCommandByName(commandNameStart)) == NULL) {
            fprintf(STDERR, "Error: command not found: %s.", commandNameStart);
            return;
        }
        str[i] = prevChar;

        i = parseCommandArgs(str, i, &commandArgcs[commandCount], commandArgvs[commandCount]);
        commandCount++;
    }

    int isForeground = (str[i] != BACKGROUND_CHARACTER);

    if (!isForeground) {
        // If there is something other than whitespace after the '&', print an error.
        i++;
        while (str[i] != '\0') {
            if (str[i] != ' ') {
                fprintf(STDERR, "Unexpected character after &: %c", str[i]);
                return;
            }
            i++;
        }
    }

    // Open the pipes
    int pipeCount = commandCount - 1;
    int pipes[pipeCount * 2];
    for (int i = 0; i < pipeCount; i++) {
        if (sys_pipe(&pipes[i * 2])) {
            fprintf(STDERR, "Error while opening pipe %d. Aborting commands.", i);
            for (int j = 0; j < i * 2; j++) {
                sys_close(pipes[j]);
            }
            return;
        }
    }

    TPid pidToWait[commandCount];
    for (int i = 0; i < commandCount; i++) {
        int fdStdin = (i == 0 ? STDIN : pipes[(i - 1) * 2]);
        int fdStdout = (i == (commandCount - 1) ? STDOUT : pipes[i * 2 + 1]);
        pidToWait[i] = -1;
        commands[i]->function(fdStdin, fdStdout, STDERR, isForeground, commandArgcs[i], (const char* const*)commandArgvs[i], &pidToWait[i]);
    }

    // Close the pipes
    for (int i = 0; i < pipeCount * 2; i++)
        sys_close(pipes[i]);

    // If foreground, wait for all created processes to finish
    if (isForeground) {
        for (int i = 0; i < commandCount; i++)
            if (pidToWait[i] >= 0) {
                sys_waitpid(pidToWait[i]);
            }
    }
}

static int parseCommandArgs(char* str, int i, int* argc, char* argv[]) {
    *argc = 0;

    while (str[i] != '\0' && str[i] != BACKGROUND_CHARACTER && str[i] != PIPE_CHARACTER) {
        if (str[i] == ' ') {
            i = advanceWhileWhitespace(str, i);
            continue;
        }
        char* argStart = &str[i];
        i = advanceUntilWhitespace(str, i);
        argv[(*argc)++] = argStart;

        if (str[i] == '\0')
            break;

        str[i++] = '\0';
    }

    return i;
}