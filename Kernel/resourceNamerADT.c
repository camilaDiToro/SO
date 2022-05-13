/* Standar library */
#include <stddef.h>

/* Local Headers */
#include <resourceNamerADT.h>
#include <memoryManager.h>
#include <string.h>

#define MAX_NAME_LENGTH 16
#define BUFFER_CHUNK_SIZE 8

typedef struct {
    char* name;
    void* resource;
} TNamedResource;

struct TResourceNamerInternal {
    TNamedResource* buffer;
    int count;
    int bufferSize;
};

static int isNameValid(const char* name) {
    if (name == NULL)
        return 0;

    for (int i = 0; i <= MAX_NAME_LENGTH; i++) {
        char c = name[i];
        if (c == '\0') {
            return i > (name[0] == '/' ? 1 : 0);
        }

        // The first character must be a letter or a slash. Subsequent
        // characters may be a letter or a number.
        if ((c < 'a' || c > 'z') && (c < 'A' || c > 'Z')) {
            if (i == 0 ? (c != '/') : (c < '0' || c > '9'))
                return 0;
        }
    }

    return 0;
}

TResourceNamer rnm_new() {
    TResourceNamer namer = mm_malloc(sizeof(struct TResourceNamerInternal));

    if (namer != NULL) {
        namer->buffer = NULL;
        namer->count = 0;
        namer->bufferSize = 0;
    }

    return namer;
}

int rnm_free(TResourceNamer namer) {
    return mm_free(namer->buffer) + mm_free(namer);
}

static int findResourceIndex(TResourceNamer namer, const char* name, int* c) {
    int i;
    *c = 1;
    for (i = 0; i < namer->count && (*c = strcmp(name, namer->buffer[i].name)) < 0; i++);
    return i;
}

int rnm_nameResource(TResourceNamer namer, void* resource, const char* name, const char** nameInternal) {
    if (!isNameValid(name))
        return 1;

    int c;
    int index = findResourceIndex(namer, name, &c);

    // If a resource with that name is already present, return 1.
    if (c == 0)
        return 1;

    char* nameCopy = mm_malloc(strlen(name) + 1);
    if (nameCopy == NULL)
        return -1;

    // Expand the buffer if necessary.
    if (namer->count == namer->bufferSize) {
        size_t newBufferSize = namer->bufferSize + BUFFER_CHUNK_SIZE;
        TNamedResource* newBuffer = mm_realloc(namer->buffer, newBufferSize * sizeof(TNamedResource));
        if (newBuffer == NULL) {
            mm_free(nameCopy);
            return -1;
        }

        namer->buffer = newBuffer;
        namer->bufferSize = newBufferSize;
    }

    for (int i = namer->count; i > index; i--)
        namer->buffer[i] = namer->buffer[i - 1];

    namer->buffer[index].name = nameCopy;
    namer->buffer[index].resource = resource;
    namer->count++;
    strcpy(nameCopy, name);
    if (nameInternal != NULL)
        *nameInternal = nameCopy;
    return 0;
}

int rnm_unnameResource(TResourceNamer namer, const char* name) {
    int c;
    int index = findResourceIndex(namer, name, &c);

    // If there is no such resource with that name, return 1.
    if (c != 0)
        return 1;

    int result = mm_free(namer->buffer[index].name);
    namer->count--;

    for (; index < namer->count; index++)
        namer->buffer[index] = namer->buffer[index + 1];

    return result;
}

void* rnm_getResource(TResourceNamer namer, const char* name) {
    int c;
    int index = findResourceIndex(namer, name, &c);
    return c == 0 ? namer->buffer[index].resource : NULL;
}