GLOBAL sys_write
GLOBAL sys_read
GLOBAL sys_time
GLOBAL sys_tick
GLOBAL sys_clear
GLOBAL sys_restartCursor
GLOBAL sys_divide
GLOBAL sys_uniqueWindow
GLOBAL sys_setScreen
GLOBAL sys_date
GLOBAL sys_printmem
GLOBAL sys_inforeg
GLOBAL sys_malloc
GLOBAL sys_free

sys_read:
    push rbp
    mov rbp, rsp
    mov r8, 0
    int 80h
    mov rsp, rbp
    pop rbp
    ret

sys_write:
    push rbp
    mov rbp, rsp
    mov r8, 1
    int 80h
    mov rsp, rbp
    pop rbp
    ret

sys_time:
    push rbp
    mov rbp, rsp
    mov r8, 2
    int 80h
    mov rsp, rbp
    pop rbp
    ret

sys_tick:
    push rbp
    mov rbp, rsp
    mov r8, 3
    int 80h
    mov rsp, rbp
    pop rbp
    ret

sys_clear:
    push rbp
    mov rbp, rsp
    mov r8, 4
    int 80h
    mov rsp, rbp
    pop rbp
    ret

sys_restartCursor:
    push rbp
    mov rbp, rsp
    mov r8, 5
    int 80h
    mov rsp, rbp
    pop rbp
    ret

sys_divide:
    push rbp
    mov rbp, rsp
    mov r8, 6
    int 80h
    mov rsp, rbp
    pop rbp
    ret

sys_uniqueWindow:
    push rbp
    mov rbp, rsp
    mov r8, 7
    int 80h
    mov rsp, rbp
    pop rbp
    ret

sys_setScreen:
    push rbp
    mov rbp, rsp
    mov r8, 9
    int 80h
    mov rsp, rbp
    pop rbp
    ret

sys_date:
    push rbp
    mov rbp, rsp
    mov r8, 10
    int 80h
    mov rsp, rbp
    pop rbp
    ret

sys_inforeg:
    push rbp
    mov rbp, rsp
    mov r8, 11
    int 80h
    mov rsp, rbp
    pop rbp
    ret

sys_malloc:
    push rbp
    mov rbp, rsp
    mov r8, 12
    int 80h
    mov rsp, rbp
    pop rbp
    ret

sys_free:
    push rbp
    mov rbp, rsp
    mov r8, 13
    int 80h
    mov rsp, rbp
    pop rbp
    ret

sys_printmem:
    push rbp
    mov rbp, rsp
    
    mov r8, 8
    int 80h
    
    mov rsp, rbp
    pop rbp
    ret
