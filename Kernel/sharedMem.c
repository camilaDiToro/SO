#include <sharedMemory.h>
#include <resourceNamerADT.h>
#include <memoryManager.h>

typedef struct {
    void* memoryPointer;
    int size;
    int linkedProcesses;
} TSharedMem;

static TResourceNamer namer;

int shm_init() {
    namer = rnm_new();
    
    if (namer != 0)
        return 1;
    
    return 0;
}

void* shm_openShm(const char* name, size_t requestedSize, size_t* size){
    TSharedMem* shmMem = (TSharedMem*) rnm_getResource(namer, name);

    if (shmMem != NULL){
        *size = shmMem->size;
        shmMem->linkedProcesses += 1;
        return shmMem->memoryPointer;
    }

    shmMem = mm_malloc(sizeof(TSharedMem));
    if (shmMem == NULL)
        return NULL;

    if ((shmMem->memoryPointer = mm_malloc(requestedSize)) == NULL) {
        mm_free(shmMem);
        return NULL;
    }

    shmMem->size = requestedSize;
    shmMem->linkedProcesses = 1;

    rnm_nameResource(namer, shmMem, name, NULL);

    return shmMem->memoryPointer;
}

int shm_closeShm(const char* name){
    TSharedMem* shmMem = (TSharedMem*)rnm_getResource(namer, name);

    if (shmMem == NULL)
        return 1;

    if (shmMem->linkedProcesses > 1) {
        shmMem->linkedProcesses -= 1;
        return 0;
    }
    
    rnm_unnameResource(namer, name);
    mm_free(shmMem->memoryPointer);
    mm_free(shmMem);
    return 0;
}