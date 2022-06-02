// This is a personal academic project. Dear PVS-Studio, please check it.
// PVS-Studio Static Code Analyzer for C, C++ and C#: http://www.viva64.com

/* Standard library */
#include <stdint.h>
#include <stddef.h>
#include <sys/types.h>

/* Local headers */
#include <pipe.h>
#include <memoryManager.h>
#include <process.h>
#include <lib.h>
#include <scheduler.h>
#include <waitQueueADT.h>
#include <resourceNamerADT.h>
#include <string.h>

/** The maximum amount of pipes the system can have at a given moment. */
#define MAX_PIPES 64

/** The minimum size for a non-null pipe buffer. */
#define PIPE_MIN_BUFFER_SIZE 512

/** The maximum size for a pipe buffer. */
#define PIPE_MAX_BUFFER_SIZE 4096

/**
 * @brief Defines a size rounding operation for pipe buffer size values.
 * The returned value must be equal or greater than the input.
 */
#define PIPE_ROUND_BUFFER_SIZE(value) (((value) + 511) / 512 * 512)

/**
 * @brief Represents a structure that holds all of a pipe's fields.
 */
typedef struct {
    /**
     * A pointer to the mm_malloc() allocated buffer that backs this pipe's storage, or null
     * if none was allocated yet.
     */
    void* buffer;

    /**
     * The total size of the buffer that backs this pipe's storage.
     */
    size_t bufferSize;

    /**
     * The position in the buffer at which the first byte availabe for reading is located.
     */
    size_t readOffset;

    /**
     * The amount of bytes that are currently stored in this pipe's buffer.
     */
    size_t remainingBytes;

    /**
     * The amount of currently open read/write file descriptors for this
     * pipe on all processes combined.
     */
    unsigned int readerFdCount, writerFdCount;

    /**
     * Queues the processes that are waiting to be able to read/write to this pipe.
     */
    TWaitQueue readProcessWaitQueue, writeProcessWaitQueue;

    /**
     * This pipe's name, or NULL if this pipe is unnamed/unlinked.
     */
    const char* name;
} TPipeInternal;

/**
 * @brief Represents the structure passed in as "resource" whenever a pipe is
 * mapped to a file descriptor on a process.
 */
typedef struct {
    TPipe pipe;
    int allowRead, allowWrite;
} TPipeFdMapping;

/** Holds all the pipes. Each slot may have a pipe or be NULL. */
static TPipeInternal* pipes[MAX_PIPES];
/** Used to quickly find available slots on the "pipes" array. */
static int nextPipeIdCandidate = 0;

/** Tracks the name of all named pipes. */
static TResourceNamer namedPipes = NULL;

static ssize_t fdReadHandler(TPid pid, int fd, void* resource, char* buf, size_t count);
static ssize_t fdWriteHandler(TPid pid, int fd, void* resource, const char* buf, size_t count);
static int fdCloseHandler(TPid pid, int fd, void* resource);
static int fdDupHandler(TPid pidFrom, TPid pidTo, int fdFrom, int fdTo, void* resource);

static TPipeInternal* tryGetPipeInternal(TPipe pipe) {
    return (pipe < 0 || pipe >= MAX_PIPES) ? NULL : pipes[pipe];
}

TPipe pipe_create() {
    // Find an ID for the pipe, or -1 if no IDs are available.
    int id = -1;
    for (int i = 0; i < MAX_PIPES; i++) {
        int idCandidate = (nextPipeIdCandidate + i) % MAX_PIPES;
        if (pipes[idCandidate] == NULL) {
            id = idCandidate;
            break;
        }
    }

    if (id == -1)
        return -1;

    // Allocate memory and create the internal objects the pipe needs.
    TPipeInternal* pipeInternal;
    TWaitQueue readQueue = NULL, writeQueue = NULL;
    if ((pipeInternal = mm_malloc(sizeof(TPipeInternal))) == NULL || (readQueue = wq_new()) == NULL || (writeQueue = wq_new()) == NULL) {
        mm_free(pipeInternal);
        if (readQueue != NULL)
            wq_free(readQueue);
        return -1;
    }

    // Initialize the pipe as empty, don't even allocate a buffer until it's necessary.
    memset(pipeInternal, 0, sizeof(TPipeInternal));
    pipeInternal->readProcessWaitQueue = readQueue;
    pipeInternal->writeProcessWaitQueue = writeQueue;
    pipes[id] = pipeInternal;

    nextPipeIdCandidate = (id + 1) % MAX_PIPES;
    return id;
}

