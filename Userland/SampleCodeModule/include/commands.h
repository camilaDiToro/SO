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

#endif