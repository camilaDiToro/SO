/* Standard library */
#include <stdint.h>
#include <sys/types.h>

/* Local headers */
#include <pipe.h>
#include <process.h>
#include <scheduler.h>
#include <graphics.h>

static TProcess processes[MAX_PROCESSES] = {NULL};

TProcess prc_create(void* functionAddress, uint8_t priority, int argc, const char* argv[]) {
    // Crear un proceso, deberia crear su contexto y su stack, le avisa al sch que se QUIERE cargar un proceso
    // llamando a sch_onCreateProcess
    int i = 0;
    for (; i < MAX_PROCESSES && processes[i] != NULL; i++);

    if (i == MAX_PROCESSES)
        return NULL;

    TProcess process = NULL;
    void* stackEnd = NULL;

    if ((process == mm_malloc(sizeof(TProcess))) == NULL ||
        ((stackEnd = mm_malloc(PROCESS_STACK_SIZE)) == NULL) {
        mm_free(process);
        return NULL;
    }

    process->pid = i;
    process->priority = DEFAULT_PRIORITY;
    process->status = READY;
    process->functionAddress = functionAddress;
    process->stackEnd = stackEnd;
    process->stackStart = stackEnd + PROCESS_STACK_SIZE;
    process->currentRSP = stackStart;

    processes[i] = process;
    sch_onProcessCreated(process);

    return process;
}

void prc_kill(TProcess process) {
    process->status = KILLED;
    sch_onProcessKilled(process);
    processes[process->pid] = 
}

TProcess prc_getProcessFromPid(uint8_t pid) {
    return processes[pid];
}

// This should be done in process.c, since now we just have de ready processes available here
void prc_printProcesses() {
    scr_print("Process list:\n");
    for (int i = 0; i < MAX_PROCESSES; i++) {
        if (processes[i] != NULL) {
            scr_print("Process ");
            scr_printDec(i);
            scr_print(": ");
            scr_print("STATE: ");
            scr_printDec(processes[i]->status);
            scr_printLine();
        }
    }
}