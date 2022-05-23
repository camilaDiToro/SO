/* Standard Library */
#include <stdint.h>
#include <stddef.h>
#include <lib.h>

/*
    Buddy is a method of memory allocation where available space
    is repeatedly split into halves, know as buddies.
    It keeps splitting buddies until it chooses the smallest
    possible block that can contain the file.
    This algorithm requires units of space to be a power of two
    and leads to considerable internal fragmentation. However,
    the buddy method is fast to allocate and deallocate memory.
*/

/* Maximum block size (inclusive) */
#define MAX_LEVEL 23

/* Minimun block size (inclusive) */
#define MIN_LEVEL 8

/* Minimun memory size for buddy system (9MB) */
#define MIN_MEMORY_SIZE (9 * 1024 * 1024)

/* Heap memory size is 8MB, 1MB will be used to allocate an array to represent a complete binary tree */
#define HEAP_MEMORY_SIZE (8 * 1024 * 1024)

/* Maximun nodes quantity */
#define MAX_NODES 65535

#define TRUE 1
#define FALSE (!TRUE)

typedef struct {
    /* Whether this node is in use by a malloc. */
    unsigned int occupied;

    /* The amount of subnodes (including this node) that are marked as occupied. */
    unsigned int occupiedSubnodes;
} TNode;

static void* heapStart;
static TNode* nodes;

static size_t wrapDownToPowerOfTwo(size_t value) {
    int power = 0;
    while (value != 0) {
        value >>= 1;
        power++;
    }
    return 1 << (power - 1);
}

static size_t wrapUpToPowerOfTwo(size_t value) {
    value--;
    int power = 0;
    while (value != 1) {
        value >>= 1;
        power++;
    }
    return 1 << power;
}

static inline unsigned int getLeftChildIndex(unsigned int index) {
    return (index << 1) + 1;
}

static inline unsigned int getRightChildIndex(unsigned int index) {
    return (index << 1) + 2;
}

static inline unsigned int getParentIndex(unsigned int index) {
    return ((index + 1) >> 1) - 1;
}

static inline unsigned int getBuddyIndex(unsigned int index) {
    return isLeftChild(index) ? getRightChildIndex(getParentIndex(index)) : getLeftChildIndex(getParentIndex(index));
}

static inline unsigned int getFirstIndexOfLevel(unsigned int level) {
    return (1 << (MAX_LEVEL - level)) - 1;
}

static int getFirstFreeIndexOfLevel(unsigned int level) {
    int idx = getFirstIndexOfLevel(level);
    int lastIdx = getFirstIndexOfLevel(level - 1);

    while (idx < lastIdx && (nodes[idx].occupied || nodes[idx].occupiedSubnodes))
        ++idx;

    if (idx == lastIdx)
        return -1;

    return idx;
}

static void setAsOccupied(int idx, int occupied) {
    if (idx >= MAX_NODES) {
        return;
    }
    nodes[idx].occupied = occupied;
    setAsOccupied(getRightChildIndex(idx), occupied);
    setAsOccupied(getLeftChildIndex(idx), occupied);
}

static void updateParents(unsigned int idx, int occupiedDelta) {
    nodes[idx].occupiedSubnodes += occupiedDelta;
    while (idx != 0) {
        idx = getParentIndex(idx);
        nodes[idx].occupiedSubnodes += occupiedDelta;
    }
}

static void* getAddress(int index, int level) {
    return heapStart + (index - getFirstIndexOfLevel(level)) * (1 << level);
}

static int searchNode(int idx, int level, void* ptr) {
    if (level < MIN_LEVEL) 
        return -1;

    if (!nodes[idx].occupied) {
        void* address = getAddress(getRightChildIndex(idx), level - 1);
        if (ptr < address)
            return searchNode(getLeftChildIndex(idx), level - 1, ptr);
        return searchNode(getRightChildIndex(idx), level - 1, ptr);
    }

    if (ptr == getAddress(idx, level))
        return idx;
    return -1;
}

static unsigned int getLevel(size_t size) {
    size--;
    unsigned int level = 0;
    while (size != 0) {
        size >>= 1;
        level++;
    }
    return level;
}

static int getMaxPosibleLevel(void * ptr) {
    int relativeAddress = ptr - heapStart;
    int level = MAX_LEVEL;
    int powOfTwo = 1 << MAX_LEVEL;
    while ((relativeAddress % powOfTwo) != 0) {
        level--;
        powOfTwo = powOfTwo >> 1;
    }
    return level;
}

static int getStartSearchingIdx(void * ptr, int maxLevel){
    int relativeAddress = ptr - heapStart;
    return getFirstIndexOfLevel(maxLevel) + (relativeAddress/(1<<maxLevel));
}


void bd_init(void* memoryStart, size_t memorySize) {

    // Word-allign memoryStart by rounding up to a multiple of 8.
    void* actualStart = (void*)WORD_ALIGN_UP(memoryStart);
    memorySize -= (actualStart - memoryStart);

    if (memorySize < MIN_MEMORY_SIZE) {
        return;
    }

    // Allocate an array to represent a complete binary tree
    nodes = actualStart;
    size_t nodesMemorySize = sizeof(TNode) * MAX_NODES;
    actualStart += nodesMemorySize;
    heapStart = actualStart;

    // Set all nodes as free
    for (int i = 0; i < MAX_NODES; ++i) {
        nodes[i].occupied = 0;
        nodes[i].occupiedSubnodes = 0;
    }
}

void* bd_malloc(size_t size) {
    unsigned int level;
    if (size > HEAP_MEMORY_SIZE || size == 0 || (level = getLevel(size)) > MAX_LEVEL)
        return NULL;

    if (level < MIN_LEVEL)
        level = MIN_LEVEL;

    int index = getFirstFreeIndexOfLevel(level);

    // Ran out of memory
    if (index < 0)
        return NULL;

    updateParents(index, 1);
    setAsOccupied(index, TRUE);

    return getAddress(index, level);
}

int bd_free(void* ptr) {
    if (ptr == NULL)
        return 0;

    if (ptr < heapStart || ptr >= (heapStart + HEAP_MEMORY_SIZE))
        return -1;


    int maxLevel = getMaxPosibleLevel(ptr);

    int index = searchNode(getStartSearchingIdx(ptr, maxLevel), maxLevel, ptr);

    if (index < 0)
        return -1;

    updateParents(index, -1);
    setAsOccupied(index, FALSE);

    return 0;
}

void* bd_realloc(void* ptr, size_t size) {
    void* newPtr = bd_malloc(ptr);

    if (newPtr != NULL) {
        memcpy(newPtr, ptr, size);
        bd_free(ptr);
    }

    return newPtr;
}

// Debbuging
#include <graphics.h>
static void debug(unsigned int index, unsigned int level) {
    if (index >= MAX_NODES)
        return;

    if (nodes[index].occupied) {
        scr_print("Node: index=");
        scr_printDec(index);
        scr_print(", occupiedSubnodes=");
        scr_printDec(nodes[index].occupiedSubnodes);
        scr_print(", level=");
        scr_printDec(level);
        scr_print("\n");
    } else {
        debug(getLeftChildIndex(index), level - 1);
        debug(getRightChildIndex(index), level - 1);
    }
}

void bd_printDebug() {
    debug(0, MAX_LEVEL);
}
