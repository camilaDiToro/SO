#ifndef _SEMAPHORE_H_
#define _SEMAPHORE_H_

#include <stddef.h>
#include <lib.h>
#include <memoryManager.h>
#include <resourceNamerADT.h>
#include <waitQueueADT.h>
#include <kernelTypes.h>
#include <scheduler.h>

#define MAX_SEMAPHORES 128
#define SEM_SUCCES 0
#define SEM_FAILED -1
#define SEM_NOTEXISTS -2

typedef int8_t Tlock;

/**
 * @brief Initialize Scheduler
 * 
 * @returns 0 if the operation succeeded, SEM_FAILED otherwise.
 *
 */
int sem_init();

Tsem sem_open(const char * name, uint8_t initialValue);

int sem_close(Tsem sem);

int sem_printDebug();

int sem_wait(Tsem sem);

int sem_post(Tsem sem);

#endif