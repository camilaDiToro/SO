#ifndef _KERNEL_TYPES_H_
#define _KERNEL_TYPES_H_

/**
 * @brief Represents a process status.
 * 
 */
typedef enum status { READY = 0, BLOCKED, KILLED } TProcessStatus;

/**
 * @brief Represents process PID
 * 
 */
typedef int TPid;

/**
 * @brief Represents process priority
 * 
 */
typedef int8_t TPriority;

/**
 * @brief Represents a process' entrypoint function.
 *
 * @returns The process' exit code.
 */
typedef int (*TProcessEntryPoint)(int argc, const char* argv[]);

#endif