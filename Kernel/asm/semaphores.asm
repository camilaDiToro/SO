GLOBAL _spin_lock
GLOBAL _unlock

; returns 0 if the lock ocurrs
; loop otherwise
_spin_lock:
  mov rax, 0
  mov al, 1
  xchg al, [rdi]
  cmp al, 0
  jne _spin_lock
  ret


_unlock:
    mov byte [rdi], 0
    ret