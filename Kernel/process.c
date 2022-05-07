/* Standard library */
#include <stdint.h>
#include <sys/types.h>

/* Local headers */
#include <process.h>
#include <memoryManager.h>
#include <pipe.h>
#include <scheduler.h>
#include <graphics.h>

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
    /** Lowest memory addres, where stack ends and malloc/free are done. */
    void* stackEnd;

    /** highest memory addres, where stack begins. */
    void* stackStart;

    TFileDescriptorTableEntry* fdTable;
} TProcessContext;

static TProcessContext processes[MAX_PROCESSES];

static int tryGetProcessFromPid(TPid pid, TProcessContext** outProcess) {
    if (pid < 0 || pid >= MAX_PROCESSES || processes[pid].stackEnd == NULL)
        return 0;

    *outProcess = &processes[pid];
    return 1;
}

TPid prc_create(TProcessEntryPoint entryPoint, int argc, const char* argv[]) {
    // Crear un proceso, deberia crear su contexto y su stack, le avisa al sch que se QUIERE cargar un proceso
    // llamando a sch_onCreateProcess
    int pid = 0;
    for (; pid < MAX_PROCESSES && processes[pid].stackEnd != NULL; pid++);

    if (pid == MAX_PROCESSES)
        return -1;

    TProcessContext* process = &processes[pid];

    if ((process->stackEnd = mm_malloc(PROCESS_STACK_SIZE)) == NULL)
        return -1;

    process->stackStart = process->stackEnd + PROCESS_STACK_SIZE;
    process->fdTable == NULL;
    
    void* currentRSP = process->stackStart;

    // TODO: Copy arguments onto the stack

    sch_onProcessCreated(pid, entryPoint, currentRSP);

    return pid;
}

int prc_kill(TPid pid) {
    TProcessContext* process;
    if (!tryGetProcessFromPid(pid, &process))
        return 1;
    
    sch_onProcessKilled(pid);

    // TODO: Dispose of all file descriptor resources.

    mm_free(process->stackEnd);
    mm_free(process->fdTable);
    process->stackEnd = NULL;
    process->stackStart = NULL;
    process->fdTable = NULL;

    return 0;
}

int prc_mapFd(TPid pid, TPipe pipe) {
    TProcessContext* process;
    if (!tryGetProcessFromPid(pid, &process))
        return 1;

    // TODO

    return 1;
}

int prc_unmapFd(TPid pid, int fd) {
    TProcessContext* process;
    if (!tryGetProcessFromPid(pid, &process))
        return 1;

    // TODO

    return 1;
}