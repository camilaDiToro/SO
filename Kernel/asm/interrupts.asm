GLOBAL _cli
GLOBAL _sti
GLOBAL picMasterMask
GLOBAL picSlaveMask
GLOBAL haltcpu
GLOBAL _hlt

GLOBAL _awakeScheduler
GLOBAL _int81

GLOBAL _irq00Handler
GLOBAL _irq01Handler
GLOBAL _irq02Handler	
GLOBAL _irq03Handler	
GLOBAL _irq04Handler
GLOBAL _irq05Handler
GLOBAL _exception0Handler
GLOBAL _exception6Handler
GLOBAL _exception0DHandler
GLOBAL _exception0EHandler
GLOBAL _sysCallHandler

EXTERN irqDispatcher
EXTERN sysCallDispatcher
EXTERN exceptionDispatcher
EXTERN sch_switchProcess

SECTION .text

%macro pushState 0
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
	push rax
%endmacro

%macro popState 0
	pop rax
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
%endmacro

%macro irqHandlerMaster 1
	pushState

	mov rdi, %1 
	call irqDispatcher

	; signal pic EOI (End of Interrupt)
	mov al, 20h
	out 20h, al

	popState
	iretq
%endmacro

%macro exceptionHandler 1
; Registers are saved in a the regdata vector in the following order:
; rax, rbx, rcx, rdx, rsi, rdi, rbp, rsp, r8, r9, r10, r11, r12, r13, r14, r15, rip, rflags

	mov [regdata], rax
	mov [regdata+8], rbx
	mov [regdata+16], rcx
	mov [regdata+24], rdx
	mov [regdata+32], rsi
	mov [regdata+40], rdi
	mov [regdata+48], rbp
	mov [regdata+64], r8
	mov [regdata+72], r9
	mov [regdata+80], r10
	mov [regdata+88], r11
	mov [regdata+96], r12
	mov [regdata+104], r13
	mov [regdata+112], r14
	mov [regdata+120], r15

	mov rax, [rsp+24] ; We get the value of RSP from the value pushed to the stack by the interruption.
	mov [regdata+56], rax
	mov rax, [rsp] ; We get the value of RIP when the exception ocurred by taking the interruption's return address.
	mov [regdata+128], rax
	mov rax, [rsp+16] ; We get the value of RFLAGS the same way (it is pushed when an interrupt occurs).
	mov [regdata+136], rax

	mov rdi, %1
	mov rsi, regdata
	call exceptionDispatcher ; This function should not return.
	jmp haltcpu
%endmacro

haltcpu:
	cli
	hlt
	ret

picMasterMask:
	push rbp
    mov rbp, rsp
    mov ax, di
    out	21h, al
    pop rbp
    retn

picSlaveMask:
	push rbp
    mov rbp, rsp
    mov ax, di  ; ax is a 16 bits mask
    out	0A1h, al
    pop rbp
    retn

; 8254 Timer (Timer Tick)
_irq00Handler:
	pushState

	mov rdi, 0
	call irqDispatcher

	mov rdi, rsp
	call sch_switchProcess
	mov rsp, rax

	; signal pic EOI (End of Interrupt)
	mov al, 20h
	out 20h, al

	popState
	iretq

; Keyboard
_irq01Handler:
	irqHandlerMaster 1

; Zero Division Exception
_exception0Handler:
	exceptionHandler 00h

; Invalid Operand Exception
_exception6Handler:
	exceptionHandler 06h

; General Protection Exception
_exception0DHandler:
	exceptionHandler 0Dh

; Page Fault Exception
_exception0EHandler:
	exceptionHandler 0Eh

_sysCallHandler:
	mov rcx, r10
	mov r9, rax
	call sysCallDispatcher
	iretq

; To avoid ticking unnecesarily (e.g. when a process yields the CPU)
_awakeScheduler:
	pushState
	
	mov rdi, rsp
	call sch_switchProcess
	mov rsp, rax
	
	popState
	iretq

_int81:
	int 81h
	ret

_hlt:
	sti
	hlt
	ret

_cli:
	cli
	ret

_sti:
	sti
	ret

SECTION .bss
regdata	resq	18	; reserve space for 18 qwords (one for each register we want to show on exceptions).
