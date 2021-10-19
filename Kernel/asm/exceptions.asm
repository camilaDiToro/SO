GLOBAL print_registers
EXTERN ncPrint
EXTERN ncPrintRegisterFormat
EXTERN ncNewline
section .text

%macro pushRegisters 0
    push rax
	push rbx
	push rcx
	push rdx
	push rbp
	push rdi
	push rsi
	push r8
	push r9
	push r10
	push r11
	push r12
	push r13
	push r14
	push r15
%endmacro

%macro popRegisters 0
	pop r15
	pop r14
	pop r13
	pop r12
	pop r11
	pop r10
	pop r9
	pop r8
	pop rsi
	pop rdi
	pop rbp
	pop rdx
	pop rcx
	pop rbx
    pop rax
%endmacro

; Only INTEGER will be contained into registers.
; ncPrint in the future will be replaced by another function in video mode.
; This function is just a first approach of what we must do
; We must check how to print the instruction that fails
; and also google the stack layout when we must handle an exception.
; TODO: check if the 15 registers are printed.
; https://os.phil-opp.com/handling-exceptions/

print_registers:
    push rbp
    mov rbp, rsp

    pushRegisters
    mov rdi, registerRAX
    call ncPrint
    mov rdi, rax
    call ncPrintRegisterFormat
    call ncNewline
    popRegisters

    pushRegisters
    mov rdi, registerRBX
    call ncPrint
    mov rdi, rbx
    call ncPrintRegisterFormat
    call ncNewline
    popRegisters

    pushRegisters
    mov rdi, registerRCX
    call ncPrint
    mov rdi, rcx
    call ncPrintRegisterFormat
    call ncNewline
    popRegisters

    pushRegisters
    mov rdi, registerRDX
    call ncPrint
    mov rdi, rdx
    call ncPrintRegisterFormat
    call ncNewline
    popRegisters

    pushRegisters
    mov rdi, registerRBP
    call ncPrint
    mov rdi, rbp
    call ncPrintRegisterFormat
    call ncNewline
    popRegisters

    pushRegisters
    mov rax, rdi
    mov rdi, registerRDI
    call ncPrint
    mov rdi, rax
    call ncPrintRegisterFormat
    call ncNewline
    popRegisters

    pushRegisters
    mov rdi, registerRSI
    call ncPrint
    mov rdi, rsi
    call ncPrintRegisterFormat
    call ncNewline
    popRegisters

    pushRegisters
    mov rdi, registerRSP
    call ncPrint
    mov rdi, rsp                     
    call ncPrintRegisterFormat
    call ncNewline
    popRegisters

    pushRegisters
    mov rdi, registerR8
    call ncPrint
    mov rdi, r8
    call ncPrintRegisterFormat
    call ncNewline
    popRegisters

    pushRegisters
    mov rdi, registerR9
    call ncPrint
    mov rdi, r9
    call ncPrintRegisterFormat
    call ncNewline
    popRegisters

    pushRegisters
    mov rdi, registerR10
    call ncPrint
    mov rdi, r10
    call ncPrintRegisterFormat
    call ncNewline
    popRegisters

    pushRegisters
    mov rdi, registerR11
    call ncPrint
    mov rdi, r11
    call ncPrintRegisterFormat
    call ncNewline
    popRegisters

    pushRegisters
    mov rdi, registerR12
    call ncPrint
    mov rdi, r12
    call ncPrintRegisterFormat
    call ncNewline
    popRegisters

    pushRegisters
    mov rdi, registerR13
    call ncPrint
    mov rdi, r13
    call ncPrintRegisterFormat
    call ncNewline
    popRegisters

    pushRegisters
    mov rdi, registerR14
    call ncPrint
    mov rdi, r14
    call ncPrintRegisterFormat
    call ncNewline
    popRegisters

    pushRegisters
    mov rdi, registerR15
    call ncPrint
    mov rdi, r15
    call ncPrintRegisterFormat
    call ncNewline
    popRegisters

    mov rsp, rbp
    pop rbp
    ret

section .data
    registerRAX db " RAX= ", 0
    registerRBX db " RBX= ", 0
    registerRCX db " RCX= ", 0
    registerRDX db " RDX= ", 0
    registerRBP db " RBP= ", 0
    registerRDI db " RDI= ", 0
    registerRSI db " RSI= ", 0
    registerRSP db " RSP= ", 0
    registerR8 db " R8= ", 0
    registerR9 db " R9= ", 0
    registerR10 db " R10= ", 0
    registerR11 db " R11= ", 0
    registerR12 db " R12= ", 0
    registerR13 db " R13= ", 0
    registerR14 db " R14= ", 0
    registerR15 db " R15= ", 0