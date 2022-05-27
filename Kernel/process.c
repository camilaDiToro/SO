/* Standard library */
#include <stdint.h>
#include <stddef.h>
#include <sys/types.h>

/* Local headers */
#include <process.h>
#include <memoryManager.h>
#include <scheduler.h>
#include <lib.h>
#include <string.h>
#include <waitQueueADT.h>

#define FD_TABLE_CHUNK_SIZE 8
#define FD_TABLE_MAX_ENTRIES 64

#define MAX_NAME_LENGTH 16

/**
 * @brief Defines an entry on a process' file descriptor table.
 * Setting "resource" to null determines the whole structure as empty or unused.
 */
typedef struct {
    void* resource;
    TFdReadHandler readHandler;
    TFdWriteHandler writeHandler;
    TFdCloseHandler closeHandler;
    TFdDupHandler dupHandler;
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

    char* name;

    TFileDescriptorTableEntry* fdTable;
    unsigned int fdTableSize;

    char** argv;
    int argc;

    TWaitQueue waitpidQueue;
} TProcessContext;

static TProcessContext processes[MAX_PROCESSES];

static int handleUnmapFdUnchecked(TProcessContext* process, TPid pid, int fd);

static int tryGetProcessFromPid(TPid pid, TProcessContext** outProcess) {
    if (pid < 0 || pid >= MAX_PROCESSES || processes[pid].stackEnd == NULL)
        return 0;

    *outProcess = &processes[pid];
    return 1;
}

static int isNameValid(const char* name) {
    if (name == NULL)
        return 1;

    for (int i = 0; i <= MAX_NAME_LENGTH; i++) {
        char c = name[i];
        if (c == '\0')
            return 1;

        // The first character must be a letter. Subsequent characters may be a letter or a number.
        if ((c < 'a' || c > 'z') && (c < 'A' || c > 'Z') && c != '_') {
            if (i == 0 || c < '0' || c > '9')
                return 0;
        }
    }

    return 0;
}

TPid prc_create(const TProcessCreateInfo* createInfo) {
    TPid pid = 0;
    for (; pid < MAX_PROCESSES && processes[pid].stackEnd != NULL; pid++);

    if (createInfo->argc < 0 || pid == MAX_PROCESSES || !isNameValid(createInfo->name))
        return -1;

    void* stackEnd = NULL;
    char* nameCopy = NULL;
    char** argvCopy = NULL;
    if ((stackEnd = mm_malloc(PROCESS_STACK_SIZE)) == NULL
        || (createInfo->name != NULL && (nameCopy = mm_malloc(strlen(createInfo->name) + 1)) == NULL)
        || (createInfo->argc != 0 && (argvCopy = mm_malloc(sizeof(char*) * createInfo->argc)) == NULL)) {
        mm_free(stackEnd);
        mm_free(nameCopy);
        return -1;
    }

    for (int i = 0; i < createInfo->argc; ++i) {
        size_t length = strlen(createInfo->argv[i]) + 1;

        if ((argvCopy[i] = mm_malloc(length)) == NULL) {
            mm_free(stackEnd);
            mm_free(nameCopy);
            while (i > 0) {
                i--;
                mm_free(argvCopy[i]);
            }
            mm_free(argvCopy);
            return -1;
        }

        memcpy(argvCopy[i], createInfo->argv[i], length);
    }

    if (createInfo->name != NULL)
        strcpy(nameCopy, createInfo->name);

    TProcessContext* process = &processes[pid];
    process->stackEnd = stackEnd;
    process->stackStart = stackEnd + PROCESS_STACK_SIZE;
    process->isForeground = createInfo->isForeground;
    process->name = nameCopy;
    process->fdTable = NULL;
    process->fdTableSize = 0;
    process->argv = argvCopy;
    process->argc = createInfo->argc;
    process->waitpidQueue = NULL;

    sch_onProcessCreated(pid, createInfo->entryPoint, createInfo->priority, process->stackStart, createInfo->argc, (const char* const*)argvCopy);

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

    if (process->waitpidQueue != NULL) {
        wq_unblockAll(process->waitpidQueue);
        wq_free(process->waitpidQueue);
        process->waitpidQueue = NULL;
    }

    for (int i = 0; i < process->argc; i++) {
        mm_free(process->argv[i]);
    }
    mm_free(process->argv);
    mm_free(process->name);
    mm_free(process->stackEnd);
    mm_free(process->fdTable);
    process->stackEnd = NULL;
    process->stackStart = NULL;
    process->fdTable = NULL;
    process->fdTableSize = 0;

    return 0;
}

