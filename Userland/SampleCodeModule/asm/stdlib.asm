GLOBAL sys_write
GLOBAL sys_read

sys_write:
    push rbp
    mov rbp, rsp
    mov rcx, 1
    int 80h
    mov rsp, rbp
    pop rbp
    ret

sys_read:
    push rbp
    mov rbp, rsp
    mov rcx, 0
    int 80h
    mov rsp, rbp
    pop rbp
    ret
