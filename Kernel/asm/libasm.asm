GLOBAL cpuVendor
GLOBAL sys_RTClock
GLOBAL sys_readKey

section .text

sys_RTClock:
	push rbp
	mov rbp, rsp

	mov al, dil
	out 70h, al
	in al, 71h

	mov rsp, rbp
	pop rbp
	ret

sys_readKey:
	push rbp
	mov rbp, rsp

	xor rax, rax
.loop:
	in al, 0x64      ; Read 8042 status register. Can be read at any time.
	and al, 0x01     ; Output register 60h should only be read IIF Bit 0 of status port is set to 1.
	cmp al, 0
	je .loop
	in al, 0x60

	mov rsp, rbp
	pop rbp
	ret

cpuVendor:
	push rbp
	mov rbp, rsp

	push rbx

	mov rax, 0
	cpuid


	mov [rdi], ebx
	mov [rdi + 4], edx
	mov [rdi + 8], ecx

	mov byte [rdi+13], 0

	mov rax, rdi

	pop rbx

	mov rsp, rbp
	pop rbp
	ret
