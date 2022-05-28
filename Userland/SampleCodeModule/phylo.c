// One or more phylosophers are sitting at a round table doing one of three things: eating, thinking, or sleeping.
// The phylosophers sit at a circular table with a big bowl of spaghetti in the center.
// There are the same number of chopsticks on the table as there are phylosophers.
// A phylosopher needs two chopsticks to eat. Whenever a phylosopher is done eating, he will drop his chopsticks and start sleeping.
// When a phylosopher is done sleeping, he will start thinking.
// The simulation will finish when user press exit button. Minwhile, user can add or remove a phylosopher from the table.

/* Local headers */
#include <phylo.h>
#include <syscalls.h>
#include <userstdlib.h>
#include <kernelTypes.h>
#include <string.h>

// phylosophers names just for having some fun
static char* phyloName[MAX_PHYLOSOPHERS] = {"Aristoteles", "Socrates", "Platon", "Pitagoras", "Kant",
                                            "Heraclito", "Sofocles", "Tales de Mileto", "Diogenes",
                                            "Democrito"};

// phylosophers array to store status, its name (for printing reasons), and its pid.
static TPhylo phylos[MAX_PHYLOSOPHERS];
static int currentIdx = 0;

// Semaphores
static TSem chopsticks[MAX_CHOPSTICKS];
static TSem semPickChopsticks;

static void phylosopher(int argc, char* argv[]);
static void waitForKey();

static void phyloThink(int phyloIdx);
static void phyloEat(int phyloIdx);
static void phyloSleep(int phyloIdx);

static void addPhylo();
static void removePhylo();
static void addChopstick();
static void removeChopstick();

static void terminatePhylos();
static void terminateChopsticks();

static void printState();

// Initialize a minimun amount of phylosophers.
void initPhyloProcess(int argc, char* argv[]) {

    printf("A minimun quantity of phylosophers are beign created, please stand by\n");
    printf("Press %c for quiting, %c for adding a new phylosopher\n", QUIT, ADD);
    printf("%c for removing a phylosopher, %c for checking the state of each phylosopher \n", REMOVE, STATE);
    sleep(2000);
    
    // Prepare the table for a minimun amount of phylosophers
    for (int i=0; i < MIN_PHYLOSOPHERS; ++i)
        addPhylo();

    // Initialize semaphore
    if ((semPickChopsticks = sys_openSem("semPickSticks", 1)) == -1) {
        fprint(STDERR, "sys_openSem failed\n");
        terminatePhylos();
        sys_exit();
    }

    waitForKey();
}

// With this function, we manage STDIN
static void waitForKey() {

    while (1) {
        char c = getChar();

        if (c < 0 || c == QUIT) {
            terminatePhylos();
            terminateChopsticks();
            sys_exit();

        } else if (c == ADD) {

            if (currentIdx >= MAX_PHYLOSOPHERS)
                printf("There's no more space in the table! Remove a phylosopher!\n");
            else 
                addPhylo();

        } else if (c == REMOVE) {

            if (currentIdx <= MIN_PHYLOSOPHERS)
                printf("Dont be selfish! Invite more phylosophers to the table!\n");
            else 
                removePhylo();

        } else if (c == STATE) 
            printState();
    }
}

// TO DO: adding all and removing them instantly produces a deadlock.
// TO DO: what if the last created phylo is eating and a new one is added, the chopstick id will be inconsistent.
// TO DO: avoid adding and removing instantly the same phylo 
static void phylosopher(int argc, char* argv[]) {

    int phyloIdx = atoi(argv[0]);

    while (1) {
        phyloThink(phyloIdx);
        phyloEat(phyloIdx);
        phyloSleep(phyloIdx);   
    }
}

static void phyloThink(int phyloIdx) {
    printf("%s is thinking ....\n", phylos[phyloIdx].phyloName);
    phylos[phyloIdx].phyloState = THINKING;
    sleep(THINKING_TIME);
}

static void phyloEat(int phyloIdx) {
    sys_waitSem(semPickChopsticks);
    sys_waitSem(chopsticks[phyloIdx]);                    // Left chopstick
    sys_waitSem(chopsticks[(phyloIdx + 1) % currentIdx]); // Right chopstick
    sys_postSem(semPickChopsticks);

    printf("%s is eating ....\n", phylos[phyloIdx].phyloName);
    phylos[phyloIdx].phyloState = EATING;
    sleep(EATING_TIME);

    sys_postSem(chopsticks[(phyloIdx + 1) % currentIdx]); // Right chopstick
    sys_postSem(chopsticks[phyloIdx]);                    // Left chopstick
}

static void phyloSleep(int phyloIdx) {
    printf("%s is sleeping .... zzzz\n", phylos[phyloIdx].phyloName);
    phylos[phyloIdx].phyloState = SLEEPING;
    sleep(SLEEPING_TIME);
}

static void addPhylo() {

    addChopstick();
    phylos[currentIdx].phyloName = phyloName[currentIdx];
    phylos[currentIdx].phyloState = THINKING;

    char idxToString[] = {currentIdx + '0', 0};
    char* auxi[] = {idxToString};
    TProcessCreateInfo phyloContexInfo = {
        .name = phyloName[currentIdx],
        .entryPoint = (void*)phylosopher,
        .isForeground = 0,
        .priority = MAX_PRIORITY,
        .argc = 1,
        .argv = (const char* const*) auxi,
    };

    phylos[currentIdx].phyloPid = sys_createProcess(-1, -1, -1, &phyloContexInfo);
    currentIdx++;
}

static void removePhylo() {
    phylos[currentIdx].phyloState = DEAD;
    sys_killProcess(phylos[currentIdx].phyloPid);
    removeChopstick();
}

static void addChopstick() {
    char aux[] = {currentIdx + '0', 0};
    char length = strlen(CHOPSTICKS_SEM_NAME);
    char* semName = sys_malloc(sizeof(char) * (length + 2));
    strcpy(semName, CHOPSTICKS_SEM_NAME);
    strcat(semName, aux);
    chopsticks[currentIdx] = sys_openSem(semName, 1);
    if (chopsticks[currentIdx] < 0) {
        fprint(STDERR, "sys_openSem failed\n");
        sys_free(semName);
        terminatePhylos();
        terminateChopsticks();
        sys_exit();
    }

    sys_free(semName);
}

static void removeChopstick() {
    sys_closeSem(chopsticks[currentIdx]);
    currentIdx--;
}

static void terminatePhylos(){
    for(int i = 0; i < currentIdx; ++i)
        sys_killProcess(phylos[i].phyloPid);
}

static void terminateChopsticks() {
    for(int i = 0; i < currentIdx; ++i)
        sys_closeSem(chopsticks[i]);
    
    sys_closeSem(semPickChopsticks);
}

static void printState() {
    for (int i = 0; i < currentIdx; ++i) {
        printf(" %s: %s\n", phylos[i].phyloName, phylos[i].phyloState == EATING ? "Eating" :
                                                 phylos[i].phyloState == SLEEPING ? "Sleeping" :
                                                 phylos[i].phyloState == THINKING ? "Thinking" : "Dead" );
    }
}