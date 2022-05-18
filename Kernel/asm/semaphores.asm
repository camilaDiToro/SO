GLOBAL _lock
GLOBAL _unlock

; returns 0 if the lock ocurrs
; 1 otherwise
_lock:
    mov rax, 0
    mov al, 1
    xchg al, [rdi]
    ret