TPipe pipe_open(const char* name) {
    // Create the resource namer if it's null.
    if (namedPipes == NULL && (namedPipes = rnm_new()) == NULL)
        return -1;

    // Get the pipe from the named resource tracker.
    // The tracker stores the TPipe plus one rather than just the TPipe,
    // since a TPipe may be 0 and the resource namer uses NULL to indicate
    // "no resource".
    TPipe pipe = (TPipe)(size_t)rnm_getResource(namedPipes, name) - 1;

    if (pipe < 0) {
        // If theres no pipe with such name, create it and name it.
        if ((pipe = pipe_create()) < 0)
            return -1;

        if (rnm_nameResource(namedPipes, (void*)(size_t)(pipe + 1), name, &pipes[pipe]->name) != 0) {
            pipe_free(pipe);
            return -1;
        }
    }

    return pipe;
}

int pipe_unlink(const char* name) {
    // Unname and get the pipe to unlink.
    TPipe pipe = (TPipe)(size_t)rnm_unnameResource(namedPipes, name) - 1;
    if (pipe < 0)
        return 1;

    TPipeInternal* pipeInternal = pipes[pipe];
    pipeInternal->name = NULL;

    // If the pipe has no more readers or writers, it should be freed.
    if (pipeInternal->readerFdCount == 0) {
        if (pipeInternal->writerFdCount == 0) {
            // No more readers nor writers, fully dispose of the pipe.
            return pipe_free(pipe);
        }

        // No more readers, we don't need to keep any data inside the pipe. Free the internal buffer.
        int result = mm_free(pipeInternal->buffer);
        pipeInternal->buffer = NULL;
        pipeInternal->bufferSize = 0;
        wq_unblockAll(pipeInternal->writeProcessWaitQueue);
        return result;
    } else if (pipeInternal->writerFdCount == 0)
        wq_unblockAll(pipeInternal->readProcessWaitQueue);

    return 0;
}

int pipe_free(TPipe pipe) {
    TPipeInternal* pipeInternal = tryGetPipeInternal(pipe);
    if (pipeInternal == NULL)
        return 1;

    pipes[pipe] = NULL;
    return mm_free(pipeInternal->buffer) + wq_free(pipeInternal->readProcessWaitQueue) + wq_free(pipeInternal->writeProcessWaitQueue) + mm_free(pipeInternal);
}

static ssize_t pipeWriteInternal(TPipeInternal* pipe, const void* buf, size_t count) {
    size_t requiredBufferSize = pipe->remainingBytes + count;
    if (pipe->bufferSize < requiredBufferSize && pipe->bufferSize < PIPE_MAX_BUFFER_SIZE) {
        // Expand this pipe's buffer to a size of min(count, PIPE_MAX_BUFFER_SIZE) rounded up.
        size_t newBufferSize = requiredBufferSize < PIPE_MAX_BUFFER_SIZE ? requiredBufferSize : PIPE_MAX_BUFFER_SIZE;
        newBufferSize = PIPE_ROUND_BUFFER_SIZE(newBufferSize);
        void* newBuf = mm_malloc(newBufferSize);
        if (newBuf != NULL) {
            size_t x = pipe->bufferSize - pipe->readOffset;
            if (x >= pipe->remainingBytes) {
                memcpy(newBuf, pipe->buffer + pipe->readOffset, pipe->remainingBytes);
            } else {
                memcpy(newBuf, pipe->buffer + pipe->readOffset, x);
                memcpy(newBuf + x, pipe->buffer, pipe->remainingBytes - x);
            }
            pipe->readOffset = 0;
            mm_free(pipe->buffer);
            pipe->buffer = newBuf;
            pipe->bufferSize = newBufferSize;
        } else if (pipe->buffer == NULL)
            return -1; // Failed to allocate a buffer for the pipe.
    } else if (pipe->remainingBytes == 0)
        pipe->readOffset = 0;

    // Calculate the position of the write needle and the amount of free space in the pipe.
    size_t writeOffset = (pipe->readOffset + pipe->remainingBytes) % pipe->bufferSize;
    size_t spaceAvailable = pipe->bufferSize - pipe->remainingBytes;

    // Calculate the amount of bytes to write as min(spaceAvailable, count).
    size_t bytesToWrite = count < spaceAvailable ? count : spaceAvailable;

    if (bytesToWrite == 0)
        return 0;

    // A pipe's buffer is treated as circular, so the write may be split into two memcpy()s.
    size_t firstWriteSize = pipe->bufferSize - writeOffset;
    if (firstWriteSize > bytesToWrite)
        firstWriteSize = bytesToWrite;
    memcpy(pipe->buffer + writeOffset, buf, firstWriteSize);

    if (firstWriteSize < bytesToWrite)
        memcpy(pipe->buffer, buf + firstWriteSize, bytesToWrite - firstWriteSize);

    pipe->remainingBytes += bytesToWrite;

    wq_unblockAll(pipe->readProcessWaitQueue);
    return bytesToWrite;
}

