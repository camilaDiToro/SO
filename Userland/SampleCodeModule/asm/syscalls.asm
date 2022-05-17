GLOBAL sys_read
GLOBAL sys_write
GLOBAL sys_time
GLOBAL sys_close
GLOBAL sys_clear
GLOBAL sys_kill
GLOBAL sys_block
GLOBAL sys_unblock
GLOBAL sys_printmem
GLOBAL sys_createProcess
GLOBAL sys_date
GLOBAL sys_inforeg
GLOBAL sys_malloc
GLOBAL sys_free
GLOBAL sys_realloc
GLOBAL sys_yield
GLOBAL sys_getPid
GLOBAL sys_nice
GLOBAL sys_exit
GLOBAL sys_listProcesses
GLOBAL sys_pipe
GLOBAL sys_openPipe
GLOBAL sys_unlinkPipe
GLOBAL sys_listPipes
GLOBAL sys_openSem
GLOBAL sys_closeSem
GLOBAL sys_unlinkSem
GLOBAL sys_postSem
GLOBAL sys_waitSem
GLOBAL sys_listSemaphores

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

sys_createProcess:
    mov rax, 0x09
    mov r10, rcx
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

sys_getPid:
    mov rax, 0x10
    int 80h
    ret

sys_nice:
    mov rax, 0x11
    int 80h
    ret

sys_exit:
    mov rax, 0x12
    int 80h
    ret

sys_listProcesses:
    mov rax, 0x13
    int 80h
    ret

sys_pipe:
    mov rax, 0x16
    int 80h
    ret

sys_openPipe:
    mov rax, 0x17
    int 80h
    ret

sys_unlinkPipe:
    mov rax, 0x18
    int 80h
    ret

sys_listPipes:
    mov rax, 0x19
    int 80h
    ret

sys_openSem:
    mov rax, 0x20
    int 80h
    ret

sys_closeSem:
    mov rax, 0x21
    int 80h
    ret

sys_unlinkSem:
    mov rax, 0x22
    int 80h
    ret

sys_postSem:
    mov rax, 0x23
    int 80h
    ret

sys_waitSem:
    mov rax, 0x24
    int 80h
    ret

sys_listSemaphores:
    mov rax, 0x25
    int 80h
    ret
