/* Local headers */
#include <shell.h>
#include <userstdlib.h>
#include <syscalls.h>

void pipeWriter(int argc, char* argv[]) {
    int pipefd[2];
    sys_openPipe("ketchup", pipefd);
    sys_close(pipefd[0]);

    for (int i = 0; i < 600000; i++)
        sys_yield();
    fprint(STDERR, "Writing: \"big \"\n");
    fprint(pipefd[1], "big ");
    for (int i = 0; i < 600000; i++)
        sys_yield();
    fprint(STDERR, "Writing: \"monke.\"\n");
    fprint(pipefd[1], "monke.");
    for (int i = 0; i < 600000; i++)
        sys_yield();
    fprint(pipefd[1], "goodbye :)");
    fprint(STDERR, "Writer exiting.\n");
    sys_exit();
    fprint(STDERR, "Why writer still alive?\n");
}

void pipeReader(int argc, char* argv[]) {
    int pipefd[2];
    sys_openPipe("ketchup", pipefd);
    sys_close(pipefd[1]);

    char buf[512];
    ssize_t count;
    do {
        fprint(STDERR, "Reading 512...\n");
        count = sys_read(pipefd[0], buf, 511);
        sys_unlinkPipe("ketchup");
        if (count <= 0)
            break;

        buf[count] = '\0';
        printf("Read %d: \"%s\".\n", (int)count, buf);
    } while (1);

    fprintf(STDERR, "Reader exiting after read returned %d.\n", (int)count);
    sys_exit();
    fprintf(STDERR, "Why reader still alive?\n");
}

void main(int argc, char* argv[]) {
    // int pipefd[2];
    // sys_pipe(pipefd);

    TProcessCreateInfo infoB = {"reader", &pipeReader, 1, 0, (const char* const*)NULL};
    sys_createProcess(-1, -1, -1, &infoB);

    TProcessCreateInfo infoA = {"writer", &pipeWriter, 1, 0, (const char* const*)NULL};
    sys_createProcess(-1, -1, -1, &infoA);

    // sys_close(pipefd[0]);
    // sys_close(pipefd[1]);

    welcome_message();
    while (1) {
        fputChar(STDERR, '>');
        wait_command();
    }
}