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

// Suggested - implement the structure you think that fits better.
// This array stores pointers to the ready processes using as idx their pid
// An array is a good idea JUST if there is a small qty of processes (which is the case we are having now)
// A list would be a better solution if the MAX_PROCESS_QTY was a bigger number
// static TPid readyProcesses[MAX_PROCESSES];

static int is_valid_pid(TPid pid);
static int is_ready(TPid pid);
static int is_active(TPid pid);
static int tryGetProcessState(TPid pid, TProcessState** outState);

static int tryGetProcessState(TPid pid, TProcessState** outState) {
    if (pid < 0 || pid >= MAX_PROCESSES || processStates[pid].currentRSP == NULL)
        return 0;

    *outState = &processStates[pid];
    return 1;
}


void sch_init() {
    currentRunningProcessPID = -1;
}

int sch_onProcessCreated(TPid pid, TProcessEntryPoint entryPoint, void* currentRSP, int argc, const char* argv[]) {
    // Processes, by default, are created in the state READY.
    processStates[pid].priority = DEFAULT_PRIORITY;
    processStates[pid].status = READY;
    
    char * argvCopy = mm_malloc(sizeof(char*) * (argc + 1));

    // TO DO: check if a deep copy is needed
    for(int i=0 ; i<argc ; ++i){
        argvCopy[i] = argv[i]; 
    }
    
    argvCopy[argc] = NULL;

    processStates[pid].currentRSP = _createProcessContext(argc, argvCopy, currentRSP, entryPoint);

    
    //TO DO: make sure the argvCopy is freed when the process is killed

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



void* sch_switchProcess(void* currentRSP) {

    if(currentRunningProcessPID!=-1){
        processStates[currentRunningProcessPID].currentRSP = currentRSP;
    }

    if(!is_ready(currentRunningProcessPID) || quantums == 0){
        TPid newPid = get_next_ready_pid();
        if(newPid == -1){
            // no ready process available, decide what to do
        }else{
            quantums = get_quantums(newPid);
            currentRunningProcessPID = newPid;
        }
    }else{
        quantums-=1;
    }
    
    /* Main idea:
      Save running process currentStackPointer
      Check if quantum has finished or there is another process with higher priority to preempt
      Check process status
      If we have to switch a process, we return the currentStackPointer of the other process.
      If not, we return currentStackPointer.
    */
    return processStates[currentRunningProcessPID].currentRSP;
}
