#ifndef _INTERRUPTS_H_
#define _INTERRUPTS_H_

/* Standard library */
#include <stdint.h>

/* Interruption handlers */
void _irq00Handler(void);
void _irq01Handler(void);

/* Exception handlers */
void _exception0Handler(void);
void _exception6Handler(void);
void _exception0DHandler(void);
void _exception0EHandler(void);

/* Syscall handler */ 
void _sysCallHandler(void);

/* Awake Scheduler without ticking */
void _awakeScheduler(void);
void _int81(void);

/* CPU and PIC */
void _cli(void);
void _sti(void);
void _hlt(void);
void haltcpu(void);
void picMasterMask(uint8_t mask);
void picSlaveMask(uint8_t mask);

#endif
