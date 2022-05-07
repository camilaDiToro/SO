#include <stdint.h>

#include <process.h>

void sch_init();

/**
 * @brief Called by process.c whenever a new process is created.
 */
void sch_onProcessCreated(TProcess process);

/**
 * @brief Called by process.c whenever a process is being killed.
 */
void sch_onProcessKilled(TProcess process);

/**
 * @brief Instructs the scheduler that a given process should not run until it is unblocked.
 */
void sch_blockProcess(TPid process);

/**
 * @brief Instructs the scheduler that a process may be marked as ready and run.
 */
void sch_unblockProcess(TPid process);

/**
 * @brief Gets the PID of the currently-running or last was-running process,
 * or -1 if no process is currently running.
 */
int sch_getCurrentPID();

void* sch_switchProcess(void* currentRSP);

/**
 * @brief ((nice))
 */
int sch_nice(TPid pid, TPriority priority);