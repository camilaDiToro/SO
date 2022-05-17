/* Local headers */
#include <shell.h>
#include <userstdlib.h>
#include <syscalls.h>

void pipeWriter(int argc, char* argv[]) {
    for (int i=0; i<600000; i++)
        sys_yield();
    print("Writing: \"big \"\n");
    fprint(STDOUT, "big ");
    for (int i=0; i<600000; i++)
        sys_yield();
    print("Writing: \"monke.\"\n");
    fprint(STDOUT, "monke.");
    for (int i=0; i<600000; i++)
        sys_yield();
    fprint(STDOUT, "goodbye :)");
    print("Writer exiting.\n");
    sys_exit();
    fprint(STDERR, "Why writer still alive?\n");
}

void pipeReader(int argc, char* argv[]) {
    char buf[512];
    ssize_t count;
    do {
        print("Reading 512...\n");
        count = sys_read(STDIN, buf, 511);
        if (count <= 0)
            break;

        buf[count] = '\0';
        printf("Read %d: \"%s\".\n", (int)count, buf);
    } while (1);

    printf("Reader exiting after read returned %d.\n", (int)count);
    sys_exit();
    fprintf(STDERR, "Why reader still alive?\n");
}

void main(int argc, char* argv[]) {
    int pipefd[2];
    sys_pipe(pipefd);

    TProcessCreateInfo infoB = {"reader", &pipeReader, 1, 0, (const char* const*) NULL};
    sys_createProcess(pipefd[0], -1, -1, &infoB);

    TProcessCreateInfo infoA = {"writer", &pipeWriter, 1, 0, (const char* const*) NULL};
    sys_createProcess(-1, pipefd[1], -1, &infoA);

    sys_close(pipefd[0]);
    sys_close(pipefd[1]);
    
    welcome_message();
    while (1) {
        fputChar(STDERR, '>');
        wait_command();
    }
}