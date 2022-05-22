/* Standard library */
#include <string.h>

/* Local headers */
#include <loop.h>
#include <shell.h>
#include <syscalls.h>
#include <test.h>
#include <userstdlib.h>
#include <kernelTypes.h>

#define MAX_COMMAND 128
#define COUNT_COMMANDS 15 // 16
#define TRUE 1
#define FALSE 0

#define IS_VOCAL(c) ((c) == 'a' || (c) == 'A' || (c) == 'e' || (c) == 'E' || (c) == 'i' || \
                     (c) == 'I' || (c) == 'o' || (c) == 'O' || (c) == 'u' || (c) == 'U')

// TO DO: que reciba los fd de escritura y lectura
typedef void (*TVoidFunction)(void);

typedef struct {
    TVoidFunction action;
    const char* name;
    int isBuiltin;
    const char* description;
} TCommand;

// static int execute_command(char* command);
static int getCommandIndex(char* str);
static void executeCommand(char* buffer);

static void help();
static void time();
// Physical memory management
static void mem(void);
// Procesos, context switching y scheduling
static void ps(void);
static void loop(int argc, char* argv[]);
static void kill(void);
static void nice(void);
static void block(void);
// Sincronización
static void sem(void);
// Inter process communication
static void cat(int argc, char* argv[]);
static void wc(int argc, char* argv[]);
static void filter(int argc, char* argv[]);
static void pipe(void);
// static void phylo();

static TCommand valid_commands[COUNT_COMMANDS] = {
    {&help, "help", TRUE, "Displays a list of all available commands.\n"},
    {&time, "time", TRUE, "Displays the system's day and time.\n"},
    {&divideByZero, "dividezero", TRUE, "Throws an exception caused by dividing by 0.\n"},
    {&invalidOp, "invalidop", TRUE, "Throws an exception caused by an invalid operation.\n"},
    {&mem, "mem", TRUE, "Displays information about the memory status. \n"},
    {&ps, "ps", TRUE, "Displays a list of all the processes with their properties: name, ID, priority, stack, base pointer and foreground.\n"},
    {&loop, "loop", FALSE, "Prints PID once every count of seconds. \n"},
    {&kill, "kill", TRUE, "Kill the process with the given ID. \n"},
    {&nice, "nice", TRUE, "Changes the priority of the process with the given ID. \n"},
    {&block, "block", TRUE, "Blocks the process with the given ID. \n"},
    {&sem, "sem", TRUE, "Displays a list of all the semaphores with their properties: state and the blocked processes in each.\n"},
    {&cat, "cat", FALSE, "Prints the standard input on the standard output. \n"},
    {&wc, "wc", FALSE, "Pritns the newlines' count the standard input on the standard output. \n"},
    {&filter, "filter", FALSE, "Filters the vowels of the standard input. \n"},
    {&pipe, "pipe", TRUE, "Displays a list of all the pipes with their properties: state and the blocked processes in each.\n"},
    //{&phylo, "phylo", FALSE, "Resolucion del problema de los filosofos comensales \n"},
};

int getCommandIndex(char* string) {
    for (int i = 0; i < COUNT_COMMANDS; i++) {
        if (!strcmp(string, valid_commands[i].name)) {
            return i;
        }
    }
    return -1;
}

void welcome_message() {
    print("Welcome to Userland\n");
    help();
}

void wait_command() {
    // char c = 0;
    // int i = 0;
    char command[MAX_COMMAND + 1];

    int ans = fgetLine(STDIN, command, MAX_COMMAND);

    if (ans != -1) {
        executeCommand(command);
    } else {
        fprint(STDERR, "Invalid command \n");
    }

    /*
    // Read the command until the user presses enter
    while ((c = getChar()) != '\n') {
        // Just delete what the user has written
        if (c == '\b' && i > 0) {
            putChar(STDOUT, c);
            command[--i] = 0;
        } else if (c != '\b') {
            putChar(STDOUT, c);
            command[i++] = c;
        }
    }
    sprint(STDERR, "\n");
    command[i] = 0;

    // Check if de command is valid and execute it if it's valid
    if (!execute_command(command)) {
        // If not valid, show mensage
        sprint(STDERR, "Invalid command \n");
        sprint(STDERR, command);
        sprint(STDERR, "\n");
    }
    */
}

