#ifndef _KERNEL_TYPES_H_
#define _KERNEL_TYPES_H_

/* Standard library */
#include <stdint.h>
#include <stddef.h>

/**
 * @brief Represents the different types of supported memory managers.
 */
typedef enum { NODE, BUDDY } TMemoryManagerType;

/**
 * @brief Represents the state of the system memory at a given point in time.
 */
typedef struct {
    size_t total;
    size_t used;
    TMemoryManagerType type;
    unsigned int chunks;
} TMemoryState;

/**
 * @brief Defines the maximum length for the name of a system resource.
 */
#define MAX_NAME_LENGTH 16

/**
 * @brief Defines the maximum amount of TPid-s that can be returned by an embedded array in a query.
 */
#define MAX_PID_ARRAY_LENGTH 8

/**
 * @brief Represents a process status.
 */
typedef enum { READY = 0, RUNNING, BLOCKED, KILLED } TProcessStatus;

/**
 * @brief Represents process' ID.
 */
typedef int TPid;

/**
 * @brief Represents process' priority.
 */
typedef int8_t TPriority;

/**
 * @brief Defines the default priority for a newly created process.
 */
#define DEFAULT_PRIORITY 0

/**
 * @brief Defines the lowest priority for a process.
 */
#define MIN_PRIORITY 10

/**
 * @brief Defines the highest priority for a process.
 */
#define MAX_PRIORITY -10

/**
 * @brief Defines the lowest priority at which a process is considered real-time.
 */
#define PRIORITY_REALTIME -5

/**
 * @brief Represents a process' entrypoint function.
 */
typedef void (*TProcessEntryPoint)(int argc, char* argv[]);

/**
 * @brief Represents a process' information at a point in time.
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
 * @brief Represents a pipe's state information at a point in time.
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

/**
 * @brief Represents a semaphore's state information at a point in time
 */
typedef struct {
    int value;
    int linkedProcesses; 
    char name[MAX_NAME_LENGTH+1];
    TPid waitingProcesses[MAX_PID_ARRAY_LENGTH+1];
} TSemaphoreInfo;

#endif