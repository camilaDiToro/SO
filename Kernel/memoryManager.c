#include <memoryManager.h>
#include <lib.h>
#include <graphicMode.h>

typedef struct memoryBlockNode {
    size_t size;
    size_t used;
    struct memoryBlockNode* previous;
    struct memoryBlockNode* next;
} TMemoryBlockNode;

static TMemoryBlockNode* firstBlock = NULL;

void mm_init(void* memoryStart, size_t memorySize) {
    // word-allign memoryStart by rounding up to a multiple of 8.
    void* actualStart = (void*)WORD_ALIGN_UP(memoryStart);
    memorySize -= (actualStart - memoryStart);

    // Allocate space for a first TMemoryBlock at the start of our segment.
    firstBlock = (TMemoryBlockNode*)actualStart;
    actualStart += sizeof(TMemoryBlockNode);
    memorySize -= sizeof(TMemoryBlockNode);

    firstBlock->size = memorySize;
    firstBlock->used = 0;
    firstBlock->previous = NULL;
    firstBlock->next = NULL;
}

void* mm_malloc(size_t size) {
    // word-allig size by rounding up to a multiple of 8.
    size = WORD_ALIGN_UP(size);

    TMemoryBlockNode* node = firstBlock;
    if (firstBlock == NULL)
        return NULL;

    // Find the first available node with enough size to fulfill the request, then mark it as used.
    while (node->used || node->size < size) {
        node = node->next;

        if (node == NULL)
            return NULL;
    }

    node->used = 1;
    
    // If that node has more space than requested, split it into another free node.
    size_t leftoverSize = node->size - size;
    if (leftoverSize > sizeof(TMemoryBlockNode)) {
        node->size = size;
        TMemoryBlockNode* newNode = (TMemoryBlockNode*)((void*)node + size + sizeof(TMemoryBlockNode));

        newNode->size = leftoverSize - sizeof(TMemoryBlockNode);
        newNode->used = 0;
        newNode->previous = node;
        newNode->next = node->next;
        node->next = newNode;

        if (newNode->next != NULL)
            newNode->next->previous = newNode;
    }

    return (void*)node + sizeof(TMemoryBlockNode);
}

int mm_free(void* ptr) {
    return 1;
}