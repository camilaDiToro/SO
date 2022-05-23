#ifndef _COMMNADS_H_
#define _COMMNADS_H_

/* Local headers */
#include <kernelTypes.h>

#define COUNT_COMMANDS 17

#define IS_VOCAL(c) ((c) == 'a' || (c) == 'A' || (c) == 'e' || (c) == 'E' || (c) == 'i' || \
                     (c) == 'I' || (c) == 'o' || (c) == 'O' || (c) == 'u' || (c) == 'U')

typedef int (*TCommandFunction)(int stdin, int stdout, int stderr, int isForeground, int argc,
                                 const char* const argv[], TPid* createdProcess);

typedef struct {
    TCommandFunction commandFunction;
    const char* name;
    const char* description;
} TCommand;

int runHelp(int stdin, int stdout, int stderr, int isForeground, int argc, const char* const argv[], TPid* createdProcess);
int runClear(int stdin, int stdout, int stderr, int isForeground, int argc, const char* const argv[], TPid* createdProcess);
int runTime(int stdin, int stdout, int stderr, int isForeground, int argc, const char* const argv[], TPid* createdProcess);
int runDivideByZero(int stdin, int stdout, int stderr, int isForeground, int argc, const char* const argv[], TPid* createdProcess);
int runInvalidOp(int stdin, int stdout, int stderr, int isForeground, int argc, const char* const argv[], TPid* createdProcess);
// Physical memory management
int runMem(int stdin, int stdout, int stderr, int isForeground, int argc, const char* const argv[], TPid* createdProcess);
// Procesos, context switching y scheduling
int runPs(int stdin, int stdout, int stderr, int isForeground, int argc, const char* const argv[], TPid* createdProcess);
int runLoop(int stdin, int stdout, int stderr, int isForeground, int argc, const char* const argv[], TPid* createdProcess);
int runKill(int stdin, int stdout, int stderr, int isForeground, int argc, const char* const argv[], TPid* createdProcess);
int runNice(int stdin, int stdout, int stderr, int isForeground, int argc, const char* const argv[], TPid* createdProcess);
int runBlock(int stdin, int stdout, int stderr, int isForeground, int argc, const char* const argv[], TPid* createdProcess);
// Sincronización
int runSem(int stdin, int stdout, int stderr, int isForeground, int argc, const char* const argv[], TPid* createdProcess);
// Inter process communication
int runCat(int stdin, int stdout, int stderr, int isForeground, int argc, const char* const argv[], TPid* createdProcess);
int runWc(int stdin, int stdout, int stderr, int isForeground, int argc, const char* const argv[], TPid* createdProcess);
int runFilter(int stdin, int stdout, int stderr, int isForeground, int argc, const char* const argv[], TPid* createdProcess);
int runPipe(int stdin, int stdout, int stderr, int isForeground, int argc, const char* const argv[], TPid* createdProcess);
int runPhylo(int stdin, int stdout, int stderr, int isForeground, int argc, const char* const argv[], TPid* createdProcess);

static TCommand valid_commands[COUNT_COMMANDS] = {
    {(TCommandFunction)runHelp, "help", "Displays a list of all available commands.\n"},
    {(TCommandFunction)runClear, "clear", "Clear windows' shell.\n"},
    {(TCommandFunction)runTime, "time", "Displays the system's day and time.\n"},
    {(TCommandFunction)runDivideByZero, "dividezero", "Throws an exception caused by dividing by 0.\n"},
    {(TCommandFunction)runInvalidOp, "invalidop", "Throws an exception caused by an invalid operation.\n"},
    {(TCommandFunction)runMem, "mem", "Displays information about the memory status. \n"},
    {(TCommandFunction)runPs, "ps", "Displays a list of all the processes with their properties: name, ID, priority, stack, base pointer and foreground.\n"},
    {(TCommandFunction)runLoop, "loop", "Prints PID once every count of seconds. \n"},
    {(TCommandFunction)runKill, "kill", "Kill the process with the given ID. \n"},
    {(TCommandFunction)runNice, "nice", "Changes the priority of the process with the given ID. \n"},
    {(TCommandFunction)runBlock, "block", "Blocks the process with the given ID. \n"},
    {(TCommandFunction)runSem, "sem", "Displays a list of all the semaphores with their properties: state and the blocked processes in each.\n"},
    {(TCommandFunction)runCat, "cat", "Prints the standard input on the standard output. \n"},
    {(TCommandFunction)runWc, "wc", "Pritns the newlines' count the standard input on the standard output. \n"},
    {(TCommandFunction)runFilter, "filter", "Filters the vowels of the standard input. \n"},
    {(TCommandFunction)runPipe, "pipe", "Displays a list of all the pipes with their properties: state and the blocked processes in each.\n"},
    {(TCommandFunction)runPhylo, "phylo", "Resolve the Dining philosophers problem.\n"},
};

#endif