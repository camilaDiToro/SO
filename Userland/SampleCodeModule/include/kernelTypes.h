#ifndef _KERNEL_TYPES_H_
#define _KERNEL_TYPES_H_

/* Standard library */
#include <stdint.h>
#include <stddef.h>

typedef enum { NODE, BUDDY } TMemoryManagerType;

typedef struct {
    size_t total;
    size_t used;
    TMemoryManagerType type;
    unsigned int chunks;
} TMemoryState;

#define MAX_NAME_LENGTH 16
#define MAX_PID_ARRAY_LENGTH 8

/**
 * @brief Represents a process status.
 */
typedef enum { READY = 0, RUNNING, BLOCKED, KILLED } TProcessStatus;

/**
 * @brief Represents process PID
 */
typedef int TPid;

/**
 * @brief Represents process priority
 */
typedef int8_t TPriority;

#define DEFAULT_PRIORITY 0
#define MIN_PRIORITY 10
#define MAX_PRIORITY -10
#define PRIORITY_REALTIME -5

/**
 * @brief Represents a process' entrypoint function.
 *
 * @returns The process' exit code.
 */
typedef void (*TProcessEntryPoint)(int argc, char* argv[]);

/**
 * @brief Represents a process' information.
 */
typedef struct {
    TPid pid;
    char name[MAX_NAME_LENGTH + 1];
    void* stackEnd;
    void* stackStart;
    int isForeground;
    TPriority priority;
    TProcessStatus status;
    void* currentRSP;
} TProcessInfo;

/**
 * @brief Represents the information needed for a create process request.
 */
typedef struct {
    const char* name;
    TProcessEntryPoint entryPoint;
    int isForeground;
    TPriority priority;
    int argc;
    const char* const* argv;
} TProcessCreateInfo;

/**
 * @brief Represents a pipe's state information.
 */
typedef struct {
    size_t remainingBytes;
    unsigned int readerFdCount;
    unsigned int writerFdCount;
    TPid readBlockedPids[MAX_PID_ARRAY_LENGTH + 1];
    TPid writeBlockedPids[MAX_PID_ARRAY_LENGTH + 1];
    char name[MAX_NAME_LENGTH + 1];
} TPipeInfo;

/**
 * @brief Represents a semaphore.
 */
typedef int8_t TSem;

typedef struct {
    int value;
    int linkedProcesses; 
    char name[MAX_NAME_LENGTH+1];
    TPid waitingProcesses[MAX_PID_ARRAY_LENGTH+1];
} TSemaphoreInfo;

#endif