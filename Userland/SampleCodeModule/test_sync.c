#include "kernelTypes.h"
#include "syscalls.h"
#include "test_util.h"
#include <stdint.h>
#include <stdio.h>

#define SEM_ID "sem"
#define TOTAL_PAIR_PROCESSES 2

int64_t global; // shared memory

void slowInc(int64_t* p, int64_t inc) {
    uint64_t aux = *p;
    sys_yield(); // This makes the race condition highly probable
    aux += inc;
    *p = aux;
}

void my_process_inc(int argc, char* argv[]) {
    uint64_t n;
    int8_t inc;
    int8_t use_sem;

    if (argc != 3)
        return;

    if ((n = satoi(argv[0])) <= 0)
        return;
    if ((inc = satoi(argv[1])) == 0)
        return;
    if ((use_sem = satoi(argv[2])) < 0)
        return;

    TSem sem;

    if (use_sem) {
        if ((sem = sys_openSem(SEM_ID, 1)) < 0) {
            printf("test_sync: ERROR opening semaphore\n");
            return;
        }
    }

    uint64_t i;
    for (i = 0; i < n; i++) {
        if (use_sem)
            sys_waitSem(sem);
        slowInc(&global, inc);
        if (use_sem)
            sys_postSem(sem);
    }

    if (use_sem)
        sys_closeSem(sem);
}

void test_sync(int argc, char* argv[]) { //{n, use_sem, 0}
    uint64_t pids[2 * TOTAL_PAIR_PROCESSES];

    if (argc != 2)
        return;

    char* argvDec[] = {argv[0], "-1", argv[1], NULL};
    char* argvInc[] = {argv[0], "1", argv[1], NULL};

    TProcessCreateInfo pciDec = {
        .name = "processDec",
        .isForeground = 1,
        .priority = DEFAULT_PRIORITY,
        .entryPoint = (TProcessEntryPoint)my_process_inc,
        .argc = 3,
        .argv = (const char* const*)argvDec};

    TProcessCreateInfo pciInc = {
        .name = "processInc",
        .isForeground = 1,
        .priority = DEFAULT_PRIORITY,
        .entryPoint = (TProcessEntryPoint)my_process_inc,
        .argc = 3,
        .argv = (const char* const*)argvInc};

    global = 0;

    uint64_t i;
    for (i = 0; i < TOTAL_PAIR_PROCESSES; i++) {
        pids[i] = sys_createProcess(-1, -1, -1, &pciDec);
        pids[i + TOTAL_PAIR_PROCESSES] = sys_createProcess(-1, -1, -1, &pciInc);
    }

    for (i = 0; i < TOTAL_PAIR_PROCESSES; i++) {
        sys_waitpid(pids[i]);
        sys_waitpid(pids[i + TOTAL_PAIR_PROCESSES]);
    }

    printf("Final value: %d\n", global);
}
