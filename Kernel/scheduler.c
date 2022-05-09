/* Local Headers */
#include <scheduler.h>
#include <time.h>
#include <graphics.h>
#include <memoryManager.h>
#include <process.h>

#define QUANTUM 4

static TProcessState processStates[MAX_PROCESSES];

static TPid currentRunningProcessPID = -1;

// Suggested - implement the structure you think that fits better.
// This array stores pointers to the ready processes using as idx their pid
// An array is a good idea JUST if there is a small qty of processes (which is the case we are having now)
// A list would be a better solution if the MAX_PROCESS_QTY was a bigger number
// static TPid readyProcesses[MAX_PROCESSES];

static int tryGetProcessState(TPid pid, TProcessState** outState) {
    if (pid < 0 || pid >= MAX_PROCESSES || processStates[pid].currentRSP == NULL)
        return 0;

    *outState = &processStates[pid];
    return 1;
}

void sch_init() {
    currentRunningProcessPID = -1;
}

int sch_onProcessCreated(TPid pid, TProcessEntryPoint entryPoint, void* currentRSP) {
    // Processes, by default, are created in the state READY.
    processStates[pid].priority = DEFAULT_PRIORITY;
    processStates[pid].status = READY;
    processStates[pid].currentRSP = (void*) entryPoint; //currentRSP; // CHANGED FOR DEBUGGING UNTIL THE SCHEDULER CAN PROPERLY LOAD A PROCESS

    // We believe that the scheduler should be in charge of pushing into
    // the stack the harcoded values that the process needs to start.

    return 0;
}

int sch_onProcessKilled(TPid pid) {
    TProcessState* processState;
    if (!tryGetProcessState(pid, &processState))
        return 1;

    processState->status = KILLED;
    processState->currentRSP = NULL;

    if (currentRunningProcessPID == pid)
        currentRunningProcessPID = -1;

    // TODO: Handle whatever the fuck you gotta handle
    // (keep in mind; what if the pid killed is currentRunningProcessPID?)

    return 0;
}

int sch_blockProcess(TPid pid) {
    TProcessState* processState;
    if (!tryGetProcessState(pid, &processState))
        return 1;

    if (processState->status == BLOCKED)
        return 0;

    processState->status = BLOCKED;

    // TODO: Whatever else is necessary (e.g. remove from round robin queue)

    return 0;
}

int sch_unblockProcess(TPid pid) {
    TProcessState* processState;
    if (!tryGetProcessState(pid, &processState))
        return 1;

    if (processState->status == READY)
        return 0;

    processState->status = READY;

    // TODO: Whatever else is necessary (e.g. add to round robin queue)

    return 0;
}

TPid sch_getCurrentPID() {
    return currentRunningProcessPID;
}

// THIS FUNCTION IS ONLY FOR DEBUGGING UNTIL THE SCHEDULER CAN PROPERLY LOAD A PROCESS
void sch_correrCucuruchitos(TPid pid) {
    TProcessEntryPoint entryPoint = (void*) processStates[pid].currentRSP;

    currentRunningProcessPID = pid;
    entryPoint(0, NULL);
}

int sch_setProcessPriority(TPid pid, TPriority priority) {
    TProcessState* processState;
    if (!tryGetProcessState(pid, &processState))
        return 1;

    if (processState->priority == priority)
        return 0;

    processState->priority = priority;

    // TODO: Whatever else is necessary (e.g. move between round robin queues)

    return 0;
}

int sch_getProcessState(TPid pid, TProcessState* outState) {
    TProcessState* processState;
    if (!tryGetProcessState(pid, &processState))
        return 1;

    *outState = *processState;
    return 0;
}

void* sch_switchProcess(void* currentRSP) {
    // readyProcesses[currentRunningProcessPID]->currentRSP = currentRSP;
    //  TO DO

    /* Main idea:
      Save running process currentStackPointer
      Check if quantum has finished or there is another process with higher priority to preempt
      Check process status
      If we have to switch a process, we return the currentStackPointer of the other process.
      If not, we return currentStackPointer.
    */

    return currentRSP;
}
