GLOBAL sys_write
GLOBAL sys_read
GLOBAL sys_time
GLOBAL sys_tick
GLOBAL sys_clear
GLOBAL sys_restartCursor
GLOBAL sys_divide
GLOBAL sys_uniqueWindow
GLOBAL invalidOp
GLOBAL sys_setScreen
GLOBAL sys_date
GLOBAL inforeg
EXTERN my_printf 
GLOBAL sys_printmem
GLOBAL divideByZero

%macro pushState 0
	push rbx
	push rcx
	push rdx
	push rbp
	push rdi
	push rsi
	push r8
	push r9
	push r10
	push r11
	push r12
	push r13
	push r14
	push r15
	push rax
%endmacro

%macro popState 0
	pop rax
	pop r15
	pop r14
	pop r13
	pop r12
	pop r11
	pop r10
	pop r9
	pop r8
	pop rsi
	pop rdi
	pop rbp
	pop rdx
	pop rcx
	pop rbx
%endmacro
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

sys_printmem:
    push rbp
    mov rbp, rsp
    
    mov r8, 8
    int 80h
    
    mov rsp, rbp
    pop rbp
    ret

    ; When the stack frame is built, the stack layout will be the following one:
    
    ; RBP   -> stack frame
    ; RAX
    ; R15
    ; R14
    ; R13
    ; R12
    ; R11
    ; R10
    ; R9
    ; R8
    ; RSI
    ; RDI
    ; RBP
    ; RDX
    ; RCX
    ; RBX
    ; RIP   -> next instruction   <------ RSP to print points here. 
    ; RBP   
    
inforeg:
      pushState
      push rbp
      mov rbp, rsp

      xor r10, r10

  .loop_registers:
      
      mov rdi, [registers+r10]
      add r10, 8
      mov rsi, [rbp+r10]
      call my_printf
      
      cmp r10, length
      jne .loop_registers

      mov rdi, registerRSP
      mov rsi, rbp
      add rsi, 136         ; 16 + 15*8 -> calculate the value of RSP.
      call my_printf

      mov rsp, rbp
      pop rbp
      popState
      ret

section .data
      registerRIP db " RIP = %x", 10 ,0
      registerRAX db " RAX = %x", 10, 0
      registerRBX db " RBX = %x", 10, 0
      registerRCX db " RCX = %x", 10, 0
      registerRDX db " RDX = %x", 10 ,0
      registerRBP db " RBP = %x", 10 ,0
      registerRDI db " RDI = %x", 10 ,0
      registerRSI db " RSI = %x", 10 ,0
      registerR8 db  " R8 = %x", 10 ,0
      registerR9 db  " R9 = %x", 10 ,0
      registerR10 db " R10 = %x", 10 ,0
      registerR11 db " R11 = %x", 10 ,0
      registerR12 db " R12 = %x", 10 ,0
      registerR13 db " R13 = %x", 10 ,0
      registerR14 db " R14 = %x", 10 ,0
      registerR15 db " R15 = %x", 10 ,0
      registers dq  registerRAX, registerR15, registerR14, registerR13, registerR12, registerR11, registerR10, registerR9, registerR8, registerRSI, registerRDI, registerRBP, registerRDX, registerRCX, registerRBX, registerRIP
      length equ $-registers
      registerRSP db " RSP = %x", 10 ,0