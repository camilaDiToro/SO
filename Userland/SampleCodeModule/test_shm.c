#include <test.h>
#include <kernelTypes.h>
#include <syscalls.h>
#include <string.h>
#include <userstdlib.h>

#define SHM_NAME "pedro"

static void memoryWriterProcess(int argc, char* argv[]) {
    size_t size;
    void* shm = sys_openShm(SHM_NAME, 1024, &size);
    if (shm == NULL) {
        fprint(STDERR, "Error: failed to open shared memory!");
        return;
    }

    strcpy(shm, "Hola a todos! Soy un string en shared memory :)");
    sys_closeShm(SHM_NAME);

    print("Termine de escribir en shared memory! Me voy :)");
}

void sharedMemoryProcess(int argc, char* argv[]) {
    size_t size;
    void* shm = sys_openShm(SHM_NAME, 1024, &size);
    if (shm == NULL) {
        fprint(STDERR, "Error: failed to open shared memory!");
        return;
    }

    TProcessCreateInfo pci = {
        .name = "shmWriter",
        .entryPoint = memoryWriterProcess,
        .isForeground = 1,
        .priority = DEFAULT_PRIORITY,
        .argc = 0,
        .argv = NULL
    };

    TPid writerPid = sys_createProcess(-1, -1, -1, &pci);
    if (writerPid < 0) {
        fprint(STDERR, "Error: Failed to create writer process! Aborting.");
        return;
    }

    sys_waitpid(writerPid);

    printf("String from shared memory: %s.\n", shm);
    
    sys_closeShm(SHM_NAME);
}