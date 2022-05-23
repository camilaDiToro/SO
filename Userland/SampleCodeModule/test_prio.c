#include <userstdlib.h>
#include <test_util.h>
#include <kernelTypes.h>
#include <syscalls.h>

#define MINOR_WAIT 1000000 // TODO: Change this value to prevent a process from flooding the screen
#define WAIT      1000000000 // TODO: Change this value to make the wait long enough to see theese processes beeing run at least twice

#define TOTAL_PROCESSES 3
#define LOWEST MIN_PRIORITY  //TODO: Change as required
#define MEDIUM 0 //TODO: Change as required
#define HIGHEST MAX_PRIORITY //TODO: Change as required

int64_t prio[TOTAL_PROCESSES] = {LOWEST, MEDIUM, HIGHEST};

void test_prio(){
  int64_t pids[TOTAL_PROCESSES];
  char *argv[] = {0};
  uint64_t i;

  TProcessCreateInfo pci = {
        .name = "endless",
        .isForeground = 1,
        .priority = DEFAULT_PRIORITY,
        .entryPoint = (TProcessEntryPoint)endless_loop_print,
        .argc = 0,
        .argv = (const char* const*)argv
    };
    

  for(i = 0; i < TOTAL_PROCESSES; i++)
    pids[i] = sys_createProcess(-1,-1,-1, &pci);

  bussy_wait(WAIT);
  printf("\nCHANGING PRIORITIES...\n");

  for(i = 0; i < TOTAL_PROCESSES; i++)
    sys_nice(pids[i], prio[i]);

  bussy_wait(WAIT);
  printf("\nBLOCKING...\n");

  for(i = 0; i < TOTAL_PROCESSES; i++)
    sys_block(pids[i]);

  printf("CHANGING PRIORITIES WHILE BLOCKED...\n");

  for(i = 0; i < TOTAL_PROCESSES; i++)
    sys_nice(pids[i], MEDIUM);

  printf("UNBLOCKING...\n");

  for(i = 0; i < TOTAL_PROCESSES; i++)
    sys_unblock(pids[i]);

  bussy_wait(WAIT);
  printf("\nKILLING...\n");

  for(i = 0; i < TOTAL_PROCESSES; i++)
    sys_killProcess(pids[i]);
}
