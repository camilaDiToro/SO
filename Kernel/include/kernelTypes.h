#ifndef _KERNEL_TYPES_H_
#define _KERNEL_TYPES_H_

typedef enum status { READY = 0, BLOCKED, KILLED } TProcessStatus;

typedef int TPid;
typedef int8_t TPriority;

#endif