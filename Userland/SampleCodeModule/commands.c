/* Local headers */
#include <commands.h>
#include <syscalls.h>
#include <string.h>
#include <userstdlib.h>

#define IS_VOCAL(c) ((c) == 'a' || (c) == 'A' || (c) == 'e' || (c) == 'E' || (c) == 'i' || \
                     (c) == 'I' || (c) == 'o' || (c) == 'O' || (c) == 'u' || (c) == 'U')

static int runHelp(int stdin, int stdout, int stderr, int isForeground, int argc, const char* const argv[], TPid* createdProcess);
static int runClear(int stdin, int stdout, int stderr, int isForeground, int argc, const char* const argv[], TPid* createdProcess);
static int runTime(int stdin, int stdout, int stderr, int isForeground, int argc, const char* const argv[], TPid* createdProcess);
static int runDivideByZero(int stdin, int stdout, int stderr, int isForeground, int argc, const char* const argv[], TPid* createdProcess);
static int runInvalidOp(int stdin, int stdout, int stderr, int isForeground, int argc, const char* const argv[], TPid* createdProcess);
// Physical memory management
static int runMem(int stdin, int stdout, int stderr, int isForeground, int argc, const char* const argv[], TPid* createdProcess);
// Procesos, context switching y scheduling
static int runPs(int stdin, int stdout, int stderr, int isForeground, int argc, const char* const argv[], TPid* createdProcess);
static int runLoop(int stdin, int stdout, int stderr, int isForeground, int argc, const char* const argv[], TPid* createdProcess);
static int runKill(int stdin, int stdout, int stderr, int isForeground, int argc, const char* const argv[], TPid* createdProcess);
static int runNice(int stdin, int stdout, int stderr, int isForeground, int argc, const char* const argv[], TPid* createdProcess);
static int runBlock(int stdin, int stdout, int stderr, int isForeground, int argc, const char* const argv[], TPid* createdProcess);
// Sincronización
static int runSem(int stdin, int stdout, int stderr, int isForeground, int argc, const char* const argv[], TPid* createdProcess);
// Inter process communication
static int runCat(int stdin, int stdout, int stderr, int isForeground, int argc, const char* const argv[], TPid* createdProcess);
static int runWc(int stdin, int stdout, int stderr, int isForeground, int argc, const char* const argv[], TPid* createdProcess);
static int runFilter(int stdin, int stdout, int stderr, int isForeground, int argc, const char* const argv[], TPid* createdProcess);
static int runPipe(int stdin, int stdout, int stderr, int isForeground, int argc, const char* const argv[], TPid* createdProcess);
static int runPhylo(int stdin, int stdout, int stderr, int isForeground, int argc, const char* const argv[], TPid* createdProcess);

static TCommand valid_commands[] = {
    {(TCommandFunction)runHelp, "help", "Displays a list of all available commands.\n"},
    {(TCommandFunction)runClear, "clear", "Clear windows' shell.\n"},
    {(TCommandFunction)runTime, "time", "Displays the system's day and time.\n"},
    {(TCommandFunction)runDivideByZero, "dividezero", "Throws an exception caused by dividing by 0.\n"},
    {(TCommandFunction)runInvalidOp, "invalidop", "Throws an exception caused by an invalid operation.\n"},
    {(TCommandFunction)runMem, "mem", "Displays information about the memory status. \n"},
    {(TCommandFunction)runPs, "ps", "Displays a list of all the processes with their properties: name, ID, priority, stack, base pointer and foreground.\n"},
    {(TCommandFunction)runLoop, "loop", "Prints PID once every count of seconds. \n"},
    {(TCommandFunction)runKill, "kill", "Kill the process with the given ID. \n"},
    {(TCommandFunction)runNice, "nice", "Changes the priority of the process with the given ID. \n"},
    {(TCommandFunction)runBlock, "block", "Blocks the process with the given ID. \n"},
    {(TCommandFunction)runSem, "sem", "Displays a list of all the semaphores with their properties: state and the blocked processes in each.\n"},
    {(TCommandFunction)runCat, "cat", "Prints the standard input on the standard output. \n"},
    {(TCommandFunction)runWc, "wc", "Pritns the newlines' count the standard input on the standard output. \n"},
    {(TCommandFunction)runFilter, "filter", "Filters the vowels of the standard input. \n"},
    {(TCommandFunction)runPipe, "pipe", "Displays a list of all the pipes with their properties: state and the blocked processes in each.\n"},
    {(TCommandFunction)runPhylo, "phylo", "Resolve the Dining philosophers problem.\n"},
};

const TCommand * getCommandByName(const char * name){
    for (int i=0; i < (sizeof(valid_commands) / sizeof(valid_commands[0])); i++) {
        if (!strcmp(name, valid_commands[i].name)) {
            return &valid_commands[i];
        }
    }
    return NULL;
}

