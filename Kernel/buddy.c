#include <stdint.h>
#include <stddef.h>

/*
    Buddy is a method of memory allocation where available space
    is repeatedly split into halves, know as buddies.
    It keeps splitting buddies until it chooses the smallest
    possible block that can contain the file.
    This algorithm requires units of space to be a power of two
    and leads to considerable internal fragmentation. However,
    the buddy method is fast to allocate and deallocate memory.
*/

// https://www.researchgate.net/publication/329809459_A_NEW_IMPLEMENTATION_TECHNIQUE_FOR_BUDDY_SYSTEM
// https://github.com/lotabout/buddy-system

// Maximum block size
#define MAX_LEVEL 23

// Minimun block size
#define MIN_LEVEL 8

// Minimun memory size for buddy system (9MB)
#define MIN_MEMORY_SIZE (9 * 1024 * 1024) 

// Heap memory size is 8MB, 1MB will be used to allocate an array to represent a complete binary tree
#define HEAP_MEMORY_SIZE (8 * 1024 * 1024) 

// Maximun nodes quantity
#define MAX_NODES 65535

typedef struct node {
    unsigned int level;
    unsigned int occupied;
    unsigned int occupiedSubnodes;
    void* memory;
} TNode;

static TNode* nodes;

static size_t wrapDownToPowerOfTwo(size_t value) {
    int power = 0;
    while (value > 0) {
        value >>= 1;
        power++;
    }
    return 1 << (power - 1);
}

static int isAPowerOf2(int number) {
    return !(number & (number - 1));
}

static unsigned int getLeftChildIndex(unsigned int index){
    /* index * 2 + 1 */
    return (index << 1) + 1; 
}

static unsigned int getRightChildIndex(unsigned int index){
    /* index * 2 + 2 */
    return (index << 1) + 2;
}

static unsigned int getParentIndex(unsigned int index){
    /* (index - 1) / 2 */
    return (index - 1) >> 1;
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

void bd_init(void* memoryStart, size_t memorySize) {

    // Word-allign memoryStart by rounding up to a multiple of 8.
    void* actualStart = (void*)WORD_ALIGN_UP(memoryStart);
    memorySize -= (actualStart - memoryStart);

    if (memorySize < MIN_MEMORY_SIZE){
        return;
    }

    // Allocate an array to represent a complete binary tree
    nodes = actualStart;
    size_t nodesMemorySize = sizeof(TNode) * MAX_NODES;
    actualStart += nodesMemorySize;

    // Allocate first node
    nodes[0].level = MAX_LEVEL;
    nodes[0].memory = actualStart;
    nodes[0].occupied = 0;
    nodes[0].occupiedSubnodes = 0;
}

