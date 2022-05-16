GLOBAL print_registers
GLOBAL give_control_to_user

EXTERN scr_print
EXTERN scr_printRegisterFormat
EXTERN scr_printLine

section .text
 
    ;   Retrieved from https://os.phil-opp.com/handling-exceptions/ 
    ;   The function must print the registers mentioned below in the following order:
    ;   RAX R15 R14 R13 R12 R11 R10 R9 R8 RSI RDI RBP RDX RCX RBX RIP CS RFLAGS RSP SS
    ;   because they were pushed to the stack in reverse order (checked using GDB).
    ;   Notice that RIP will point to the instruction that caused the exception.

    ;   -- lower part of the stack --
    ;	RBP
    ;   RIP    -> calling function 
    ;   RAX
    ;   R15
    ;   R14
    ;   R13
    ;   R12
    ;   R11
    ;   R10
    ;   R9
    ;   R8
    ;   RSI
    ;   RDI
    ;   RBP
    ;   RDX
    ;   RCX
    ;   RBX
    ;   RIP     -> instruction that caused the exception
    ;   CS
    ;   RFLAGS
    ;   RSP
    ;   SS  
    ;   -- upper part of the stack --

print_registers:
    push rbp
    mov rbp, rsp

    xor r10, r10

.loop_registers:
      
    mov rdi, [registers+r10]
    call scr_print
    add r10, 8
    mov rdi, [rbp+r10+8]
    call scr_printRegisterFormat
    call scr_printLine
    cmp r10, length
    jne .loop_registers

    mov rsp, rbp
    pop rbp
    ret

section .data
    segmentSS db " SS = ", 0
    rflags db " RFLAGS = ", 0
    segmentCS db " CS = ", 0
    registerRIP db " RIP = ", 0
    registerRAX db " RAX = ", 0
    registerRBX db " RBX = ", 0
    registerRCX db " RCX = ", 0
    registerRDX db " RDX = ", 0
    registerRBP db " RBP = ", 0
    registerRDI db " RDI = ", 0
    registerRSI db " RSI = ", 0
    registerRSP db " RSP = ", 0
    registerR8 db  " R8 = ", 0
    registerR9 db  " R9 = ", 0
    registerR10 db " R10 = ", 0
    registerR11 db " R11 = ", 0
    registerR12 db " R12 = ", 0
    registerR13 db " R13 = ", 0
    registerR14 db " R14 = ", 0
    registerR15 db " R15 = ", 0
    registers dq  registerRAX, registerR15, registerR14, registerR13, registerR12, registerR11, registerR10,registerR9, registerR8, registerRSI, registerRDI, registerRBP, registerRDX, registerRCX, registerRBX, registerRIP, segmentCS, rflags, registerRSP, segmentSS
    length equ $-registers
