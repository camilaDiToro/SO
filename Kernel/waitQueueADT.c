/* Standard library */

/* Local headers */
#include <waitQueueADT.h>
#include <memoryManager.h>
#include <scheduler.h>
#include <lib.h>

#define BUFFER_CHUNK_SIZE 8

struct TWaitQueueInternal {
    TPid* buf;
    unsigned int offset;
    unsigned int count;
    unsigned int bufSize;
};

TWaitQueue wq_new() {
    TWaitQueue queue;
    if ((queue = mm_malloc(sizeof(struct TWaitQueueInternal))) == NULL)
        return NULL;
    
    queue->buf = NULL;
    queue->offset = 0;
    queue->count = 0;
    queue->bufSize = 0;
    return queue;
}

int wq_free(TWaitQueue queue) {
    return mm_free(queue->buf) + mm_free(queue);
}

int wq_add(TWaitQueue queue, TPid pid) {
    if (queue->count == queue->bufSize) {
        unsigned int newBufSize = queue->bufSize + BUFFER_CHUNK_SIZE;
        TPid* newBuf = mm_malloc(newBufSize * sizeof(TPid));

        if (newBuf == NULL)
            return 1;
        
        unsigned int x = queue->bufSize - queue->offset;
        if (x <= queue->count) {
            memcpy(newBuf, &queue->buf[queue->offset], queue->count * sizeof(TPid));
        } else {
            memcpy(newBuf, &queue->buf[queue->offset], x * sizeof(TPid));
            memcpy(&newBuf[x], queue->buf, (queue->count - x) * sizeof(TPid));
        }

        queue->offset = 0;
        mm_free(queue->buf);
        queue->buf = newBuf;
        queue->bufSize = newBufSize;
    }

    queue->buf[(queue->offset + queue->count++) % queue->bufSize] = pid;
    return 0;
}

int wq_count(TWaitQueue queue) {
    return queue->count;
}

int wq_contains(TWaitQueue queue, TPid pid) {
    for (unsigned int i = 0; i < queue->count; i++)
        if (queue->buf[(queue->offset + i) % queue->bufSize] == pid)
            return 1;
    return 0;
}

int wq_addIfNotExists(TWaitQueue queue, TPid pid) {
    return wq_contains(queue, pid) ? 0 : wq_add(queue, pid);
}

int wq_remove(TWaitQueue queue, TPid pid) {
    unsigned int i;
    for (i = 0; i < queue->count && queue->buf[(queue->offset + i) % queue->bufSize] != pid; i++);

    if (i == queue->count)
        return 1;
    
    queue->count--;
    for (; i < queue->count; i++)
        queue->buf[(queue->offset + i) % queue->bufSize] = queue->buf[(queue->offset + i + 1) % queue->bufSize];
    return 0;
}

int wq_unblockSingle(TWaitQueue queue) {
    int failed = 0;

    while (queue->count != 0) {
        TPid pid = queue->buf[queue->offset];
        queue->offset = (queue->offset + 1) % queue->bufSize;
        queue->count--;

        if (sch_unblockProcess(pid) == 0)
            return 0;
        failed++;
    }

    return failed;
}

int wq_unblockAll(TWaitQueue queue) {
    int failed = 0;

    for (unsigned int i = 0; i < queue->count; i++)
        if (sch_unblockProcess(queue->buf[(queue->offset + i) % queue->bufSize]) != 0)
            failed++;

    queue->count = 0;
    queue->offset = 0;
    return failed;
}