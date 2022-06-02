#ifndef _COMMANDS_H_
#define _COMMANDS_H_

/* Local headers */
#include <kernelTypes.h>

typedef int (*TCommandFunction)(int stdin, int stdout, int stderr, int isForeground, int argc, const char* const argv[], TPid* createdProcess);

typedef struct {
    TCommandFunction function;
    const char* name;
    const char* description;
} TCommand;

const TCommand* getCommandByName(const char* name);

/* General commands */
int runHelp(int stdin, int stdout, int stderr, int isForeground, int argc, const char* const argv[], TPid* createdProcess);
int runClear(int stdin, int stdout, int stderr, int isForeground, int argc, const char* const argv[], TPid* createdProcess);
int runEcho(int stdin, int stdout, int stderr, int isForeground, int argc, const char* const argv[], TPid* createdProcess);
int runTime(int stdin, int stdout, int stderr, int isForeground, int argc, const char* const argv[], TPid* createdProcess);
int runDivideByZero(int stdin, int stdout, int stderr, int isForeground, int argc, const char* const argv[], TPid* createdProcess);
int runInvalidOp(int stdin, int stdout, int stderr, int isForeground, int argc, const char* const argv[], TPid* createdProcess);

/* Physical memory management commands */
int runMem(int stdin, int stdout, int stderr, int isForeground, int argc, const char* const argv[], TPid* createdProcess);

/* Processe-related commands */
int runPs(int stdin, int stdout, int stderr, int isForeground, int argc, const char* const argv[], TPid* createdProcess);
int runLoop(int stdin, int stdout, int stderr, int isForeground, int argc, const char* const argv[], TPid* createdProcess);
int runKill(int stdin, int stdout, int stderr, int isForeground, int argc, const char* const argv[], TPid* createdProcess);
int runNice(int stdin, int stdout, int stderr, int isForeground, int argc, const char* const argv[], TPid* createdProcess);
int runBlock(int stdin, int stdout, int stderr, int isForeground, int argc, const char* const argv[], TPid* createdProcess);
int runUnblock(int stdin, int stdout, int stderr, int isForeground, int argc, const char* const argv[], TPid* createdProcess);

/* Process synchronization commands */
int runSem(int stdin, int stdout, int stderr, int isForeground, int argc, const char* const argv[], TPid* createdProcess);

/* Inter process communication commands */
int runCat(int stdin, int stdout, int stderr, int isForeground, int argc, const char* const argv[], TPid* createdProcess);
int runWc(int stdin, int stdout, int stderr, int isForeground, int argc, const char* const argv[], TPid* createdProcess);
int runFilter(int stdin, int stdout, int stderr, int isForeground, int argc, const char* const argv[], TPid* createdProcess);
int runPipe(int stdin, int stdout, int stderr, int isForeground, int argc, const char* const argv[], TPid* createdProcess);
int runPhylo(int stdin, int stdout, int stderr, int isForeground, int argc, const char* const argv[], TPid* createdProcess);

/* Tests */
int runNamedPipes(int stdin, int stdout, int stderr, int isForeground, int argc, const char* const argv[], TPid* createdProcess);
int runTestMM(int stdin, int stdout, int stderr, int isForeground, int argc, const char* const argv[], TPid* createdProcess);
int runTestAsync(int stdin, int stdout, int stderr, int isForeground, int argc, const char* const argv[], TPid* createdProcess);
int runTestSync(int stdin, int stdout, int stderr, int isForeground, int argc, const char* const argv[], TPid* createdProcess);
int runTestProcesses(int stdin, int stdout, int stderr, int isForeground, int argc, const char* const argv[], TPid* createdProcess);
int runTestPrio(int stdin, int stdout, int stderr, int isForeground, int argc, const char* const argv[], TPid* createdProcess);

#endif