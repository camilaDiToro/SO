/* Local headers */
#include <shell.h>
#include <userstdlib.h>
#include <syscalls.h>

/*void recurrent_screaming(int argc, char* argv[]) {
    while (1) {
        print(argv[0]);
    }
}*/

/*void pipeWriter(int argc, char* argv[]) {
    sys_close(5);
    for (int i=0; i<600000; i++)
        sys_yield();
    print("Writing: \"big \"\n");
    fprint(6, "big ");
    for (int i=0; i<600000; i++)
        sys_yield();
    print("Writing: \"monke.\"\n");
    fprint(6, "monke.");
    for (int i=0; i<600000; i++)
        sys_yield();
    fprint(6, "goodbye :)");
    print("Writer exiting.\n");
    sys_exit();
    fprint(STDERR, "Why writer still alive?\n");
}

void pipeReader(int argc, char* argv[]) {
    sys_close(6);
    char buf[512];
    ssize_t count;
    do {
        print("Reading 512...\n");
        count = sys_read(5, buf, 511);
        if (count <= 0)
            break;

        buf[count] = '\0';
        printf("Read %d: \"%s\".\n", (int)count, buf);
    } while (1);

    printf("Reader exiting after read returned %d.\n", (int)count);
    sys_exit();
    fprintf(STDERR, "Why reader still alive?\n");
}*/

void main(int argc, char* argv[]) {
    /*TProcessCreateInfo infoB = {"reader", pipeReader, 1, 0, (const char* const*) NULL};
    sys_createProcess(-1, -1, -1, &infoB);

    TProcessCreateInfo infoA = {"writer", pipeWriter, 1, 0, (const char* const*) NULL};
    sys_createProcess(-1, -1, -1, &infoA);*/



    /*char* argsA[] = {"A"};
    TProcessCreateInfo infoA = {"screamAAA", recurrent_screaming, 1, 1, (const char* const*) argsA};
    sys_createProcess(-1, -1, -1, &infoA);
    
    char* argsB[] = {"B"};
    TProcessCreateInfo infoB = {"screamBBB", recurrent_screaming, 1, 1, (const char* const*) argsB};
    sys_createProcess(-1, -1, -1, &infoB);
    
    char* argsC[] = {"C"};
    TProcessCreateInfo infoC = {"screamCCC", recurrent_screaming, 1, 1, (const char* const*) argsC};
    sys_createProcess(-1, -1, -1, &infoC);*/
    
    welcome_message();
    while (1) {
        fputChar(STDERR, '>');
        wait_command();
    }
}