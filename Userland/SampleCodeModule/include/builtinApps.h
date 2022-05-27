#ifndef _BUILTINAPPS_H_
#define _BUILTINAPPS_H_

/* Standard library */
#include <stdint.h>

void loopProcess(uint64_t argc, char* argv[]);

void catProcess(uint64_t argc, char* argv[]);

void wcProcess(uint64_t argc, char* argv[]);

void filterProcess(uint64_t argc, char* argv[]);

#endif