/* Standard library */
#include <string.h>

/* Local headers */
#include "syscalls.h"
#include "test_util.h"
#include <userstdlib.h>

#define MAX_BLOCKS (4 * 1024)

typedef struct MM_rq {
    void* address;
    uint32_t size;
} mm_rq;

void test_mm(int argc, char* argv[]) {

    // TODO: Buddy can't handle large mallocs!
    while (1) {
        void* ptr = sys_malloc(4000000);
        fprintf(STDERR, "i slep zzz\n");
        sys_free(ptr);
    }
    return;

    mm_rq mm_rqs[MAX_BLOCKS];
    uint8_t rq;
    uint32_t total;
    uint64_t max_memory;
    fprintf(STDERR, "A");

    max_memory = 4 * 1024 * 1024;

    while (1) {
        rq = 0;
        total = 0;

        // Request as many blocks as we can
        while (rq < MAX_BLOCKS && total < max_memory) {
            uint32_t cami = GetUniform(max_memory - total - 1) + 1;
    fprintf(STDERR, "B[%u]", (unsigned int)cami);
            mm_rqs[rq].size = cami;
    fprintf(STDERR, "J");
            mm_rqs[rq].address = sys_malloc(mm_rqs[rq].size);
    fprintf(STDERR, "C");

            if (mm_rqs[rq].address) {
                total += mm_rqs[rq].size;
                rq++;
            }
        }

    fprintf(STDERR, "D");
        // Set
        uint32_t i;
        for (i = 0; i < rq; i++)
            if (mm_rqs[i].address)
                setmem(mm_rqs[i].address, i, mm_rqs[i].size);
    fprintf(STDERR, "E");

        // Check
        for (i = 0; i < rq; i++)
            if (mm_rqs[i].address)
                if (!memcheck(mm_rqs[i].address, i, mm_rqs[i].size)) {
                    fprintf(STDERR, "test_mm ERROR\n");
                    return;
                }
    fprintf(STDERR, "F");

        // Free
        for (i = 0; i < rq; i++)
            if (mm_rqs[i].address)
                sys_free(mm_rqs[i].address);
    fprintf(STDERR, "G");
    }
}