//------------------------------------------------------------------------------------------------------------

int runHelp(int stdin, int stdout, int stderr, int isForeground, int argc, const char* const argv[], TPid* createdProcess) {

    print("The available commands are:\n");

    for (int i = 0; i < (sizeof(valid_commands) / sizeof(valid_commands[0])); i++) {
        printf("\t '%s' \t - %s", valid_commands[i].name, valid_commands[i].description);
    }

    return 1;
}

//------------------------------------------------------------------------------------------------------------

int runClear(int stdin, int stdout, int stderr, int isForeground, int argc, const char* const argv[], TPid* createdProcess) {

    return 1;
}

//------------------------------------------------------------------------------------------------------------

int runTime(int stdin, int stdout, int stderr, int isForeground, int argc, const char* const argv[], TPid* createdProcess) {
    char time[11];
    sys_time(time);
    printf("\n Time: %s", time);

    char date[11];
    sys_date(date);
    printf("\n Date: %s", date);

    printf("\n Millis since startup: %u\n", (unsigned int)sys_millis());

    return 1;
}

//------------------------------------------------------------------------------------------------------------
int runDivideByZero(int stdin, int stdout, int stderr, int isForeground, int argc, const char* const argv[], TPid* createdProcess) {
    divideByZero();
    return 1;
}

//------------------------------------------------------------------------------------------------------------

int runInvalidOp(int stdin, int stdout, int stderr, int isForeground, int argc, const char* const argv[], TPid* createdProcess) {
    invalidOp();
    return 1;
}

//------------------------------------------------------------------------------------------------------------

int runMem(int stdin, int stdout, int stderr, int isForeground, int argc, const char* const argv[], TPid* createdProcess) {
    TMemoryState memoryState;
    if (sys_memState(&memoryState)) {
        print("Failed to get memory state.\n");
        return -1;
    }

    printf("Memory Manager Type: %s\n", memoryState.type == NODE ? "NODE" : memoryState.type == BUDDY ? "BUDDY"
                                                                                                      : "UNKNOWN");
    printf("Total memory: %u.", memoryState.total);
    printf(" Used: %u (%u%%).", memoryState.used, (memoryState.used * 100 / memoryState.total));
    printf(" Available: %u.\n", memoryState.total - memoryState.used);
    printf("Total chunks: %u\n", memoryState.chunks);

    return 1;
}

//------------------------------------------------------------------------------------------------------------

int runPs(int stdin, int stdout, int stderr, int isForeground, int argc, const char* const argv[], TPid* createdProcess) {
    TProcessInfo arr[16];
    int count = sys_listProcesses(arr, 16);
    printf("Listing %d process/es: \n", count);

    for (int i = 0; i < count; i++) {
        const char* status = arr[i].status == READY ? "READY" : arr[i].status == RUNNING ? "RUNNING"
                                                            : arr[i].status == BLOCKED   ? "BLOCKED"
                                                            : arr[i].status == KILLED    ? "KILLED"
                                                                                         : "UNKNOWN";

        printf("pid=%d, name=%s, stackEnd=%x, stackStart=%x, isForeground=%d, priority=%d, status=%s, rsp=%x\n",
               arr[i].pid, arr[i].name, arr[i].stackEnd, arr[i].stackStart, arr[i].isForeground, arr[i].priority, status, arr[i].currentRSP);
    }

    return 1;
}

//------------------------------------------------------------------------------------------------------------

void loop(void) {
}

int runLoop(int stdin, int stdout, int stderr, int isForeground, int argc, const char* const argv[], TPid* createdProcess) {
     TProcessCreateInfo p_loop = {
        .name = "loop",
        .entryPoint = (TProcessEntryPoint) loop,
        .isForeground = isForeground,
        .priority = DEFAULT_PRIORITY,
        .argc = argc,
        .argv = argv
    };
    //TProcessCreateInfo p_loop = {"loop", loop, isForeground, DEFAULT_PRIORITY, argc, argv};
    return sys_createProcess(stdin, stdout, stderr, &p_loop);
    // return atoi(argv[0]);   //Thomas me dijo que el pid estaba en argv[0]
}

//------------------------------------------------------------------------------------------------------------

int runKill(int stdin, int stdout, int stderr, int isForeground, int argc, const char* const argv[], TPid* createdProcess) {
    if (argc == 0) {
        fprintf(stdout, "Sos un gil");
        return -1;
    }

    TPid pidToKill = atoi(argv[0]);
    int result = sys_killProcess(pidToKill);
    if (result == 0) {
        fprintf(stdout, "Killed process %d.\n", pidToKill);
    } else {
        fprintf(stdout, "Failed to kill process %d. Error: %d.\n", pidToKill, result);
    }

    return 1;
}

