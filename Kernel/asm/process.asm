GLOBAL _createProcessContext

;                                 rdi                 rsi          rdx                      rcx
; void* _createProcessContext(int argc, const char* argv[], void * rsp, TProcessEntryPoint entryPoint);
_createProcessContext:
    
    push rbp
    mov rbp, rsp
    
    mov rsp, rdx ; rsp 
    push 0x0     ; SS
    push rdx     ; RSP
    push 0x202   ; RFLAGS
    push 0x8     ; CS
    push rcx     ; entryPoint
    
    push 0x01    ; RBX
    push 0x02    ; RCX
    push 0x03    ; RDX
    push rdx     ; RBP (initally, RBP = RSP)
	push rdi     ; RDI --> argc
	push rsi     ; RSI --> argv
	push 0x08    ; R8   
	push 0x09    ; r9
	push 0x0A    ; r10
	push 0x0B    ; r11
	push 0x0C    ; r12
	push 0x0D    ; r13
	push 0x0E    ; r14
	push 0x0F    ; r15    
	push 0x10    ; rax

    mov rax, rsp ; return the currentRSP of the process 

    mov rsp, rbp
    pop rbp
    ret 