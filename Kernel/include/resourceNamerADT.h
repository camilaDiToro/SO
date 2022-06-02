#ifndef _RESOURCE_NAMER_ADT_
#define _RESOURCE_NAMER_ADT_

typedef struct TResourceNamerInternal* TResourceNamer;

/**
 * @brief Creates a new resource namer instance.
 * 
 * @returns The created resource namer instance, or NULL if the operation failed.
 */
TResourceNamer rnm_new();

/**
 * @brief Frees all resources used by a resource namer. Attempting to use a
 * resource namer after it has been freed results in undefined behavior.
 * 
 * @returns 0 if the operation succeeded, != 0 if not.
 */
int rnm_free(TResourceNamer namer);

/**
 * @brief Maps a given resource to a given name. The namer does not keep a reference
 * to the string, but rather makes an internal copy. This internal copy is also placed
 * in nameInternal (if not NULL).
 * 
 * @returns 0 if the operation succeeded, 1 if the resource was already present,
 * or -1 if the operation failed for other reasons.
 */
int rnm_nameResource(TResourceNamer namer, void* resource, const char* name, const char** nameInternal);

/**
 * @brief Unmaps a given resource to a given name.
 * 
 * @returns The resource whose name was removed, or NULL if no resource
 * had such name.
 */
void* rnm_unnameResource(TResourceNamer namer, const char* name);

/**
 * @brief Gets a resource by it's name.
 * 
 * @returns The named resource, or NULL if none was found with such name.
 */
void* rnm_getResource(TResourceNamer namer, const char* name);

#endif