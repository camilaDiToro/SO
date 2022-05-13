GLOBAL sys_write
GLOBAL sys_read
GLOBAL sys_time
GLOBAL sys_close
GLOBAL sys_clear
GLOBAL sys_date
GLOBAL sys_printmem
GLOBAL sys_inforeg
GLOBAL sys_malloc
GLOBAL sys_free
GLOBAL sys_realloc
GLOBAL sys_pipe
GLOBAL sys_kill
GLOBAL sys_block
GLOBAL sys_unblock
GLOBAL sys_create
GLOBAL sys_yield
GLOBAL sys_pid
GLOBAL sys_nice
GLOBAL sys_exit

sys_read:
    mov rax, 0x00
    int 80h
    ret

sys_write:
    mov rax, 0x01
    int 80h
    ret

sys_time:
    mov rax, 0x02
    int 80h
    ret

sys_close:
    mov rax, 0x03
    int 80h
    ret

sys_clear:
    mov rax, 0x04
    int 80h
    ret

sys_kill:
    mov rax, 0x05
    int 80h
    ret

sys_block:
    mov rax, 0x06
    int 80h 
    ret

sys_unblock:
    mov rax, 0x07
    int 80h
    ret

sys_printmem:
    mov rax, 0x08
    int 80h
    ret

sys_create:
    mov rax, 0x09
    int 80h
    ret

sys_date:
    mov rax, 0x0A
    int 80h
    ret

sys_inforeg:
    mov rax, 0x0B
    int 80h
    ret

sys_malloc:
    mov rax, 0x0C
    int 80h
    ret

sys_free:
    mov rax, 0x0D
    int 80h
    ret

sys_realloc:
    mov rax, 0x0E
    int 80h
    ret

sys_yield:
    mov rax, 0x0F
    int 80h 
    ret

sys_pid
    mov rax, 0x10
    int 80h
    ret

sys_nice
    mov rax, 0x11
    int 80h
    ret

sys_exit
    mov rax, 0x12
    int 80h
    ret

sys_pipe:
    mov rax, 0x16
    int 80h
    ret