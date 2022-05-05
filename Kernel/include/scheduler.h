#include <stdint.h>

void sch_init(void* functionAddress, uint8_t priority, int argc, const char * argv[]);
void* sch_switchProcess(void * currentRSP);
int sch_loadProcess(void * functionAddress, uint8_t priority, int argc, const char * argv[]);
uint8_t sch_getCurrentPID();
int sch_kill(uint8_t pid);
int sch_block(uint8_t pid);
int sch_unblock(uint8_t pid);
void sch_getProcesses();
int sch_nice(uint8_t pid, uint8_t priority);