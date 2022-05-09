/* Standard library */
#include <stdint.h>

/* Local headers */
#include <process.h>

typedef enum status { READY = 0, BLOCKED, KILLED } TProcessStatus;

typedef int8_t TPriority;

typedef struct {
    TPriority priority;
    TProcessStatus status;
    void* currentRSP;
} TProcessState;

#define DEFAULT_PRIORITY 0

void sch_init();

/**
 * @brief Called by process.c whenever a new process is created.
 *
 * @returns 0 if the operation succeeded.
 */
int sch_onProcessCreated(TPid pid, TProcessEntryPoint entryPoint, void* currentRSP);

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
 * @brief Gets the PID of the currently-running or last was-running process,
 * or -1 if no process is currently running.
 */
TPid sch_getCurrentPID();

// THIS FUNCTION IS ONLY FOR DEBUGGING UNTIL THE SCHEDULER CAN PROPERLY LOAD A PROCESS
void sch_correrCucuruchitos(TPid pid);

/**
 * @brief Sets a process' priority.
 *
 * @returns 0 if the operation succeeded.
 */
int sch_setProcessPriority(TPid pid, TPriority priority);

/**
 * @brief Gets a process' state information, containing it's priority, status, and stack pointer.
 *
 * @returns 0 if the operation succeeded.
 */
int sch_getProcessState(TPid pid, TProcessState* outState);

void* sch_switchProcess(void* currentRSP);