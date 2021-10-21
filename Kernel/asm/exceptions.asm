GLOBAL print_registers
GLOBAL give_control_to_user
EXTERN print
EXTERN printRegisterFormat
EXTERN newLine
EXTERN main
EXTERN getStackBase
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
give_control_to_user:
    call getStackBase	        ; Get thet stack address
	mov rsp, rax				; Set up the stack with the returned address
	call main
    

; This function is just a first approach of what we must do
; We must check how to print the instruction that fails
; and also google the stack layout when an exception is handled.
; https://os.phil-opp.com/handling-exceptions/

print_registers:
    push rbp
    mov rbp, rsp

    pushRegisters
    mov rdi, registerRAX
    call print
    mov rdi, rax
    call printRegisterFormat
    mov rdi, 0
    call newLine
    popRegisters

    pushRegisters
    mov rdi, registerRBX
    call print
    mov rdi, rbx
    call printRegisterFormat
    mov rdi, 0
    call newLine
    popRegisters

    pushRegisters
    mov rdi, registerRCX
    call print
    mov rdi, rcx
    call printRegisterFormat
    mov rdi, 0
    call newLine
    popRegisters

    pushRegisters
    mov rdi, registerRDX
    call print
    mov rdi, rdx
    call printRegisterFormat
    mov rdi, 0
    call newLine
    popRegisters

    pushRegisters
    mov rdi, registerRBP
    call print
    mov rdi, rbp
    call printRegisterFormat
    mov rdi, 0
    call newLine
    popRegisters

    pushRegisters
    mov rax, rdi
    mov rdi, registerRDI
    call print
    mov rdi, rax
    call printRegisterFormat
    mov rdi, 0
    call newLine
    popRegisters

    pushRegisters
    mov rdi, registerRSI
    call print
    mov rdi, rsi
    call printRegisterFormat
    mov rdi, 0
    call newLine
    popRegisters

    pushRegisters
    mov rdi, registerRSP
    call print
    mov rdi, rsp                     
    call printRegisterFormat
    mov rdi, 0
    call newLine
    popRegisters

    pushRegisters
    mov rdi, registerR8
    call print
    mov rdi, r8
    call printRegisterFormat
    mov rdi, 0
    call newLine
    popRegisters

    pushRegisters
    mov rdi, registerR9
    call print
    mov rdi, r9
    call printRegisterFormat
    mov rdi, 0
    call newLine
    popRegisters

    pushRegisters
    mov rdi, registerR10
    call print
    mov rdi, r10
    call printRegisterFormat
    mov rdi, 0
    call newLine
    popRegisters

    pushRegisters
    mov rdi, registerR11
    call print
    mov rdi, r11
    call printRegisterFormat
    mov rdi, 0
    call newLine
    popRegisters

    pushRegisters
    mov rdi, registerR12
    call print
    mov rdi, r12
    call printRegisterFormat
    mov rdi, 0
    call newLine
    popRegisters

    pushRegisters
    mov rdi, registerR13
    call print
    mov rdi, r13
    call printRegisterFormat
    mov rdi, 0
    call newLine
    popRegisters

    pushRegisters
    mov rdi, registerR14
    call print
    mov rdi, r14
    call printRegisterFormat
    mov rdi, 0
    call newLine
    popRegisters

    pushRegisters
    mov rdi, registerR15
    call print
    mov rdi, r15
    call printRegisterFormat
    mov rdi, 0
    call newLine
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

