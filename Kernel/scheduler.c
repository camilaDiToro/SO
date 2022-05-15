/* Local Headers */
#include <scheduler.h>
#include <time.h>
#include <graphics.h>
#include <memoryManager.h>
#include <interrupts.h>

#define QUANTUM 4

typedef struct {
    TPriority priority;
    TProcessStatus status;
    void* currentRSP;
} TProcessState;

static TProcessState processStates[MAX_PROCESSES];
static TPid currentRunningPID = -1;
static uint8_t quantums = 0;

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
    TPid next = currentRunningPID;

    do {
        next = (next + 1) % MAX_PROCESSES;
        if (isReady(next)) {
            return next;
        }
    } while (next != currentRunningPID);
    return -1;
}

static int tryGetProcessState(TPid pid, TProcessState** outState) {
    if (!isActive(pid))
        return 0;

    *outState = &processStates[pid];
    return 1;
}

void sch_init() {
    currentRunningPID = -1;
    quantums = 0;
}

int sch_onProcessCreated(TPid pid, TProcessEntryPoint entryPoint, void* currentRSP, int argc, const char* const argv[]) {
    // Processes, by default, are created in the state READY.
    processStates[pid].priority = DEFAULT_PRIORITY;
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
        currentRunningPID = -1;

    return 0;
}

int sch_blockProcess(TPid pid) {
    TProcessState* processState;
    if (!tryGetProcessState(pid, &processState))
        return 1;

    if (processState->status == BLOCKED)
        return 0;

    processState->status = BLOCKED;

    if (currentRunningPID == pid)
        quantums = 0;

    return 0;
}

int sch_unblockProcess(TPid pid) {
    TProcessState* processState;
    if (!tryGetProcessState(pid, &processState))
        return 1;

    if (processState->status == READY)
        return 0;

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

    if (processState->priority == newPriority)
        return 0;

    processState->priority = newPriority;

    return 0;
}

void sch_yieldProcess() {
    quantums = 0;
    _int81();
}

void* sch_switchProcess(void* currentRSP) {

    if (currentRunningPID != -1) {
        processStates[currentRunningPID].currentRSP = currentRSP;
    }

    if (!isReady(currentRunningPID) || quantums == 0) {
        TPid newPid = getNextReadyPid();

        while ((newPid = getNextReadyPid()) < 0)
            _hlt();

        quantums = getQuantums(newPid);
        currentRunningPID = newPid;
    } else {
        quantums -= 1;
    }

    return processStates[currentRunningPID].currentRSP;
}

int sch_getProcessInfo(TPid pid, TProcessInfo* info) {
    TProcessState* processState;
    if (!tryGetProcessState(pid, &processState))
        return 1;
    
    info->status = processStates->status;
    info->priority = processStates->priority;
    info->currentRSP = processState->currentRSP;
    return 0;
}