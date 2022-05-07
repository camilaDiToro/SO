/* Standard library */
#include <stdint.h>

/* Local headers */
#include <process.h>

typedef enum status { READY = 0, BLOCKED, KILLED } TProcessStatus;

typedef int8_t TPriority;

#define DEFAULT_PRIORITY 0

void sch_init();

/**
 * @brief Called by process.c whenever a new process is created.
 */
int sch_onProcessCreated(TPid pid, TProcessEntryPoint entryPoint, void* currentRSP);

/**
 * @brief Called by process.c whenever a process is being killed.
 */
int sch_onProcessKilled(TPid pid);

/**
 * @brief Instructs the scheduler that a given process should not run until it is unblocked.
 */
int sch_blockProcess(TPid pid);

/**
 * @brief Instructs the scheduler that a process may be marked as ready and run.
 */
int sch_unblockProcess(TPid pid);

/**
 * @brief Gets the PID of the currently-running or last was-running process,
 * or -1 if no process is currently running.
 */
TPid sch_getCurrentPID();

int sch_getProcessPriority(TPid pid, TPriority* outPriority);

int sch_setProcessPriority(TPid pid, TPriority priority);

void* sch_switchProcess(void* currentRSP);