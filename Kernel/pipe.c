/* Standard library */
#include <stdint.h>
#include <stddef.h>
#include <sys/types.h>

#include <string.h>

/* Local headers */
#include <pipe.h>
#include <memoryManager.h>
#include <process.h>
#include <lib.h>
#include <graphics.h>

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
};

static ssize_t fdReadHandler(TPid pid, int fd, void* resource, char* buf, size_t count);
static ssize_t fdWriteHandler(TPid pid, int fd, void* resource, const char* buf, size_t count);
static int fdCloseHandler(TPid pid, int fd, void* resource);

TPipe pipe_create() {
    TPipe pipe = mm_malloc(sizeof(struct TPipeInternal));
    if (pipe == NULL)
        return NULL;
    
    // Initialize the pipe as empty, don't even allocate a buffer until it's necessary.
    pipe->buffer = NULL;
    pipe->bufferSize = 0;
    pipe->readOffset = 0;
    pipe->remainingBytes = 0;
    return pipe;
}

int pipe_free(TPipe pipe) {
    return mm_free(pipe->buffer) + mm_free(pipe);
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
            memcpy(newBuf, pipe->buffer + pipe->readOffset, pipe->bufferSize - pipe->readOffset);
            memcpy(newBuf + pipe->readOffset, pipe->buffer, pipe->readOffset);
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

size_t pipe_getRemainingBytes(TPipe pipe) {
    return pipe->remainingBytes;
}

int pipe_mapToProcessFd(TPipe pipe, TPid pid) {
    int r = prc_mapFd(pid, pipe, &fdReadHandler, &fdWriteHandler, &fdCloseHandler);
    if (r < 0)
        return r;

    // TODO: process tracking? "Who is using each pipe" so we know when to dispose them.

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
    // TPipe pipe = (TPipe) resource;
    // TODO: Implement blocking read
    return -1;
}

static ssize_t fdWriteHandler(TPid pid, int fd, void* resource, const char* buf, size_t count) {
    // TPipe pipe = (TPipe) resource;
    // TODO: Implement blocking write
    return -1;
}

static int fdCloseHandler(TPid pid, int fd, void* resource) {
    // TPipe pipe = (TPipe) resource;
    // TODO: process tracking? "Who is using each pipe" so we know when to dispose them.
    return -1;
}