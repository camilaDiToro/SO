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
    mov r8, 0
    int 80h
    ret

sys_write:
    mov r8, 1
    int 80h
    ret

sys_time:
    mov r8, 2
    int 80h
    ret

sys_tick:
    mov r8, 3
    int 80h
    ret

sys_clear:
    mov r8, 4
    int 80h
    ret

sys_restartCursor:
    mov r8, 5
    int 80h
    ret

sys_divide:
    mov r8, 6
    int 80h
    ret

sys_uniqueWindow:
    mov r8, 7
    int 80h
    ret

sys_setScreen:
    mov r8, 9
    int 80h
    ret

sys_date:
    mov r8, 10
    int 80h
    ret

sys_inforeg:
    mov r8, 11
    int 80h
    ret

sys_malloc:
    mov r8, 12
    int 80h
    ret

sys_free:
    mov r8, 13
    int 80h
    ret

sys_printmem:
    mov r8, 8
    int 80h
    ret
