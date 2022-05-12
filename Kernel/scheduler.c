/* Local Headers */
#include <scheduler.h>
#include <time.h>
#include <graphics.h>
#include <memoryManager.h>
#include <process.h>

#define QUANTUM 4

static TProcessState processStates[MAX_PROCESSES];
static TPid currentRunningProcessPID = -1;
static uint8_t quantums = 0;

static int is_valid_pid(TPid pid);
static int is_ready(TPid pid);
static int is_active(TPid pid);
static int tryGetProcessState(TPid pid, TProcessState** outState);
static void haltUntilNextProcessReady();
extern void _int20();
extern void _hlt();

static int tryGetProcessState(TPid pid, TProcessState** outState) {
    if (pid < 0 || pid >= MAX_PROCESSES || processStates[pid].currentRSP == NULL)
        return 0;

    *outState = &processStates[pid];
    return 1;
}

void sch_init() {
    currentRunningProcessPID = -1;
    quantums = 0;
}

int sch_onProcessCreated(TPid pid, TProcessEntryPoint entryPoint, void* currentRSP, int argc, const char* argv[]) {
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

    if (currentRunningProcessPID == pid)
        currentRunningProcessPID = -1;

    return 0;
}

int sch_blockProcess(TPid pid) {
    TProcessState* processState;
    if (!tryGetProcessState(pid, &processState))
        return 1;

    if (processState->status == BLOCKED)
        return 0;
    
    processState->status = BLOCKED;

    if (currentRunningProcessPID == pid)
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
    return currentRunningProcessPID;
}

int sch_setProcessPriority(TPid pid, TPriority newPriority) {
    TProcessState* processState;
    if (!tryGetProcessState(pid, &processState))
        return 1;

    if(newPriority < MAX_PRIORITY || newPriority > MIN_PRIORITY)
        return 1;

    if (processState->priority == newPriority)
        return 0;

    processState->priority = newPriority;

    return 0;
}

int sch_getProcessState(TPid pid, TProcessState* outState) {
    TProcessState* processState;
    if (!tryGetProcessState(pid, &processState))
        return 1;

    *outState = *processState;
    return 0;
}

void sch_yieldProcess(){
    quantums = 0;

    // TO DO: try to avoid ticking 
    // Waiting for the next tick will result in busy waiting?
    _int20();
}

static int is_valid_pid(TPid pid){
    return pid >=0 && pid<MAX_PROCESSES;
}

static int is_active(TPid pid){
    return is_valid_pid(pid) && processStates[pid].currentRSP!=NULL;
}

static int is_ready(TPid pid){
    return is_active(pid) && processStates[pid].status == READY;
}

static int get_quantums(TPid pid){
    return MIN_PRIORITY - processStates[pid].priority;
}

static TPid get_next_ready_pid(){
    TPid next = currentRunningProcessPID;

    do{
        next = (next + 1)%MAX_PROCESSES;
        if(is_ready(next)){
            return next;
        }
    }while(next!=currentRunningProcessPID);
    return -1;
}

static void haltUntilNextProcessReady(){
    while(get_next_ready_pid() == -1){
        _hlt();
    }
}

void* sch_switchProcess(void* currentRSP) {

    if(currentRunningProcessPID!=-1){
        processStates[currentRunningProcessPID].currentRSP = currentRSP;
    } 

    if(!is_ready(currentRunningProcessPID) || quantums == 0){
        TPid newPid = get_next_ready_pid();
        if(newPid == -1){
            haltUntilNextProcessReady();
        } else {
            quantums = get_quantums(newPid);
            currentRunningProcessPID = newPid;
        }
    }else{
        quantums-=1;
    }

    return processStates[currentRunningProcessPID].currentRSP;
}
