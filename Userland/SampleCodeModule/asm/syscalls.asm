GLOBAL sys_read
GLOBAL sys_write
GLOBAL sys_close

GLOBAL sys_clearScreen

GLOBAL sys_millis
GLOBAL sys_time
GLOBAL sys_date

GLOBAL sys_malloc
GLOBAL sys_free
GLOBAL sys_realloc
GLOBAL sys_memState

GLOBAL sys_getPid
GLOBAL sys_createProcess
GLOBAL sys_exit
GLOBAL sys_block
GLOBAL sys_unblock
GLOBAL sys_yield
GLOBAL sys_killProcess
GLOBAL sys_nice
GLOBAL sys_listProcesses
GLOBAL sys_waitpid

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

%macro syscall 1
    mov rax, %1
    mov r10, rcx
    int 80h
    ret
%endmacro

sys_read: syscall 0x00
sys_write: syscall 0x01
sys_close: syscall 0x02

sys_clearScreen: syscall 0x10

sys_millis: syscall 0x20
sys_time: syscall 0x21
sys_date: syscall 0x22

sys_malloc: syscall 0x30
sys_free: syscall 0x31
sys_realloc: syscall 0x32
sys_memState: syscall 0x33

sys_getPid: syscall 0x40
sys_createProcess: syscall 0x41
sys_exit: syscall 0x42
sys_block: syscall 0x43
sys_unblock: syscall 0x44
sys_yield: syscall 0x45
sys_killProcess: syscall 0x46
sys_nice: syscall 0x47
sys_listProcesses: syscall 0x48
sys_waitpid: syscall 0x49

sys_pipe: syscall 0x50
sys_openPipe: syscall 0x51
sys_unlinkPipe: syscall 0x52
sys_listPipes: syscall 0x53

sys_openSem: syscall 0x60
sys_closeSem: syscall 0x61
sys_postSem: syscall 0x63
sys_waitSem: syscall 0x64
sys_listSemaphores: syscall 0x65