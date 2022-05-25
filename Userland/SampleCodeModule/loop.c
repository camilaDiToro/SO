#include <loop.h>
#include <syscalls.h>
#include <userstdlib.h>

void loopProcess(uint64_t argc, char *argv[]){
    TPid pid = sys_getPid();
    while(1){
        printf("Hi! My PID is: %d \n", pid);
        sleep(3500);
    }
}