#ifndef _SCHEDULER_H_
#define _SCHEDULER_H_

/* Local headers */
#include <kernelTypes.h>

/**
 * @brief Initialize the Scheduler.
 */
void sch_init();

/**
 * @brief Called by process.c whenever a new process is created.
 *
 * @returns 0 if the operation succeeded, != 0 if not.
 */
int sch_onProcessCreated(TPid pid, TProcessEntryPoint entryPoint, TPriority priority, void* currentRSP, int argc, const char* const argv[]);

/**
 * @brief Called by process.c whenever a process is being killed.
 * If the caller is said process, it is not immediatelly stopped. For this to occur,
 * sch_yieldProcess() must be called.
 *
 * @returns 0 if the operation succeeded, != 0 if not.
 */
int sch_onProcessKilled(TPid pid);

/**
 * @brief Instructs the scheduler that a given process should not run until it is unblocked.
 * If the caller is said process, it is not immediately blocked. For this to occur,
 * sch_yieldProcess() must be called afterwards.
 *
 * @returns 0 if the operation succeeded, != 0 if not.
 */
int sch_blockProcess(TPid pid);

/**
 * @brief Instructs the scheduler that a process may be marked as ready and run.
 *
 * @returns 0 if the operation succeeded, != 0 if not.
 */
int sch_unblockProcess(TPid pid);

/**
 * @brief Gets the PID of the currently-running or last was-running process
 *
 * @returns PID or -1 if no process is currently running.
 */
TPid sch_getCurrentPID();

/**
 * @brief Sets a process' priority.
 *
 * @returns 0 if the operation succeeded, != 0 if not.
 */
int sch_setProcessPriority(TPid pid, TPriority newPriority);

/**
 * @brief Decides which process to run next
 *
 * @param currentRSP RSP of the interrupted process
 * @return RSP of the next process to run
 */
void* sch_switchProcess(void* currentRSP);

/**
 * @brief Yields control of the CPU to the next process on the ready list.
 * If the caller is not a process or is one that exited, this function does not return.
 */
void sch_yieldProcess();

/**
 * @brief Fills the given struct with the scheduler-related data of the requested process.
 * 
 * @returns 0 if the operation succeeded, != 0 if not.
 */
int sch_getProcessInfo(TPid pid, TProcessInfo* info);

#endif