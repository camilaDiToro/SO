#ifndef _SCHEDULER_H_
#define _SCHEDULER_H_

/* Standard library */
#include <stdint.h>

/* Local headers */
#include <kernelTypes.h>
#include <process.h>

typedef struct {
    TPriority priority;
    TProcessStatus status;
    void* currentRSP;
} TProcessState;

#define DEFAULT_PRIORITY 0
#define MIN_PRIORITY 10
#define MAX_PRIORITY -10

/**
 * @brief Initialize Scheduler
 *
 */
void sch_init();

/**
 * @brief Called by process.c whenever a new process is created.
 *
 * @returns 0 if the operation succeeded.
 */
int sch_onProcessCreated(TPid pid, TProcessEntryPoint entryPoint, void* currentRSP, int argc, const char* const argv[]);

/**
 * @brief Called by process.c whenever a process is being killed.
 *
 * @returns 0 if the operation succeeded.
 */
int sch_onProcessKilled(TPid pid);

/**
 * @brief Instructs the scheduler that a given process should not run until it is unblocked.
 *
 * @returns 0 if the operation succeeded.
 */
int sch_blockProcess(TPid pid);

/**
 * @brief Instructs the scheduler that a process may be marked as ready and run.
 *
 * @returns 0 if the operation succeeded.
 */
int sch_unblockProcess(TPid pid);

/**
 * @brief Gets the PID of the currently-running or last was-running process
 *
 * @returns PID or -1 if no process is currently running.
 */
TPid sch_getCurrentPID();

// THIS FUNCTION IS ONLY FOR DEBUGGING UNTIL THE SCHEDULER CAN PROPERLY LOAD A PROCESS
void sch_correrCucuruchitos(TPid pid);

/**
 * @brief Sets a process' priority.
 *
 * @returns 0 if the operation succeeded.
 */
int sch_setProcessPriority(TPid pid, TPriority newPriority);

/**
 * @brief Gets a process' state information, containing it's priority, status, and stack pointer.
 *
 * @returns 0 if the operation succeeded.
 */
int sch_getProcessState(TPid pid, TProcessState* outState);

/**
 * @brief Decides which process to run next
 *
 * @param currentRSP RSP of the interrupted process
 * @return RSP of the next process to run
 */
void* sch_switchProcess(void* currentRSP);

/**
 * @brief The process that is running voluntarily yields the CPU
 *
 */
void sch_yieldProcess();

/**
 * @brief Stores the scheduler info of the process with the indicated pid into an info struct.
 * 
 * @returns 0 if the operation succeeded.
 */
int sch_getProcessInfo(TPid pid, TProcessInfo* info);

#endif