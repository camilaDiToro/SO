// This is a personal academic project. Dear PVS-Studio, please check it.
// PVS-Studio Static Code Analyzer for C, C++ and C#: http://www.viva64.com

/* Local Headers */
#include <scheduler.h>
#include <process.h>
#include <time.h>
#include <memoryManager.h>
#include <interrupts.h>

#define QUANTUM 4

// These are "Pseudo PIDs". That is, PID numbers that do not represent actual processes.
// The difference between these is that PSEUDOPID_KERNEL represents the kernel's main
// function (which is probly running an infinite hlt loop), while PSEUDOPID_NONE represents
// a caller that should not be reassigned CPU time (e.g. a process that was just killed).
#define PSEUDOPID_KERNEL -1
#define PSEUDOPID_NONE -2

typedef struct {
    TPriority priority;
    TProcessStatus status;
    void* currentRSP;
} TProcessState;

static void* mainRSP;

static TProcessState processStates[MAX_PROCESSES];
static TPid currentRunningPID;
static TPid forceRunNextPID;
static uint8_t quantums;

extern void* _createProcessContext(int argc, const char* const argv[], void* rsp, TProcessEntryPoint entryPoint);

static int isValidPid(TPid pid) {
    return pid >= 0 && pid < MAX_PROCESSES;
}

static int isActive(TPid pid) {
    return isValidPid(pid) && processStates[pid].currentRSP != NULL;
}

static int isReady(TPid pid) {
    return isActive(pid) && processStates[pid].status == READY;
}

static int getQuantums(TPid pid) {
    return MIN_PRIORITY - processStates[pid].priority;
}

static TPid getNextReadyPid() {
    TPid first = currentRunningPID < 0 ? 0 : currentRunningPID;
    TPid next = first;

    do {
        next = (next + 1) % MAX_PROCESSES;
        if (isReady(next)) {
            return next;
        }
    } while (next != first);

    return PSEUDOPID_KERNEL;
}

static int tryGetProcessState(TPid pid, TProcessState** outState) {
    if (!isActive(pid))
        return 0;

    *outState = &processStates[pid];
    return 1;
}

void sch_init() {
    forceRunNextPID = PSEUDOPID_NONE;
    currentRunningPID = PSEUDOPID_KERNEL;
    quantums = 0;
}

int sch_onProcessCreated(TPid pid, TProcessEntryPoint entryPoint, TPriority priority, void* currentRSP, int argc, const char* const argv[]) {
    if (priority < MAX_PRIORITY || priority > MIN_PRIORITY)
        priority = DEFAULT_PRIORITY;

    // Processes, by default, are created in the state READY.
    processStates[pid].priority = priority;
    processStates[pid].status = READY;
    processStates[pid].currentRSP = _createProcessContext(argc, argv, currentRSP, entryPoint);
    return 0;
}

int sch_onProcessKilled(TPid pid) {
    TProcessState* processState;
    if (!tryGetProcessState(pid, &processState))
        return 1;

    if (processState->status == KILLED)
        return 0;

    processState->status = KILLED;
    processState->currentRSP = NULL;

    if (currentRunningPID == pid)
        currentRunningPID = PSEUDOPID_NONE;

    return 0;
}

int sch_blockProcess(TPid pid) {
    TProcessState* processState;
    if (!tryGetProcessState(pid, &processState))
        return 1;

    processState->status = BLOCKED;

    if (currentRunningPID == pid)
        quantums = 0;

    return 0;
}

int sch_unblockProcess(TPid pid) {
    TProcessState* processState;
    if (!tryGetProcessState(pid, &processState))
        return 1;

    if (processState->status == READY || processState->status == RUNNING)
        return 0;

    if (processState->priority <= PRIORITY_REALTIME)
        forceRunNextPID = pid;

    processState->status = READY;

    return 0;
}

TPid sch_getCurrentPID() {
    return currentRunningPID;
}

int sch_setProcessPriority(TPid pid, TPriority newPriority) {
    TProcessState* processState;
    if (!tryGetProcessState(pid, &processState))
        return 1;

    if (newPriority < MAX_PRIORITY || newPriority > MIN_PRIORITY)
        return 1;

    processState->priority = newPriority;

    return 0;
}

void sch_yieldProcess() {
    quantums = 0;
    _int81();
}

void* sch_switchProcess(void* currentRSP) {
    if (currentRunningPID >= 0) {
        processStates[currentRunningPID].currentRSP = currentRSP;
        if (processStates[currentRunningPID].status == RUNNING)
            processStates[currentRunningPID].status = READY;
    }
    else if (currentRunningPID == PSEUDOPID_KERNEL)
        mainRSP = currentRSP;

    if (isReady(forceRunNextPID)) {
        currentRunningPID = forceRunNextPID;
        forceRunNextPID = PSEUDOPID_NONE;
        quantums = getQuantums(currentRunningPID);
    } else if (!isReady(currentRunningPID) || quantums == 0) {
        currentRunningPID = getNextReadyPid();

        if (currentRunningPID == PSEUDOPID_KERNEL) {
            quantums = 0;
            return mainRSP;
        }

        quantums = getQuantums(currentRunningPID);
    } else {
        quantums -= 1;
    }

    processStates[currentRunningPID].status = RUNNING;
    return processStates[currentRunningPID].currentRSP;
}

int sch_getProcessInfo(TPid pid, TProcessInfo* info) {
    TProcessState* processState;
    if (!tryGetProcessState(pid, &processState))
        return 1;

    info->status = processState->status;
    info->priority = processState->priority;
    info->currentRSP = processState->currentRSP;
    return 0;
}