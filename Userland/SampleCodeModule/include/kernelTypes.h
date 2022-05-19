#ifndef _KERNEL_TYPES_H_
#define _KERNEL_TYPES_H_

/* Standard library */
#include <stdint.h>
#include <stddef.h>

#define MAX_NAME_LENGTH 16
#define MAX_PID_ARRAY_LENGTH 8

/**
 * @brief Represents a process status.
 */
typedef enum status { READY = 0, BLOCKED, KILLED } TProcessStatus;

/**
 * @brief Represents process PID
 */
typedef int TPid;

/**
 * @brief Represents process priority
 */
typedef int8_t TPriority;

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
    TPid readBlockedPids[MAX_PID_ARRAY_LENGTH];
    TPid writeBlockedPids[MAX_PID_ARRAY_LENGTH];
    char name[MAX_NAME_LENGTH + 1];
} TPipeInfo;

/**
 * @brief Represents a semaphore.
 */
typedef int TSem;

typedef struct {
    const char* name;
} TSemaphoreInfo;

#endif