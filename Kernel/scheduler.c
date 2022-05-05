/* Local Headers */
#include "time.h"
#include "scheduler.h"
#include "graphics.h"
#include "memoryManager.h"

#define MAX_PROCESSES 16
#define QUANTUM 4
#define PROCESS_STACK_SIZE 4096  

typedef enum status {READY = 0, BLOCKED, KILLED} TprocessStatus;

typedef struct processContext {
    uint8_t priority;
    TprocessStatus status;
    void* topRSP;
    void* baseRSP;
    void* currentRSP;
    void* functionAddress;
} TProcessBlock;

static TProcessBlock * processes[MAX_PROCESSES] = {NULL};
static uint8_t currentRunningProcessPID = 0;

static int validPID(uint8_t pid);

void sch_init(void* functionAddress, uint8_t priority, int argc, const char * argv[]){

    // TO DO: Add halt process to avoid busy waiting?

    // First process to run is the shell (SampleCodeModule)
    int PID = sch_loadProcess(functionAddress, priority, argc, argv);
    if(-1 == PID){
        return -1;
    }

    currentRunningProcessPID = PID;

    // ¿Open here process context? (Check Intel TSS)

    return 0;
}

int sch_loadProcess(void* functionAddress, uint8_t priority, int argc, const char * argv[]){

    int i=0;
    for (; i<MAX_PROCESSES && processes[i] != NULL; i++);

    if (i == MAX_PROCESSES)
        return -1;

    TProcessBlock * pblock = mm_malloc(sizeof(TProcessBlock));

    if (pblock == NULL)
        return -1;

    uint64_t topRSP = mm_malloc(PROCESS_STACK_SIZE);

    if (topRSP == NULL) {
        free(pblock);
        return -1;
    }

    // TO DO: Define priority set (0,1,2, ...)
    pblock->priority = priority;
    pblock->status = READY;
    pblock->functionAddress = functionAddress;
    pblock->topRSP = topRSP;
    pblock->baseRSP = topRSP + PROCESS_STACK_SIZE;
    pblock->currentRSP = // TO DO: Build process context (Check Intel TSS)

    if(pblock-> currentRSP == NULL){
        return -1;
    }

    processes[i] = pblock;
    return i;
}

void* sch_switchProcess(uint64_t * currentRSP){
    processes[currentRunningProcessPID]->currentRSP = currentRSP;
    // TO DO

    /* Main idea:
      Save running process currentStackPointer
      Check if quantum has finished or there is another process with higher priority to preempt 
      Check process status
      If we have to switch a process, we return the currentStackPointer of the other process.
      If not, we return currentStackPointer.
    */
}

uint8_t sch_getCurrentPID(){
    return currentRunningProcessPID;
}

int sch_kill(uint8_t pid){

    if(!validPID(pid)){
        return -1;
   } 

   // What happens if pid = SampleCodeModuleProcess?
   if(processes[pid]->status == READY){
        processes[pid]->status = KILLED;
        return 0;
   } else {
        return -1;
   }
}

int sch_block(uint8_t pid){

   if(!validPID(pid)){
        return -1;
   } 
   
   // What happens if pid = SampleCodeModuleProcess?
   if(processes[pid]->status == READY){
        processes[pid]->status = BLOCKED;
        return 0;
   } else {
        return -1;
   }
   // If calling some of these functions during an interruption, should we change a process' status to READY? 
   // We're about to hand control right back to it anyways
}

int sch_unblock(uint8_t pid){

   if(!validPID(pid)){
        return -1;
   } 
   
   if(processes[pid]->status == BLOCKED){
        processes[pid]->status = READY;
        return 0;
   } else {
        return -1;
   }

}

void sch_getProcesses(){
    scr_print("Process list:\n");
    for (int i=0; i < MAX_PROCESSES; i++){
        if (processes[i] != NULL) {
            scr_print("Process ");
            scr_printDec(i);
            scr_print(": ");
            scr_print("STATE: ");
            scr_printDec(processes[i]->status);
            scr_print('\n');
        }
    }

    return;
}

int sch_nice(uint8_t pid, uint8_t priority){

    if(!validPID(pid)){
        return -1;
    } 

    // TO DO: check priority
    // What happens if pid = SampleCodeModuleProcess?
    processes[pid]->priority = priority;
}

void sch_freeProcess(uint8_t pid) {
    free(processes[pid]);
    processes[pid] = NULL;
    return;
}

static int validPID(uint8_t pid){
    return pid < MAX_PROCESSES;
}
