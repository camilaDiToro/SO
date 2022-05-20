/* Standard library */
#include <string.h>

/* Local headers */
#include <shell.h>
#include <syscalls.h>
#include <userstdlib.h>


typedef void (*TVoidFunction)(void);

typedef struct {
    TVoidFunction action;
    const char* name;
} TCommand;

static int execute_command(char* command);
static void help();
static void time();
static void ps();
static void pipe();

static TCommand valid_commands[] = {
    {&help, "help"},
    {&time, "time"},
    {&divideByZero, "dividezero"},
    {&invalidOp, "invalidop"},
    {&ps, "ps"},
    {&pipe, "pipe"},
    {0, 0}};

void welcome_message() {
    print("Bienvenido a Userland\n");
    help();
}

void wait_command() {
    char c = 0;
    char command[128];
    int i = 0;

    // Read the command until the user presses enter
    while ((c = getChar()) != '\n') {
        // Just delete what the user has written
        if (c == '\b' && i > 0) {
            putChar(c);
            command[--i] = 0;
        } else if (c != '\b') {
            putChar(c);
            command[i++] = c;
        }
    }
    fprint(STDERR, "\n");
    command[i] = 0;

    // Check if de command is valid.
    // Execute it if its valid
    if (!execute_command(command)) {
        // If not valid, show mensage
        fprint(STDERR, "Comando invalido \n");
        fprint(STDERR, command);
        fprint(STDERR, "\n");
    }
}

int execute_command(char* command) {
    for (int i = 0; valid_commands[i].name; i++) {
        if (strcmp(command, valid_commands[i].name) == 0) {
            valid_commands[i].action();
            return 1;
        }
    }

    return 0;
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

void help(void) {
    printf("Los comandos disponibles son:\n");

    // help
    printf("   'help'       - Despliega un listado de todos los comandos disponibles.\n");

    // time
    printf("   'time'       - Despliega el dia y hora del sistema.\n");

    // inforeg
    printf("   'inforeg'    - Imprime el valor de todos los registros. \n");
    printf("                  Se debe presionar '-' para guardar el estado de los reistros. \n");

    // printmem
    printf("   'printmem'   - Realiza un volcado de memoria de 32 bytes a partir de la direccion recibida como parametro.\n");

    // Division by 0
    printf("   'dividezero' - Genera una excepcion causada por dividir por 0.\n");

    // Invalid operation
    printf("   'invalidop'  - Genera una excepcion causada por una operacion invalida.\n");
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

static void pipe() {
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