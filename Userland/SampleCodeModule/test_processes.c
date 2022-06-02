// This is a personal academic project. Dear PVS-Studio, please check it.
// PVS-Studio Static Code Analyzer for C, C++ and C#: http://www.viva64.com

/* Local headers */
#include <test.h>
#include <test_util.h>
#include <syscalls.h>
#include <userstdlib.h>

/* Constants */
#define MAX_PROCESSES 10

enum State { RUNNING_TEST,
             BLOCKED_TEST,
             KILLED_TEST };

typedef struct P_rq {
    int32_t pid;
    enum State state;
} p_rq;

void test_processes(int argc, char* argv[]) {
    uint8_t rq;
    uint8_t alive = 0;
    uint8_t action;
    uint64_t max_processes = MAX_PROCESSES;
    char* argvAux[] = {0};

    p_rq p_rqs[max_processes];

    while (1) {

        TProcessCreateInfo pci = {
            .name = "loopProcess",
            .isForeground = 0,
            .priority = DEFAULT_PRIORITY,
            .entryPoint = (TProcessEntryPoint)endless_loop,
            .argc = 0,
            .argv = (const char* const*)argvAux};

        // Create max_processes processes
        for (rq = 0; rq < max_processes; rq++) {
            p_rqs[rq].pid = sys_createProcess(-1, -1, -1, &pci);
            if (p_rqs[rq].pid == -1) {
                printf("test_processes: ERROR creating process\n");
                return;
            } else {
                p_rqs[rq].state = RUNNING_TEST;
                alive++;
            }
        }

        // Randomly kills, blocks or unblocks processes until every one has been killed
        while (alive > 0) {

            for (rq = 0; rq < max_processes; rq++) {
                action = GetUniform(100) % 2;

                switch (action) {
                    case 0:
                        if (p_rqs[rq].state == RUNNING_TEST || p_rqs[rq].state == BLOCKED_TEST) {
                            if (sys_killProcess(p_rqs[rq].pid) != 0) {
                                printf("test_processes: ERROR killing process\n");
                                return;
                            }
                            p_rqs[rq].state = KILLED_TEST;
                            alive--;
                        }
                        break;

                    case 1:
                        if (p_rqs[rq].state == RUNNING_TEST) {
                            if (sys_block(p_rqs[rq].pid) != 0) {
                                printf("test_processes: ERROR blocking process\n");
                                return;
                            }
                            p_rqs[rq].state = BLOCKED_TEST;
                        }
                        break;
                }
            }

            // Randomly unblocks processes
            for (rq = 0; rq < max_processes; rq++)
                if (p_rqs[rq].state == BLOCKED_TEST && GetUniform(100) % 2) {
                    if (sys_unblock(p_rqs[rq].pid) != 0) {
                        printf("test_processes: ERROR unblocking process\n");
                        return;
                    }
                    p_rqs[rq].state = RUNNING_TEST;
                }
        }
    }
}