/*
int execute_command(char* command) {
    for (int i = 0; valid_commands[i].name; i++) {
        if (strcmp(command, valid_commands[i].name) == 0) {
            valid_commands[i].action();
            return 1;
        }
    }

    return 0;
} */

void executeCommand(char* str) {
    char* other = str;
    int ans = strdiv(str, &other, '|');
    // If there isn't a pipe
    if (ans == -1) {
        // Check if it's a valid command
        int index = getCommandIndex(str);
        if (index == -1) {
            fprint(STDERR, "Invalid command \n");
            return;
        }
        // If it's a builtin app, execute it
        if (valid_commands[index].isBuiltin) {
            valid_commands[index].action();
            return;
        }
        // Otherwise, it's not builtin, create a process
        TProcessCreateInfo process = {valid_commands[index].name, valid_commands[index].action, 1, DEFAULT_PRIORITY, 0, NULL}; // What is 1, 0 and NULL ??
        sys_createProcess(-1, -1, -1, &process);
    } else {
        int index1 = getCommandIndex(str);
        int index2 = getCommandIndex(other);
        int pipefd[2];
        // Check if both are valid commands
        if (index1 == -1 || index2 == -1) {
            fprint(STDERR, "Invalid command \n");
            return;
        }
        // If one of them is a builtin app, cannot pipe them
        if (valid_commands[index1].isBuiltin || valid_commands[index2].isBuiltin) {
            fprint(STDERR, "It's not possible pipe builtin commands.\n");
            return;
        }

        // Open the pipe
        //...

        // Create a process for each "command"
        TProcessCreateInfo process1 = {valid_commands[index1].name, valid_commands[index2].action, 1, DEFAULT_PRIORITY, 0, NULL}; // What is 1, 0 and NULL ??
        sys_createProcess(-1, pipefd[1], -1, &process1);

        TProcessCreateInfo process2 = {valid_commands[index1].name, valid_commands[index2].action, 1, DEFAULT_PRIORITY, 0, NULL}; // What is 1, 0 and NULL ??
        sys_createProcess(pipefd[0], -1, -1, &process2);
    }
}

void help(void) { 
    print("The available commands are:\n");

    for (int i = 0; i < COUNT_COMMANDS; i++) {
        printf("\t '%s' \t - %s", valid_commands[i].name, valid_commands[i].description);
    }
}

void loop(void) {
    TProcessCreateInfo pci = {
        .name = "loop",
        .isForeground = 1,
        .priority = DEFAULT_PRIORITY,
        .entryPoint = (TProcessEntryPoint)loopProcess,
        .argc = 0,
        .argv = NULL};
    sys_createProcess(-1, -1, -1, &pci);
}

void namedPipes(void) {
    TProcessCreateInfo pci = {
        .name = "namedPipes",
        .isForeground = 1,
        .priority = DEFAULT_PRIORITY,
        .entryPoint = (TProcessEntryPoint)namedPipesProcess,
        .argc = 0,
        .argv = NULL};
    sys_createProcess(-1, -1, -1, &pci);
}

void testMM(void) {
    TProcessCreateInfo pci = {
        .name = "testMM",
        .isForeground = 1,
        .priority = DEFAULT_PRIORITY,
        .entryPoint = (TProcessEntryPoint)test_mm,
        .argc = 0,
        .argv = NULL};
    sys_createProcess(-1, -1, -1, &pci);
}

void testAsync(void) {
    // TODO check if the 5 could be asked to the user
    printf("Async testing... \n");
    char* argv[] = {"5", "0", NULL};
    TProcessCreateInfo pci = {
        .name = "testAsync",
        .isForeground = 1,
        .priority = DEFAULT_PRIORITY,
        .entryPoint = (TProcessEntryPoint)test_sync,
        .argc = 2,
        .argv = (const char* const*)argv};
    sys_createProcess(-1, -1, -1, &pci);
}

void testSync(void) {
    // TODO check if the 5 could be asked to the user
    printf("Sync testing... \n");
    char* argv[] = {"5", "1", NULL};
    TProcessCreateInfo pci = {
        .name = "testSync",
        .isForeground = 1,
        .priority = DEFAULT_PRIORITY,
        .entryPoint = (TProcessEntryPoint)test_sync,
        .argc = 2,
        .argv = (const char* const*)argv};
    sys_createProcess(-1, -1, -1, &pci);
}

