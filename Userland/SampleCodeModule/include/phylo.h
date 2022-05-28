#ifndef __PHYLO_H__
#define __PHYLO_H__

#include <kernelTypes.h>

#define MAX_PHYLOSOPHERS 10
#define MIN_PHYLOSOPHERS 5
#define MAX_CHOPSTICKS MAX_PHYLOSOPHERS
#define MIN_CHOPSTICKS MIN_PHYLOSOPHERS

#define CHOPSTICKS_SEM_NAME "semChops"

#define ADD 'a'
#define REMOVE 'r'
#define QUIT 'q'
#define STATE 's'

#define EATING_TIME 5000
#define THINKING_TIME 5000
#define SLEEPING_TIME 5000

typedef enum phyloState {EATING = 0, SLEEPING, THINKING, DEAD} phyloState;

typedef struct phylo {
    char* phyloName;
    phyloState phyloState;
    TPid phyloPid;
} TPhylo;

void initPhyloProcess(int argc, char * argv[]);

#endif