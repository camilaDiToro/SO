#include <philosophers.h>
#include <kernelTypes.h>
#include <syscalls.h>
#include <userstdlib.h>

#define INITIAL_PHILOSOPHERS 4
#define MAX_PHILOSOPHERS 8
#define CHOPS 5

typedef enum {THINKING = 0, WAITING, EATING} TPhiloState;

TSem chopSems[MAX_PHILOSOPHERS];
TPhiloState philos[MAX_PHILOSOPHERS];

void philosopherLifecycle(int argc, char * argv[]){
    sleep(5);
    printf("%s %s", argv[0],argv[1]);
}

void phylo(void){

    char chopSemName[16] = "chop";
    for(int i=0 ; i < CHOPS ; ++i){
        chopSemName[5] = '0' + i;
        chopSems[i] = sys_openSem(chopSemName, 1);
    }

    TProcessCreateInfo pci = {
        .isForeground = 1,
        .priority = DEFAULT_PRIORITY,
        .entryPoint = philosopherLifecycle,
        .argc = 2,
    };

    char name[10] = "philo";
    name[6] = 0;
    
    char sem1[10] = "chop";
    sem1[5] = 0;

    char sem2[10] = "chop";
    sem2[5] = 0;
    
    char* argv[] = {"", ""};

    for(int i=0 ; i<INITIAL_PHILOSOPHERS ; ++i){
        name[5] = '0' + i;
        pci.name = name;
        sem1[4] = '0' + i;
        argv[0] = sem1;
        sem2[4] = '0' + ((i+1)%INITIAL_PHILOSOPHERS);
        argv[1] = sem2;
        pci.argv = argv;
        sys_createProcess(-1,-1,-1,&pci);
    }
}


