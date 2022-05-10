GLOBAL cpuVendor
GLOBAL save_registers
GLOBAL _int20

; systemCalls.c
EXTERN store_registers

section .text

cpuVendor:
	push rbx

	mov rax, 0
	cpuid


	mov [rdi], ebx
	mov [rdi + 4], edx
	mov [rdi + 8], ecx

	mov byte [rdi+13], 0

	mov rax, rdi

	pop rbx
	ret

; Save registers for inforeg
save_registers:
	push rbp
	mov rbp, rsp

	mov rdi, rsp
	add rdi, 112		; 14*8 = 112
	call store_registers

    mov rsp, rbp
	pop rbp
	ret

_int20:
	int 20h
	ret