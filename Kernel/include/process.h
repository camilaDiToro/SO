#ifndef _PROCESS_H_
#define _PROCESS_H_

/* Standard library */
#include <stdint.h>

/* Local headers */
#include <pipe.h>

#define PROCESS_STACK_SIZE 4096
#define MAX_PROCESSES 16
#define DEFAULT_PRIORITY 0

typedef uint8_t TPid;
typedef int8_t TPriority;
typedef enum status { READY = 0, BLOCKED, KILLED } TProcessStatus;

typedef void (*TProcessEntryPoint)();

/**
 * Defines an entry on a process' file descriptor table.
 */
typedef struct {
    int fd;
    TPipe pipe;
} TFileDescriptorTableEntry;

/**
 * Defines a process' context information.
 */
typedef struct {
    TPid pid;
    TPriority priority;
    TProcessStatus status;

    /** highest memory addres, where stack begins. */
    void* stackStart;

    /** Lowest memory addres, where stack ends and malloc/free are done. */
    void* stackEnd;
    void* currentRSP;
    void* functionAddress;
    TFileDescriptorTableEntry* fdTable;
} TProcessContext;

typedef TProcessContext* TProcess;

/**
 * @brief Creates a new process from a given entry point and arguments list.
 * 
 * @returns The newly created TProcess, or NULL if process creation failed.
 */
TProcess prc_create(TProcessEntryPoint entryPoint, int argc, const char* argv[]);

/**
 * @brief Kills a process and frees associated resources. Using a process after
 * it has been killed results in undefined behaviour.
 */
void prc_kill(TProcess process);

TProcess prc_getProcessFromPid(Tpid pid);

void prc_printProcesses();

#endif