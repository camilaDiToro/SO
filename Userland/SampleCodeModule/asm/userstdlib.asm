GLOBAL sys_write
GLOBAL sys_read
GLOBAL sys_time
GLOBAL sys_tick
GLOBAL sys_clear
GLOBAL sys_restartCursor
GLOBAL sys_divide
GLOBAL sys_uniqueWindow
GLOBAL invalidOp

GLOBAL divideByZero

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

; Retrivied from https://mudongliang.github.io/x86/html/file_module_x86_id_318.html
invalidOp:
    push rbp
    mov rbp, rsp

    UD2

    mov rsp, rbp
    pop rbp
    ret

divideByZero:
    push rbp
    mov rbp, rsp

    mov rdx, 1
    mov rax, 0
    div rax

    mov rsp, rbp
    pop rbp
    ret