ssize_t pipeReadInternal(TPipeInternal* pipe, void* buf, size_t count) {
    // Calculate the amount of bytes to read as min(remainingBytes, count).
    size_t bytesToRead = pipe->remainingBytes;
    if (bytesToRead > count)
        bytesToRead = count;

    if (bytesToRead == 0)
        return 0;

    // A pipe's buffer is treated as circular, so the read may be split into two memcpy()s.
    size_t firstReadSize = pipe->bufferSize - pipe->readOffset;
    if (firstReadSize > bytesToRead)
        firstReadSize = bytesToRead;
    memcpy(buf, pipe->buffer + pipe->readOffset, firstReadSize);

    if (firstReadSize < bytesToRead)
        memcpy(buf + firstReadSize, pipe->buffer, bytesToRead - firstReadSize);

    pipe->remainingBytes -= bytesToRead;
    pipe->readOffset = (pipe->readOffset + bytesToRead) % pipe->bufferSize;

    wq_unblockAll(pipe->writeProcessWaitQueue);

    // If no bytes remain for reading and there are no writers, then "end of file" was reached.
    // We can free the internal pipe's buffer to save memory.
    if (pipe->buffer != NULL && pipe->writerFdCount == 0 && pipe->remainingBytes == 0 && pipe->name == NULL) {
        mm_free(pipe->buffer);
        pipe->buffer = NULL;
        pipe->bufferSize = 0;
        wq_unblockAll(pipe->readProcessWaitQueue);
    }

    return bytesToRead;
}

ssize_t pipe_read(TPipe pipe, void* buf, size_t count) {
    TPipeInternal* pipeInternal = tryGetPipeInternal(pipe);
    return pipeInternal == NULL ? -1 : pipeReadInternal(pipeInternal, buf, count);
}

ssize_t pipe_write(TPipe pipe, const void* buf, size_t count) {
    TPipeInternal* pipeInternal = tryGetPipeInternal(pipe);
    return pipeInternal == NULL ? -1 : pipeWriteInternal(pipeInternal, buf, count);
}

int pipe_mapToProcessFd(TPid pid, int fd, TPipe pipe, int allowRead, int allowWrite) {
    TPipeInternal* pipeInternal = tryGetPipeInternal(pipe);
    if (pipeInternal == NULL)
        return -1;

    TPipeFdMapping* mapping = mm_malloc(sizeof(TPipeFdMapping));
    if (mapping == NULL)
        return -1;

    int r = prc_mapFd(pid, fd, mapping, allowRead ? &fdReadHandler : NULL, allowWrite ? &fdWriteHandler : NULL, &fdCloseHandler, &fdDupHandler);
    if (r < 0) {
        mm_free(mapping);
        return r;
    }

    // Ensure these are a value of either 0 or 1.
    allowRead = (allowRead != 0);
    allowWrite = (allowWrite != 0);

    // Fill in the mapping info to the struct.
    mapping->pipe = pipe;
    mapping->allowRead = allowRead;
    mapping->allowWrite = allowWrite;

    // Resource tracking.
    pipeInternal->readerFdCount += allowRead;
    pipeInternal->writerFdCount += allowWrite;

    return r;
}

