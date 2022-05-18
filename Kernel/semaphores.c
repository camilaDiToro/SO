#include <semaphores.h>
#include <graphics.h>

// A list might be implemented to track de asociated processes to a semaphore
// Now just are being counted the number of processes, but this could lead to an inconsistency if,
// for example, a process tries to close the same semaphore twice
typedef struct {
    uint8_t value;
    TLock lock;
    uint8_t linkedProcesses; 
    const char* name;
    TWaitQueue waitingProcesses;  //Tracks the processes waiting for a post in this semaphore
} TSemaphore;

TSemaphore * semaphores[MAX_SEMAPHORES] = {NULL};
TResourceNamer namer;
TLock generalLock;


extern int _spin_lock(TLock * lock);
static int sem_free(TSem sem);
static int isValidSemId(TSem sem);
static int adquireSem(TSem sem);


static int sem_free(TSem sem) {
    int value = rnm_unnameResource(namer, semaphores[sem]->name); 
    value += wq_free(semaphores[sem]->waitingProcesses);
    value += mm_free(semaphores[sem]);
    semaphores[sem] = NULL;
    if(value!=0)
        return SEM_FAILED;
    return SEM_SUCCES;
}

static int isValidSemId(TSem sem) {
    return sem>=0 && sem<MAX_SEMAPHORES && semaphores[sem]!=NULL;
}

static int adquireSem(TSem sem){
    _spin_lock(&generalLock);
    
    if(!isValidSemId(sem)){
        generalLock = 0;
        return SEM_NOTEXISTS;
    }

    _spin_lock(&(semaphores[sem]->lock));
    generalLock = 0;
    return SEM_SUCCES;
}

int sem_printDebug() {

    for(int sem = 0 ; sem < MAX_SEMAPHORES ; ++sem) {
        if(semaphores[sem]!=NULL){
            scr_print("Id: ");
            scr_printDec(sem);
            scr_print(", \t Value: ");
            scr_printDec(semaphores[sem]->value);
            scr_print(", \t Linked processes: ");
            scr_printDec(semaphores[sem]->linkedProcesses);
            scr_print(", \t Name: ");
            scr_print(semaphores[sem]->name);
            scr_printChar('\n');
        }
    }
    return SEM_SUCCES;
}

int sem_init() {
    namer = rnm_new();
    generalLock = 0;
    if(namer != 0)
        return SEM_FAILED;
    return 0;
}

TSem sem_open(const char * name, uint8_t initialValue) {

    _spin_lock(&generalLock);

    TSem sem = (TSem)(int64_t)rnm_getResource(namer, name);

    if(sem!=0){
        semaphores[sem]->linkedProcesses+=1;
        generalLock = 0;
        return sem;
    }
    
    int i;
    for( i=1 ; i < MAX_SEMAPHORES && semaphores[i]; ++i );

    if(i == MAX_SEMAPHORES) {
        generalLock = 0;
        return SEM_FAILED;
    }        
    
    semaphores[i] = mm_malloc(sizeof(TSemaphore));
    if(semaphores[i] == NULL){
        generalLock = 0;
        return SEM_FAILED;
    }
    semaphores[i]->value = initialValue;
    semaphores[i]->lock = 0;
    semaphores[i]->linkedProcesses = 1;
    semaphores[i]->waitingProcesses = wq_new();

    if(semaphores[i]->waitingProcesses == NULL){
        mm_free(semaphores[i]);
        generalLock = 0;
        return SEM_FAILED;
    }

    if(rnm_nameResource(namer,(void *)(int64_t)i, name, &(semaphores[i]->name))!=0){
        //check what should be freed
        generalLock = 0;
        return SEM_FAILED;
    }

    generalLock = 0;
    return (TSem) i;
}

int sem_close(TSem sem) {

    if(adquireSem(sem)== SEM_NOTEXISTS){
        return SEM_NOTEXISTS;
    }

    if(semaphores[sem]->linkedProcesses == 1){
        return sem_free(sem); //The semaphore is destroyed, so we do not care about its lock
    }                       
    
    semaphores[sem]->linkedProcesses-= 1;
    semaphores[sem]->lock = 0;
    return SEM_SUCCES;
}

int sem_post(TSem sem) {

    if(adquireSem(sem)== SEM_NOTEXISTS){
        return SEM_NOTEXISTS;
    }
    
    semaphores[sem]->value++;
    wq_unblockSingle(semaphores[sem]->waitingProcesses);

    semaphores[sem]->lock = 0;
    return SEM_SUCCES;
}

int sem_wait(TSem sem) {

    if(adquireSem(sem)== SEM_NOTEXISTS){
        return SEM_NOTEXISTS;
    }           
    
    if(semaphores[sem]->value > 0){
        semaphores[sem]->value--;
        semaphores[sem]->lock = 0;
        return SEM_SUCCES;
    }

    TPid cpid = sch_getCurrentPID();

    while (semaphores[sem]->value == 0) {
        wq_add(semaphores[sem]->waitingProcesses, cpid);
        sch_blockProcess(cpid);
        semaphores[sem]->lock = 0;
        sch_yieldProcess();
        _spin_lock(&(semaphores[sem]->lock));
    }

    semaphores[sem]->value--;
    semaphores[sem]->lock = 0;
    return SEM_SUCCES;
}