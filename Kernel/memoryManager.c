/* Local headers */
#include <memoryManager.h>
#include <lib.h>
#include <graphics.h>

/**
 * Represents a structure prepended to all allocated memory chunks to track said memory chunks.
 *
 * The size of this struct must be a multiple of 8 in order to preserve word-alignment.
 */
typedef struct memoryBlockNode {
    size_t size;
    size_t leftoverSize;
    struct memoryBlockNode* previous;
    struct memoryBlockNode* next;
    size_t checksum;
} TMemoryBlockNode;

static TMemoryBlockNode* firstBlock = NULL;

static void calcNodeChecksum(const TMemoryBlockNode* node, size_t* result) {
    *result = node->size ^ node->leftoverSize ^ (size_t)node->previous ^ (size_t)node->next;
}

void mm_init(void* memoryStart, size_t memorySize) {
    // word-allign memoryStart by rounding up to a multiple of 8.
    void* actualStart = (void*)WORD_ALIGN_UP(memoryStart);
    memorySize -= (actualStart - memoryStart);
    memorySize = WORD_ALIGN_DOWN(memorySize);

    // Allocate space for a first TMemoryBlockNode at the start of our segment.
    firstBlock = (TMemoryBlockNode*)actualStart;
    actualStart += sizeof(TMemoryBlockNode);
    memorySize -= sizeof(TMemoryBlockNode);

    firstBlock->size = 0;
    firstBlock->leftoverSize = memorySize;
    firstBlock->previous = NULL;
    firstBlock->next = NULL;
    calcNodeChecksum(firstBlock, &firstBlock->checksum);
}

void* mm_malloc(size_t size) {
    if (firstBlock == NULL || size == 0)
        return NULL;

    size = WORD_ALIGN_UP(size);

    TMemoryBlockNode* node = firstBlock;
    size_t totalSizeWithNode = size + sizeof(TMemoryBlockNode);

    // Find the first available node with enough size to fulfill the request.
    // A valid node needs to have enough leftoverSize for the requested space, plus the
    // TMemoryBlockNode placed at the start of the chunk. A node may however have a size
    // if 0, in which case it may be used 'as is' and creating a new node isn't necessary.
    while ((node->size != 0 || node->leftoverSize < size) && node->leftoverSize < totalSizeWithNode) {
        node = node->next;

        if (node == NULL)
            return NULL;
    }

    if (node->size == 0) {
        node->size = size;
        node->leftoverSize -= node->size;
        calcNodeChecksum(node, &node->checksum);
        return (void*)node + sizeof(TMemoryBlockNode);
    }

    TMemoryBlockNode* newNode = (TMemoryBlockNode*)((void*)node + sizeof(TMemoryBlockNode) + node->size);
    newNode->size = size;
    newNode->leftoverSize = node->leftoverSize - sizeof(TMemoryBlockNode) - newNode->size;
    newNode->previous = node;
    newNode->next = node->next;
    node->leftoverSize = 0;
    node->next = newNode;

    if (newNode->next != NULL) {
        newNode->next->previous = newNode;
        calcNodeChecksum(newNode->next, &newNode->next->checksum);
    }

    calcNodeChecksum(newNode, &newNode->checksum);
    calcNodeChecksum(node, &node->checksum);
    return (void*)newNode + sizeof(TMemoryBlockNode);
}

void* mm_realloc(void* ptr, size_t size) {
    size = WORD_ALIGN_UP(size);

    if (ptr == NULL)
        return mm_malloc(size);

    if (size == 0) {
        mm_free(ptr);
        return NULL;
    }

    TMemoryBlockNode* node = (TMemoryBlockNode*)(ptr - sizeof(TMemoryBlockNode));

    size_t checksum;
    calcNodeChecksum(node, &checksum);
    if (checksum != node->checksum)
        return NULL;

    if (node->size == size)
        return ptr;

    if (size < node->size) {
        node->leftoverSize += node->size - size;
        node->size = size;
        calcNodeChecksum(node, &node->checksum);
        return ptr;
    }

    size_t extraRequiredSize = size - node->size;
    if (node->leftoverSize >= extraRequiredSize) {
        node->leftoverSize -= extraRequiredSize;
        node->size = size;
        calcNodeChecksum(node, &node->checksum);
        return ptr;
    }

    void* newPtr = mm_malloc(size);
    if (newPtr != NULL)
        mm_free(ptr);
    return newPtr;
}

int mm_free(void* ptr) {
    if (ptr == NULL)
        return 0;

    TMemoryBlockNode* node = (TMemoryBlockNode*)(ptr - sizeof(TMemoryBlockNode));

    size_t checksum;
    calcNodeChecksum(node, &checksum);
    if (checksum != node->checksum)
        return 1;

    if (node->previous == NULL) {
        node->leftoverSize += node->size;
        node->size = 0;
        calcNodeChecksum(node, &node->checksum);
    } else {
        node->previous->leftoverSize += node->size + node->leftoverSize + sizeof(TMemoryBlockNode);
        node->previous->next = node->next;
        calcNodeChecksum(node->previous, &node->previous->checksum);

        if (node->next != NULL) {
            node->next->previous = node->previous;
            calcNodeChecksum(node->next, &node->next->checksum);
        }
    }

    return 0;
}

void mm_printDebug() {
    scr_print("\nMM debug:");
    int i = 0;
    for (TMemoryBlockNode* node = firstBlock; node != NULL; node = node->next, i++) {
        scr_printChar('\n');
        scr_printDec(i);
        scr_printChar('[');
        scr_printDec(node->size);
        scr_print(", ");
        scr_printDec(node->leftoverSize);
        scr_print(", ");
        if (node->previous == NULL)
            scr_print("NULL");
        else
            scr_printDec(node->previous->size);
        scr_print(", ");
        if (node->next == NULL)
            scr_print("NULL");
        else
            scr_printDec(node->next->size);
        scr_printChar(']');

        if (i > 16) {
            scr_print("\n...\n");
            TMemoryBlockNode* prev;
            for (; node != NULL; prev = node, node = node->next, i++)
                ;
            i--;
            node = prev;
            scr_printDec(i);
            scr_printChar('[');
            scr_printDec(node->size);
            scr_print(", ");
            scr_printDec(node->leftoverSize);
            scr_print(", ");
            if (node->previous == NULL)
                scr_print("NULL");
            else
                scr_printDec(node->previous->size);
            scr_print(", ");
            if (node->next == NULL)
                scr_print("NULL");
            else
                scr_printDec(node->next->size);
            scr_printChar(']');

            break;
        }
    }
}
