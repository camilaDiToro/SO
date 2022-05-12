/* Standard library */
#include <stdint.h>
#include <stddef.h>
#include <sys/types.h>

/* Local headers */
#include <process.h>
#include <memoryManager.h>
#include <pipe.h>
#include <scheduler.h>
#include <graphics.h>
#include <lib.h>
#include <string.h>

#define FD_TABLE_CHUNK_SIZE 8
#define FD_TABLE_MAX_ENTRIES 64

/**
 * @brief Defines an entry on a process' file descriptor table.
 * Setting "resource" to null determines the whole structure as empty or unused.
 */
typedef struct {
    void* resource;
    TFdReadHandler readHandler;
    TFdWriteHandler writeHandler;
    TFdCloseHandler closeHandler;
} TFileDescriptorTableEntry;

/**
 * @brief Defines a process' context information.
 */
typedef struct {
    /** Lowest memory addres, where stack ends and malloc/free are done. */
    void* stackEnd;

    /** highest memory addres, where stack begins. */
    void* stackStart;

    int isForeground;

    TFileDescriptorTableEntry* fdTable;
    unsigned int fdTableSize;

    char** argv;
    int argc;
} TProcessContext;

static TProcessContext processes[MAX_PROCESSES];

static int handleUnmapFdUnchecked(TProcessContext* process, TPid pid, int fd);

static int tryGetProcessFromPid(TPid pid, TProcessContext** outProcess) {
    if (pid < 0 || pid >= MAX_PROCESSES || processes[pid].stackEnd == NULL)
        return 0;

    *outProcess = &processes[pid];
    return 1;
}

TPid prc_create(TProcessEntryPoint entryPoint, int argc, const char* argv[]) {
    TPid pid = 0;
    for (; pid < MAX_PROCESSES && processes[pid].stackEnd != NULL; pid++);

    if (pid == MAX_PROCESSES)
        return -1;

    TProcessContext* process = &processes[pid];

    if ((process->stackEnd = mm_malloc(PROCESS_STACK_SIZE)) == NULL)
        return -1;

    process->stackStart = process->stackEnd + PROCESS_STACK_SIZE;
    process->isForeground = 1;
    process->fdTable = NULL;
    process->fdTableSize = 0;

    void* currentRSP = process->stackStart;

    char** argvCopy = mm_malloc(sizeof(char *) * argc);
    for(int i = 0; i < argc; ++i)
    {
        size_t length = strlen(argv[i])+1;
        argvCopy[i] = mm_malloc(length);
        memcpy(argvCopy[i], argv[i], length);
    }
    process->argc = argc;
    process->argv = argvCopy;
    sch_onProcessCreated(pid, entryPoint, currentRSP, argc, argvCopy);

    return pid;
}

int prc_kill(TPid pid) {
    TProcessContext* process;
    if (!tryGetProcessFromPid(pid, &process))
        return 1;

    // Close all remaining file descriptors for this process.
    for (int fd = 0; fd < process->fdTableSize; fd++)
        if (process->fdTable[fd].resource != NULL)
            handleUnmapFdUnchecked(process, pid, fd);

    sch_onProcessKilled(pid);

    for (int i = 0; i < process->argc; i++){
        mm_free(process->argv[i]);
    }
    mm_free(process->argv);
    mm_free(process->stackEnd);
    mm_free(process->fdTable);
    process->stackEnd = NULL;
    process->stackStart = NULL;
    process->fdTable = NULL;
    process->fdTableSize = 0;

    return 0;
}

int prc_mapFd(TPid pid, int fd, void* resource, TFdReadHandler readHandler, TFdWriteHandler writeHandler, TFdCloseHandler closeHandler) {
    TProcessContext* process;
    if (resource == NULL || !tryGetProcessFromPid(pid, &process))
        return -1;

    if (fd < 0) {
        // Look for the lowest available file descriptor.
        for (fd = 0; fd < process->fdTableSize && process->fdTable[fd].resource != NULL; fd++);

    } else {
        // Check that the requested fd is available.
        if (fd < process->fdTableSize && process->fdTable[fd].resource != NULL)
            return -1;
    }

    // If the table doesn't have enough space for this fd, expand it.
    if (fd >= process->fdTableSize) {
        size_t newFdTableSize = (process->fdTableSize + FD_TABLE_CHUNK_SIZE) / FD_TABLE_CHUNK_SIZE * FD_TABLE_CHUNK_SIZE;
        if (newFdTableSize > FD_TABLE_MAX_ENTRIES)
            newFdTableSize = FD_TABLE_MAX_ENTRIES;

        if (fd >= newFdTableSize)
            return -1;

        TFileDescriptorTableEntry* newFdTable = mm_realloc(process->fdTable, sizeof(TFileDescriptorTableEntry) * newFdTableSize);
        if (newFdTable == NULL)
            return -1;

        process->fdTable = newFdTable;
        process->fdTableSize = newFdTableSize;
    }

    process->fdTable[fd].resource = resource;
    process->fdTable[fd].readHandler = readHandler;
    process->fdTable[fd].writeHandler = writeHandler;
    process->fdTable[fd].closeHandler = closeHandler;

    return fd;
}

int prc_unmapFd(TPid pid, int fd) {
    TProcessContext* process;
    if (fd < 0 || !tryGetProcessFromPid(pid, &process) || process->fdTableSize <= fd || process->fdTable[fd].resource == NULL)
        return 1;

    return handleUnmapFdUnchecked(process, pid, fd);
}

static int handleUnmapFdUnchecked(TProcessContext* process, TPid pid, int fd) {
    TFileDescriptorTableEntry* entry = &process->fdTable[fd];
    int r;
    if (entry->closeHandler != NULL && (r = entry->closeHandler(pid, fd, entry->resource)) != 0)
        return r;

    entry->resource = NULL;
    entry->readHandler = NULL;
    entry->writeHandler = NULL;
    entry->closeHandler = NULL;
    return 0;
}

int prc_isForeground(TPid pid) {
    TProcessContext* process;
    if (!tryGetProcessFromPid(pid, &process))
        return -1;

    return process->isForeground;
}

int prc_setIsForeground(TPid pid, int isForeground) {
    TProcessContext* process;
    if (!tryGetProcessFromPid(pid, &process))
        return -1;

    return process->isForeground = (isForeground != 0);
    return 0;
}

ssize_t prc_handleReadFd(TPid pid, int fd, char* buf, size_t count) {
    TProcessContext* process;
    TFileDescriptorTableEntry* entry;
    if (fd < 0 || !tryGetProcessFromPid(pid, &process) || process->fdTableSize <= fd
        || (entry = &process->fdTable[fd])->resource == NULL || entry->readHandler == NULL)
        return -1;

    return entry->readHandler(pid, fd, entry->resource, buf, count);
}

ssize_t prc_handleWriteFd(TPid pid, int fd, const char* buf, size_t count) {
    TProcessContext* process;
    TFileDescriptorTableEntry* entry;
    if (fd < 0 || !tryGetProcessFromPid(pid, &process) || process->fdTableSize <= fd
        || (entry = &process->fdTable[fd])->resource == NULL || entry->writeHandler == NULL)
        return -1;

    return entry->writeHandler(pid, fd, entry->resource, buf, count);
}