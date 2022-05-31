// This is a personal academic project. Dear PVS-Studio, please check it.
// PVS-Studio Static Code Analyzer for C, C++ and C#: http://www.viva64.com

/* Local headers */
#include <kernelTypes.h>
#include <syscalls.h>
#include <test.h>
#include <userstdlib.h>
#include <commands.h>

#define pipeName "testingPipe"

void readAndClosePipe() {
    TPid p = sys_getPid();
    printf("Reading process with pid %d created \n", p);

    printf("Process %d opening pipe with name %s \n", p, pipeName);
    int pipeFd[2];

    if (sys_openPipe(pipeName, pipeFd) < 0) {
        printf("Error while trying to open pipe");
        return;
    }

    if (sys_close(pipeFd[1]) < 0) {
        printf("Error while trying to close pipe fd");
        return;
    }

    printf("Unlinking pipe...\n");
    sys_unlinkPipe(pipeName);
    TPid createdPid;
    runPipe(STDIN, STDOUT, STDERR, 1, 0, NULL, &createdPid);
    print("\n");

    char buffer[101];
    ssize_t count = sys_read(pipeFd[0], buffer, 100);
    if (count < 0) {
        printf("Error while trying to read from pipe");
        return;
    }
    buffer[count] = '\0';

    printf("Process %d read from pipe \"%s\" the content \"%s\" \n", p, pipeName, buffer);
}

void openAndWritePipe(void) {
    TPid p = sys_getPid();
    printf("Writing process with pid %d created \n", p);

    printf("Process %d opening pipe with name \"%s\" \n", p, pipeName);
    int pipeFd[2];

    if (sys_openPipe(pipeName, pipeFd) < 0) {
        printf("Error while trying to open pipe");
        return;
    }
    printf("Process %d writing in pipe with name \"%s\" \n", p, pipeName);

    if (sys_close(pipeFd[0]) < 0) {
        printf("Error while trying to close pipe");
        return;
    }

    fprint(pipeFd[1], "This message comes from a dead process");

    if (sys_close(pipeFd[1]) < 0) {
        printf("Error while trying to close pipe fd");
        return;
    }
}

void namedPipesProcess(int argc, char* argv[]) {
    TPid p = sys_getPid();
    printf("Main process with pid %d created \n", p);

    TProcessCreateInfo pci = {
        .name = "writeProcess",
        .isForeground = 1,
        .priority = DEFAULT_PRIORITY,
        .entryPoint = (TProcessEntryPoint)openAndWritePipe,
        .argc = 0,
        .argv = NULL};

    printf("Process %d creating the process that will open and write the pipe \n", p);
    TPid wpid = sys_createProcess(-1, -1, -1, &pci);
    if (wpid < 0) {
        printf("Error while trying to create the writing process");
        return;
    }

    sys_waitpid(wpid);
    printf("Process %d finished \n", wpid);

    TPid createdPid;
    runPipe(STDIN, STDOUT, STDERR, 1, 0, NULL, &createdPid);

    TProcessCreateInfo pci2 = {
        .name = "readingProcess",
        .isForeground = 1,
        .priority = DEFAULT_PRIORITY,
        .entryPoint = (TProcessEntryPoint)readAndClosePipe,
        .argc = 0,
        .argv = NULL};

    printf("\nProcess %d creating the process that will read and close the pipe \n", p);
    TPid rpid = sys_createProcess(-1, -1, -1, &pci2);
    if (rpid < 0) {
        printf("Error while trying to create the reading process");
        return;
    }

    sys_waitpid(rpid);
    printf("Process %d finished.\n", rpid);
    runPipe(STDIN, STDOUT, STDERR, 1, 0, NULL, &createdPid);

}