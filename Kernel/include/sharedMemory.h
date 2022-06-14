#ifndef _SHARED_MEMORY_H_
#define _SHARED_MEMORY_H_

#include <stddef.h>

int shm_init();

/**
 * @brief Opens the shared memory.
 * 
 */
void* shm_openShm(const char* name, size_t requestedSize, size_t* size);

/**
 * @brief Close the shared memory.
 * 
 * @returns 0 if the operation succeeded, !=0 if not.
 */
int shm_closeShm(const char* name);

#endif