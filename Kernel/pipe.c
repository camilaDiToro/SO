/* Standard library */
#include <stdint.h>
#include <stddef.h>
#include <sys/types.h>

/* Local headers */
#include <pipe.h>
#include <memoryManager.h>
#include <process.h>
#include <lib.h>
#include <graphics.h>
#include <scheduler.h>
#include <waitQueueADT.h>

/** The minimum size for a non-null pipe buffer. */
#define PIPE_MIN_BUFFER_SIZE 512

/** The maximum size for a pipe buffer. */
#define PIPE_MAX_BUFFER_SIZE 4096

/**
 * Defines a size rounding operation for pipe buffer size values. The returned value must be equal or greater than the input.
 */
#define PIPE_ROUND_BUFFER_SIZE(value) (((value) + 511) / 512 * 512)

struct TPipeInternal {
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

    unsigned int readerFdCount, writerFdCount;

    TWaitQueue readProcessWaitQueue, writeProcessWaitQueue;
};

typedef struct {
    TPipe pipe;
    int allowRead, allowWrite;
} TPipeFdMapping;

static ssize_t fdReadHandler(TPid pid, int fd, void* resource, char* buf, size_t count);
static ssize_t fdWriteHandler(TPid pid, int fd, void* resource, const char* buf, size_t count);
static int fdCloseHandler(TPid pid, int fd, void* resource);

TPipe pipe_create() {
    TPipe pipe;
    TWaitQueue readQueue = NULL, writeQueue = NULL;
    if ((pipe = mm_malloc(sizeof(struct TPipeInternal))) == NULL || (readQueue = wq_new()) == NULL || (writeQueue = wq_new()) == NULL) {
        mm_free(pipe);
        if (readQueue != NULL)
            wq_free(readQueue);
        return NULL;
    }

    // Initialize the pipe as empty, don't even allocate a buffer until it's necessary.
    pipe->buffer = NULL;
    pipe->bufferSize = 0;
    pipe->readOffset = 0;
    pipe->remainingBytes = 0;
    pipe->readerFdCount = 0;
    pipe->writerFdCount = 0;
    pipe->readProcessWaitQueue = readQueue;
    pipe->writeProcessWaitQueue = writeQueue;
    return pipe;
}

int pipe_free(TPipe pipe) {
    return mm_free(pipe->buffer) + wq_free(pipe->readProcessWaitQueue) + wq_free(pipe->writeProcessWaitQueue) + mm_free(pipe);
}