int pipe_listPipes(TPipeInfo* array, int maxPipes) {
    int pipeCounter = 0;
    for (int i = 0; i < MAX_PIPES && pipeCounter < maxPipes; i++) {
        TPipeInternal* pipe = pipes[i];
        if (pipe != NULL) {
            TPipeInfo* info = &array[pipeCounter++];
            info->remainingBytes = pipe->remainingBytes;
            info->readerFdCount = pipe->readerFdCount;
            info->writerFdCount = pipe->writerFdCount;

            int readPids = wq_getPids(pipe->readProcessWaitQueue, info->readBlockedPids, MAX_PID_ARRAY_LENGTH);
            info->readBlockedPids[readPids] = -1;

            int writePids = wq_getPids(pipe->writeProcessWaitQueue, info->writeBlockedPids, MAX_PID_ARRAY_LENGTH);
            info->writeBlockedPids[writePids] = -1;

            if (pipe->name == NULL)
                info->name[0] = '\0';
            else
                strncpy(info->name, pipe->name, MAX_NAME_LENGTH);
        }
    }

    return pipeCounter;
}

static ssize_t fdReadHandler(TPid pid, int fd, void* resource, char* buf, size_t count) {
    TPipeFdMapping* mapping = (TPipeFdMapping*)resource;
    TPipeInternal* pipe = pipes[mapping->pipe];

    if (count == 0)
        return 0;

    ssize_t r;
    while ((r = pipeReadInternal(pipe, buf, count)) == 0 && (pipe->name != NULL || pipe->writerFdCount != 0)) {
        wq_add(pipe->readProcessWaitQueue, pid);
        sch_blockProcess(pid);
        sch_yieldProcess();
    }

    return r;
}

static ssize_t fdWriteHandler(TPid pid, int fd, void* resource, const char* buf, size_t count) {
    TPipeFdMapping* mapping = (TPipeFdMapping*)resource;
    TPipeInternal* pipe = pipes[mapping->pipe];

    if (count == 0)
        return 0;

    ssize_t r;
    while ((r = pipeWriteInternal(pipe, buf, count)) == 0 && (pipe->name != NULL || pipe->readerFdCount != 0)) {
        wq_add(pipe->writeProcessWaitQueue, pid);
        sch_blockProcess(pid);
        sch_yieldProcess();
    }

    return r == 0 ? -1 : r;
}

static int fdCloseHandler(TPid pid, int fd, void* resource) {
    TPipeFdMapping* mapping = (TPipeFdMapping*)resource;
    TPipeInternal* pipe = pipes[mapping->pipe];

    pipe->readerFdCount -= mapping->allowRead;
    pipe->writerFdCount -= mapping->allowWrite;
    int result = mm_free(mapping);

    // If this pipe is not named, we should check if there are no more readers or writers,
    // since this may imply the pipe has to be freed, or some processes have to be woken up.
    if (pipe->name == NULL) {
        if (pipe->readerFdCount == 0) {
            if (pipe->writerFdCount == 0) {
                // No more readers nor writers, fully dispose of the pipe.
                return result + pipe_free(mapping->pipe);
            }

            // No more readers, we don't need to keep any data inside the pipe. Free the internal buffer.
            result += mm_free(pipe->buffer);
            pipe->buffer = NULL;
            pipe->bufferSize = 0;
            wq_unblockAll(pipe->writeProcessWaitQueue);
        } else if (pipe->writerFdCount == 0)
            wq_unblockAll(pipe->readProcessWaitQueue);
    }

    return result;
}

static int fdDupHandler(TPid pidFrom, TPid pidTo, int fdFrom, int fdTo, void* resource) {
    TPipeFdMapping* mapping = (TPipeFdMapping*)resource;
    return pipe_mapToProcessFd(pidTo, fdTo, mapping->pipe, mapping->allowRead, mapping->allowWrite);
}




// This comment exists solely so this file has exactly 420 lines ((nice))