//------------------------------------------------------------------------------------------------------------

// Solicitar al usuario el PID del proceso a cambiar su prioridad

// Chequear que sea un PID valido

// Solicitar la nueva prioridad

// Chequear que sea un prioridad valida

// Llamar a una syscall pasandolo el PID y la prioridad ??

int runNice(int stdin, int stdout, int stderr, int isForeground, int argc, const char* const argv[], TPid* createdProcess) {

    return 1;
}

//------------------------------------------------------------------------------------------------------------

// Solicitar al usuario el PID del proceso a bloquear

// Chequear que sea un PID valido

// Llamar a una syscall pasandolo el PID ??

int runBlock(int stdin, int stdout, int stderr, int isForeground, int argc, const char* const argv[], TPid* createdProcess) {

    return 1;
}

//------------------------------------------------------------------------------------------------------------

int runSem(int stdin, int stdout, int stderr, int isForeground, int argc, const char* const argv[], TPid* createdProcess) {

    return 1;
}

//------------------------------------------------------------------------------------------------------------

void cat(void) {
    int c;
    while ((c = getChar()) != '\n') {
        putChar(c);
    }
}

int runCat(int stdin, int stdout, int stderr, int isForeground, int argc, const char* const argv[], TPid* createdProcess) {
    TProcessCreateInfo p_cat = {
        .name = "cat",
        .entryPoint = (TProcessEntryPoint) cat,
        .isForeground = isForeground,
        .priority = DEFAULT_PRIORITY,
        .argc = argc,
        .argv = argv
    };
    //TProcessCreateInfo p_cat = {"cat", cat, isForeground, DEFAULT_PRIORITY, argc, argv};
    *createdProcess = sys_createProcess(stdin, stdout, stderr, &p_cat);
    return 1;
}

//------------------------------------------------------------------------------------------------------------

void wc(void) {
    int c;
    int cantLines = 0;
    while ((c = getChar()) != -1) {
        if (c == '\n') {
            cantLines++;
        }
    }
    printf("%d", cantLines);
}

int runWc(int stdin, int stdout, int stderr, int isForeground, int argc, const char* const argv[], TPid* createdProcess) {
    TProcessCreateInfo p_wc = {
        .name = "wc",
        .entryPoint = (TProcessEntryPoint) wc,
        .isForeground = isForeground,
        .priority = DEFAULT_PRIORITY,
        .argc = argc,
        .argv = argv
    };
    //TProcessCreateInfo p_wc = {"wc", wc, isForeground, DEFAULT_PRIORITY, argc, argv};
    *createdProcess = sys_createProcess(stdin, stdout, stderr, &p_wc);
    return 1;
}

//------------------------------------------------------------------------------------------------------------

void filter(void) {
    int c;
    while ((c = getChar()) != -1) {
        if (IS_VOCAL(c)) {
            putChar(c);
        }
    }
}

int runFilter(int stdin, int stdout, int stderr, int isForeground, int argc, const char* const argv[], TPid* createdProcess) {
    TProcessCreateInfo p_filter = {
        .name = "filter",
        .entryPoint = (TProcessEntryPoint) filter,
        .isForeground = isForeground,
        .priority = DEFAULT_PRIORITY,
        .argc = argc,
        .argv = argv
    };
    //TProcessCreateInfo p_filter = {"filter", filter, isForeground, DEFAULT_PRIORITY, argc, argv};
    *createdProcess = sys_createProcess(stdin, stdout, stderr, &p_filter);
    return 1;
}

//------------------------------------------------------------------------------------------------------------

int runPipe(int stdin, int stdout, int stderr, int isForeground, int argc, const char* const argv[], TPid* createdProcess) {
    print("Listing pipes: ");
    TPipeInfo array[16];
    int count = sys_listPipes(array, 16);
    printf("%d\n", count);

    for (int i = 0; i < count; i++) {
        printf("bytes=%u, readers=%u, writers=%u, name=%s", (unsigned int)array[i].remainingBytes, (unsigned int)array[i].readerFdCount,
               (unsigned int)array[i].writerFdCount, array[i].name);

        printf(", readBlocked={");
        for (int c = 0; array[i].readBlockedPids[c] >= 0; c++) {
            if (c != 0)
                printf(", ");
            printf("%d", array[i].readBlockedPids[c]);
        }
        printf("}");

        printf(", writeBlocked={");
        for (int c = 0; array[i].writeBlockedPids[c] >= 0; c++) {
            if (c != 0)
                printf(", ");
            printf("%d", array[i].writeBlockedPids[c]);
        }
        printf("}\n");
    }

    return 1;
}

//------------------------------------------------------------------------------------------------------------

int runPhylo(int stdin, int stdout, int stderr, int isForeground, int argc, const char* const argv[], TPid* createdProcess) {
    return 1;
}