void testProcesses(void) {
    TProcessCreateInfo pci = {
        .name = "testSync",
        .isForeground = 0,
        .priority = DEFAULT_PRIORITY,
        .entryPoint = (TProcessEntryPoint)test_processes,
        .argc = 0,
        .argv = NULL};
    sys_createProcess(-1, -1, -1, &pci);
}

void time(void) {
    char time[11];
    sys_time(time);
    printf("\n Time: %s", time);

    char date[11];
    sys_date(date);
    printf("\n Date: %s", date);

    printf("\n Millis since startup: %u\n", (unsigned int)sys_millis());
}

void mem(void) {
     TMemoryState memoryState;
    if (sys_memState(&memoryState)) {
        print("Failed to get memory state.\n");
        return;
    }

    printf("Memory Manager Type: %s\n", memoryState.type == NODE ? "NODE" : memoryState.type == BUDDY ? "BUDDY" : "UNKNOWN");
    printf("Total memory: %u.", memoryState.total);
    printf(" Used: %u (%u%%).", memoryState.used, (memoryState.used * 100 / memoryState.total));
    printf(" Available: %u.\n", memoryState.total - memoryState.used);
    printf("Total chunks: %u\n", memoryState.chunks);
}


void ps(void) {
    TProcessInfo arr[16];
    int count = sys_listProcesses(arr, 16);
    printf("Listing %d process/es: \n", count);
    for (int i = 0; i < count; i++) {
        const char* status = arr[i].status == READY ? "READY" : arr[i].status == RUNNING ? "RUNNING" : arr[i].status == BLOCKED
            ? "BLOCKED" : arr[i].status == KILLED ? "KILLED" : "UNKNOWN";

        printf("pid=%d, name=%s, stackEnd=%x, stackStart=%x, isForeground=%d, priority=%d, status=%s, rsp=%x\n",
        arr[i].pid, arr[i].name, arr[i].stackEnd, arr[i].stackStart, arr[i].isForeground, arr[i].priority, status, arr[i].currentRSP);
    }
}

void loop(int argc, char* argv[]) {
    // Solicitar un pid ??

    // Imprimir
    /*while(1){
        printf("\n PID: %d \n", pid);
        sleep();
    } */

    // sys_exit();
}

void kill(void) {
    // Solicitar al usuario el PID del proceso a matar

    // Chequear que sea un PID valido

    // Llamar a una syscall pasandole el PID ?
}

void nice(void) {
    // Solicitar al usuario el PID del proceso a cambiar su prioridad

    // Chequear que sea un PID valido

    // Solicitar la nueva prioridad

    // Chequear que sea un prioridad valida

    // Llamar a una syscall pasandolo el PID y la prioridad ??
}

void block(void) {
    // Solicitar al usuario el PID del proceso a bloquear

    // Chequear que sea un PID valido

    // Llamar a una syscall pasandolo el PID ??
}

void sem(void) {
    // Llamar a una syscall ??
}

void cat(int argc, char* argv[]) {
    // Leer de STDIN e imprime
    int c;
    while ((c = getChar()) != -1) {
        putChar(c);
    }
}

void wc(int argc, char* argv[]) {
    int c;
    int cantLines = 0;
    // Leer de STDIN la cantidad de '\n'
    while ((c = getChar()) != -1) {
        if (c == '\n') {
            cantLines++;
        }
    }
    printf("%d", cantLines);
}

void filter(int argc, char* argv[]) {
    int c;
    // Leer de STDIN e mprimir solo las vocales
    while ((c = getChar()) != -1) {
        if (IS_VOCAL(c)) {
            putChar(c);
        }
    }
}

void pipe(void) {
    print("Listing pipes: ");
    TPipeInfo array[16];
    int count = sys_listPipes(array, 16);
    printf("%d\n", count);

    for (int i = 0; i < count; i++) {
        printf("bytes=%u, readers=%u, writers=%u, name=%s", (unsigned int)array[i].remainingBytes, (unsigned int)array[i].readerFdCount, (unsigned int)array[i].writerFdCount, array[i].name);

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
}