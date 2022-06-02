#ifndef _WAIT_QUEUE_ADT_
#define _WAIT_QUEUE_ADT_

/* Local headers */
#include <kernelTypes.h>

typedef struct TWaitQueueInternal* TWaitQueue;

/**
 * @brief Creates a new wait queue instance.
 * 
 * @returns The created wait queue instance, or NULL of the operation failed.
 */
TWaitQueue wq_new();

/**
 * @brief Frees all resources used by a wait queue. Attempting to use a
 * wait queue after it has been freed results in undefined behavior.
 * 
 * @returns 0 if the operation succeeded, !=0 if not.
 */
int wq_free(TWaitQueue queue);

/**
 * @brief Adds a PID to the waiting queue. If the PID was already in the queue,
 * this operation will add a new entry and have the PID multiple times on the queue.
 *  
 * @returns 0 if the operation succeeded, !=0 if not.
 */
int wq_add(TWaitQueue queue, TPid pid);

/**
 * @brief Gets the amount of entries on the queue.
 */
int wq_count(TWaitQueue queue);

/**
 * @brief Gets whether a given PID has an entry in the queue.
 * 
 * @returns 1 if the given PID is contained in the queue, 0 otherwise.
 */
int wq_contains(TWaitQueue queue, TPid pid);

/**
 * @brief Adds a PID to the waiting queue, only if it was not already in it.
 * 
 * @returns 0 if the operation succeeded. The PID already being on the queue is considered success.
 */
int wq_addIfNotExists(TWaitQueue queue, TPid pid);

/**
 * @brief Removes a PID from the waiting queue.
 * 
 * @returns 0 if the operation succeeded. The PID not being on the queue is considered a failure.
 */
int wq_remove(TWaitQueue queue, TPid pid);

/**
 * @brief Unblocks a single process on the queue.
 * 
 * @returns 0 if the operation succeded. The queue being empty is not considered a failure.
 */
int wq_unblockSingle(TWaitQueue queue);

/**
 * @brief Unblocks all the processes on the queue.
 * 
 * @returns 0 if the operation succeded. The queue being empty is not considered a failure.
 */
int wq_unblockAll(TWaitQueue queue);

/**
 * @brief Gets up to maxPids PIDs stored in this queue, in the order in which they were queued.
 * (array[0] will be the next PID to be unblocked).
 * 
 * @returns The amount of PIDs written to the array.
 */
int wq_getPids(TWaitQueue queue, TPid* array, int maxPids);

#endif