int prc_mapFd(TPid pid, int fd, void* resource, TFdReadHandler readHandler, TFdWriteHandler writeHandler, TFdCloseHandler closeHandler, TFdDupHandler dupHandler) {
    TProcessContext* process;
    if (resource == NULL || !tryGetProcessFromPid(pid, &process))
        return -1;

    if (fd < 0) {
        // Look for the lowest available file descriptor.
        for (fd = 3; fd < process->fdTableSize && process->fdTable[fd].resource != NULL; fd++);
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

        memset(&newFdTable[process->fdTableSize], 0, sizeof(TFileDescriptorTableEntry) * (newFdTableSize - process->fdTableSize));
        process->fdTable = newFdTable;
        process->fdTableSize = newFdTableSize;
    }

    process->fdTable[fd].resource = resource;
    process->fdTable[fd].readHandler = readHandler;
    process->fdTable[fd].writeHandler = writeHandler;
    process->fdTable[fd].closeHandler = closeHandler;
    process->fdTable[fd].dupHandler = dupHandler;

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

    process->isForeground = (isForeground != 0);
    return 0;
}

int prc_dupFd(TPid pidFrom, TPid pidTo, int fdFrom, int fdTo) {
    TProcessContext* processFrom;
    if (fdFrom < 0 || !tryGetProcessFromPid(pidFrom, &processFrom) || processFrom->fdTableSize <= fdFrom || processFrom->fdTable[fdFrom].resource == NULL || processFrom->fdTable[fdFrom].dupHandler == NULL)
        return 1;

    return processFrom->fdTable[fdFrom].dupHandler(pidFrom, pidTo, fdFrom, fdTo, processFrom->fdTable[fdFrom].resource);
}

ssize_t prc_handleReadFd(TPid pid, int fd, char* buf, size_t count) {
    TProcessContext* process;
    TFileDescriptorTableEntry* entry;
    if (fd < 0 || !tryGetProcessFromPid(pid, &process) || process->fdTableSize <= fd || (entry = &process->fdTable[fd])->resource == NULL || entry->readHandler == NULL)
        return -1;

    return entry->readHandler(pid, fd, entry->resource, buf, count);
}

ssize_t prc_handleWriteFd(TPid pid, int fd, const char* buf, size_t count) {
    TProcessContext* process;
    TFileDescriptorTableEntry* entry;
    if (fd < 0 || !tryGetProcessFromPid(pid, &process) || process->fdTableSize <= fd || (entry = &process->fdTable[fd])->resource == NULL || entry->writeHandler == NULL)
        return -1;

    return entry->writeHandler(pid, fd, entry->resource, buf, count);
}

int prc_listProcesses(TProcessInfo* array, int maxProcesses) {
    int processCounter = 0;
    for (int i = 0; i < MAX_PROCESSES && processCounter < maxProcesses; ++i) {
        TProcessContext* processContext = &processes[i];
        if (processContext->stackEnd != NULL) {
            TProcessInfo* info = &array[processCounter++];
            info->pid = i;
            strncpy(info->name, processContext->name, MAX_NAME_LENGTH);
            info->stackEnd = processContext->stackEnd;
            info->stackStart = processContext->stackStart;
            info->isForeground = processContext->isForeground;
            sch_getProcessInfo(i, info);
        }
    }

    return processCounter;
}

int prc_unblockOnKilled(TPid pidToUnblock, TPid pidToWait) {
    TProcessContext* process;
    if (!tryGetProcessFromPid(pidToWait, &process))
        return 1;

    if (process->waitpidQueue == NULL && (process->waitpidQueue = wq_new()) == NULL)
        return -1;

    wq_addIfNotExists(process->waitpidQueue, pidToUnblock);
    return 0;
}