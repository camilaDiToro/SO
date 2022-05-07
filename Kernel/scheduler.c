/* Local Headers */
#include <time.h>
#include <scheduler.h>
#include <graphics.h>
#include <memoryManager.h>
#include <process.h>

#define QUANTUM 4

static int currentRunningProcessPID = -1;

// Suggested - implement the structure you think that fits better.
// This array stores pointers to the ready processes using as idx their pid
// An array is a good idea JUST if there is a small qty of processes (which is the case we are having now)
// A list would be a better solution if the MAX_PROCESS_QTY was a bigger number
static TProcess readyProcesses[MAX_PROCESSES] = {NULL};

static int validPID(int pid);

void sch_init() {
    currentRunningProcessPID = -1;
}

void sch_onProcessCreated(TProcess process) {

    process->currentRSP = 0; // TODO

    // This has been commented since we believe that the 
    // scheduler shoud be in charge of putting into de stack de harcoded values 
    // that the process needs to start. 
    /*if(process-> currentRSP == NULL){
        mm_free(stackEnd);
        mm_free(process);
        return NULL;
    }*/

    //Processes, by default, are created in the state READY.
    readyProcesses[process->pid] = process; 
}

void sch_onProcessKilled(TProcess process) {
    readyProcesses[process->pid] = NULL;
    prc_kill(process);
}

void sch_blockProcess(TPid process){
    Tprocess prc = prc_getProcessFromPid(pid);
    readyProcesses[process] = NULL;
    prc->status = BLOCKED;
}

void sch_unblockProcess(TPid process){
    Tprocess prc = prc_getProcessFromPid(pid);
    readyProcesses[process] = process;
    prc->status = READY;
}

int sch_getCurrentPID() {
    return currentRunningProcessPID;
}

void* sch_switchProcess(void* currentRSP) {
    readyProcesses[currentRunningProcessPID]->currentRSP = currentRSP;
    // TO DO

    /* Main idea:
      Save running process currentStackPointer
      Check if quantum has finished or there is another process with higher priority to preempt
      Check process status
      If we have to switch a process, we return the currentStackPointer of the other process.
      If not, we return currentStackPointer.
    */
}

int sch_nice(TPid pid, Tpriority priority) {

    if (!validPID(pid)) {
        return -1;
    }

    // TO DO: check priority
    // What happens if pid = SampleCodeModuleProcess?
    Tprocess prc = prc_getProcessFromPid(pid);
    prc->priority = priority;
}

/* int sch_kill(uint8_t pid) {

    if (!validPID(pid)) {
        return -1;
    }

    // What happens if pid = SampleCodeModuleProcess?
    if (processes[pid]->status == READY) {
        processes[pid]->status = KILLED;
        return 0;
    } else {
        return -1;
    }
} */

/* int sch_block(uint8_t pid) {

    if (!validPID(pid)) {
        return -1;
    }

    // What happens if pid = SampleCodeModuleProcess?
    if (processes[pid]->status == READY) {
        processes[pid]->status = BLOCKED;
        return 0;
    } else {
        return -1;
    }
    // If calling some of these functions during an interruption, should we change a process' status to READY?
    // We're about to hand control right back to it anyways
} */

/* int sch_unblock(uint8_t pid) {

    if (!validPID(pid)) {
        return -1;
    }

    if (processes[pid]->status == BLOCKED) {
        processes[pid]->status = READY;
        return 0;
    } else {
        return -1;
    }
} */

/* void sch_freeProcess(uint8_t pid) {
    free(processes[pid]);
    processes[pid] = NULL;
    return;
} */

static int validPID(int pid) {
    return pid < MAX_PROCESSES;
}
