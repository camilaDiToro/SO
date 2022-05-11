GLOBAL sys_write
GLOBAL sys_read
GLOBAL sys_time
GLOBAL sys_close
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
GLOBAL sys_realloc

sys_read:
    mov rax, 0
    int 80h
    ret

sys_write:
    mov rax, 1
    int 80h
    ret

sys_time:
    mov rax, 2
    int 80h
    ret

sys_close:
    mov rax, 3
    int 80h
    ret

sys_clear:
    mov rax, 4
    int 80h
    ret

sys_printmem:
    mov rax, 8
    int 80h
    ret

sys_date:
    mov rax, 10
    int 80h
    ret

sys_inforeg:
    mov rax, 11
    int 80h
    ret

sys_malloc:
    mov rax, 12
    int 80h
    ret

sys_free:
    mov rax, 13
    int 80h
    ret

sys_realloc:
    mov rax, 14
    int 80h
    ret