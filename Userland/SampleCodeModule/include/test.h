#ifndef _TESTING_H_
#define _TESTING_H_

/* Standard library */
#include <stdint.h>

uint64_t test_mm(uint64_t argc, char* argv[]);

uint64_t test_sync(uint64_t argc, char* argv[]);

int64_t test_processes(uint64_t argc, char* argv[]);

void test_prio(uint64_t argc, char* argv[]);

void namedPipesProcess(uint64_t argc, char* argv[]);

#endif