ssize_t pipe_write(TPipe pipe, const void* buf, size_t count) {
    if (pipe->bufferSize < count && pipe->bufferSize < PIPE_MAX_BUFFER_SIZE) {
        // Expand this pipe's buffer to a size of min(count, PIPE_MAX_BUFFER_SIZE) rounded up.
        size_t newBufferSize = count;
        if (newBufferSize > PIPE_MAX_BUFFER_SIZE)
            newBufferSize = PIPE_MAX_BUFFER_SIZE;
        newBufferSize = PIPE_ROUND_BUFFER_SIZE(newBufferSize);
        
        void* newBuf = mm_malloc(newBufferSize);
        if (newBuf != NULL) {
            size_t x = pipe->bufferSize - pipe->readOffset;
            if (x <= pipe->remainingBytes) {
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
    return bytesToWrite;
}

ssize_t pipe_read(TPipe pipe, void* buf, size_t count) {
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
    return bytesToRead;
}

int pipe_mapToProcessFd(TPid pid, int fd, TPipe pipe, int allowRead, int allowWrite) {
    TPipeFdMapping* mapping = mm_malloc(sizeof(TPipeFdMapping));
    if (mapping == NULL)
        return -1;

    int r = prc_mapFd(pid, fd, mapping, allowRead ? &fdReadHandler : NULL, allowWrite ? &fdWriteHandler : NULL, &fdCloseHandler);
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
    // TODO: lockPipe(); // Should this be done in this function? Who's the calling PID? What if it's the kernel?
    pipe->readerFdCount += allowRead;
    pipe->writerFdCount += allowWrite;
    // TODO: unlockPipe();

    return r;
}

void pipe_printDebug(TPipe pipe) {
    scr_printChar('[');
    scr_printHex((uint64_t)pipe->buffer);
    scr_print(", ");
    scr_printDec(pipe->bufferSize);
    scr_print(", ");
    scr_printDec(pipe->readOffset);
    scr_print(", ");
    scr_printDec(pipe->remainingBytes);
    scr_printChar(']');
}

static ssize_t fdReadHandler(TPid pid, int fd, void* resource, char* buf, size_t count) {
    TPipeFdMapping* mapping = (TPipeFdMapping*) resource;
    TPipe pipe = mapping->pipe;

    if (count == 0)
        return 0;

    // TODO: Make access to the TPipe struct mutually exclusive between processes (but interruptable?)

    // lockPipe(); // With a semaphore or somethin
    ssize_t r;
    while ((r = pipe_read(pipe, buf, count)) == 0 && pipe->writerFdCount != 0) {
        wq_add(pipe->readProcessWaitQueue, pid);
        sch_blockProcess(pid);
        sch_yieldProcess();

        // Add to queue of blocked
        // unlockPipe();
        // sch_block(pid);
        // sch_ready(pidOtro);
        // sch_yield(pid);
        // lockPipe();
    }

    // If no bytes remain for reading and there are no writers, then "end of file" was reached.
    // We can free the internal pipe's buffer to save memory.
    if (pipe->buffer != NULL && pipe->writerFdCount == 0 && pipe->remainingBytes == 0) {
        mm_free(pipe->buffer);
        pipe->buffer = NULL;
        pipe->bufferSize = 0;
        wq_unblockAll(pipe->readProcessWaitQueue);
    } else
        wq_unblockAll(pipe->writeProcessWaitQueue);

    // unlockPipe();
    return r;
}

static ssize_t fdWriteHandler(TPid pid, int fd, void* resource, const char* buf, size_t count) {
    TPipeFdMapping* mapping = (TPipeFdMapping*) resource;
    TPipe pipe = mapping->pipe;

    if (count == 0)
        return 0;

    // TODO: Implement blocking write.
    // TODO: Make access to the TPipe struct mutually exclusive between processes.
    // lockPipe();
    ssize_t r;
    while ((r = pipe_write(pipe, buf, count)) == 0 && pipe->readerFdCount != 0) {
        wq_add(pipe->writeProcessWaitQueue, pid);
        sch_blockProcess(pid);
        sch_yieldProcess();

        // Add to queue of blocked
        // unlockPipe();
        // sch_block(pid);
        // sch_ready(pidOtro);
        // sch_yield(pid);
        // lockPipe();
    }

    wq_unblockAll(pipe->readProcessWaitQueue);

    // unlockPipe();
    return r == 0 ? -1 : r;
}

static int fdCloseHandler(TPid pid, int fd, void* resource) {
    TPipeFdMapping* mapping = (TPipeFdMapping*) resource;
    TPipe pipe = mapping->pipe;
    // TODO: Make access to the TPipe struct mutually exclusive between processes.
    // lockPipe();
    pipe->readerFdCount -= mapping->allowRead;
    pipe->writerFdCount -= mapping->allowWrite;
    int result = mm_free(mapping);

    wq_remove(pipe->readProcessWaitQueue, pid);
    wq_remove(pipe->writeProcessWaitQueue, pid);

    if (pipe->readerFdCount == 0) {
        if (pipe->writerFdCount == 0) {
            // No more readers nor writers, fully dispose of the pipe.
            return result + pipe_free(pipe);
        } else {
            // No more readers, we don't need to keep any data inside the pipe. Free the internal buffer.
            result += mm_free(pipe->buffer);
            pipe->buffer = NULL;
            pipe->bufferSize = 0;
            wq_unblockAll(pipe->writeProcessWaitQueue);
        }
    } else if (pipe->writerFdCount == 0)
        wq_unblockAll(pipe->readProcessWaitQueue);

    // unlockPipe();
    return result;
}