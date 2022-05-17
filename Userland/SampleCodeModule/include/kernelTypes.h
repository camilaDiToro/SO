#ifndef _KERNEL_TYPES_H_
#define _KERNEL_TYPES_H_

/* Standard library */
#include <stdint.h>

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
    const char* name;
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

typedef struct {
    const char* name;
} TPipeInfo;

/**
 * @brief Represents a semaphore.
 */
typedef int TSem;

typedef struct {
    const char* name;
} TSemaphoreInfo;

#endif