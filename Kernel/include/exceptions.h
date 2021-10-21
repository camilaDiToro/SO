#ifndef __EXCEPTIONS_H__
#define __EXCEPTIONS_H__

void exceptionDispatcher(int exception);
void give_control_to_user();
void _halt();
void print_registers();
void printRegisterFormat(uint64_t reg);
void _hlt();


#endif