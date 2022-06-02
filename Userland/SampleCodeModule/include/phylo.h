#ifndef __PHYLO_H__
#define __PHYLO_H__

/* Local headers */
#include <kernelTypes.h>

/* Constants */
#define PHYLO_PRIORITY DEFAULT_PRIORITY

#define MAX_PHYLOSOPHERS 10
#define MIN_PHYLOSOPHERS 5
#define MAX_CHOPSTICKS MAX_PHYLOSOPHERS
#define MIN_CHOPSTICKS MIN_PHYLOSOPHERS

#define CHOPSTICKS_SEM_NAME "semChops"

#define ADD 'a'
#define REMOVE 'r'
#define QUIT 'q'

#define EATING_TIME_MIN 2000
#define EATING_TIME_MAX 5000
#define THINKING_TIME_MIN 2000
#define THINKING_TIME_MAX 5000
#define SLEEPING_TIME_MIN 2000
#define SLEEPING_TIME_MAX 5000

typedef enum {EATING = 0, WAITINGTOEAT, SLEEPING, THINKING, DEAD} TPhyloState;

typedef struct phylo {
    char* phyloName;
    TPhyloState phyloState;
    TPid phyloPid;
} TPhylo;

/**
 * @brief Initialize phylos program 
 */
void initPhyloProcess(int argc, char * argv[]);

#endif