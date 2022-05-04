#ifndef _MEMORY_MANAGER_H_
#define _MEMORY_MANAGER_H_

/* Standard library */
#include <stddef.h>

/**
 * @brief Initializes the memory manager with a given memory segment
 *
 * @param memoryStart the starting address of the segment of memory to be used by the memory manager
 * @param memorySize the amount of bytes available for the memory manager from initial address
 */
void mm_init(void* memoryStart, size_t memorySize);

/**
 * @brief Request the memory manager to reserve a chunk of memory.
 *
 * @param size the amount of memory requested
 *
 * @return pointer to the reserved memory, or NULL if the operation failed
 */
void* mm_malloc(size_t size);

/**
 * @brief Instructs the memory manager that a chunk of memory previously reserved by mm_malloc()
 *        may now be marked as available and used elsewhere.
 *
 * @param ptr pointer to the memory zone to be released
 *
 * @return 0 if the operation succeeds, 1 otherwise
 */
int mm_free(void* ptr);

/**
 * @brief Request the memory manager to change the size of a reserved chunk of memory.
 *
 * @param ptr pointer to the memory chunk previously reserved to be resized
 *
 * @param size the new size for the memory chunk
 *
 * @return pointer to the reserved memory, or NULL if the operation failed
 */
void* mm_realloc(void* ptr, size_t size);

void mm_printDebug();

#endif