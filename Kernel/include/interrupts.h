#ifndef _INTERRUPTS_H_
#define _INTERRUPTS_H_

/* Standard library */
#include <stdint.h>

void _irq00Handler(void);
void _irq01Handler(void);

void _exception0Handler(void);
void _exception6Handler(void);
void _exception0DHandler(void);
void _exception0EHandler(void);

void _awakeScheduler(void);
void _int81(void);

void _sysCallHandler(void);

void _cli(void);
void _sti(void);
void _hlt(void);
void haltcpu(void);
void picMasterMask(uint8_t mask);
void picSlaveMask(uint8_t mask);

#endif
