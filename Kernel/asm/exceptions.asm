GLOBAL print_registers
GLOBAL give_control_to_user
EXTERN print
EXTERN printRegisterFormat
EXTERN newLine
EXTERN main
EXTERN getStackBase
section .text

%macro pushRegisters 0
    push rax
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
%endmacro

%macro popRegisters 0
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
    pop rax
%endmacro
give_control_to_user:
    call getStackBase	        ; Get thet stack address
	mov rsp, rax				; Set up the stack with the returned address
	call main
 
  ; Retrieved from https://os.phil-opp.com/handling-exceptions/
  ; The function recieves the following stack layout:
  ; SS  RSP  RFLAGS CS RIP RBP from the context that generates the exception.
  ; RAX RBX RCX RDX RBP RDI RSI R8 R9 R10 R11 R12 R13 R14 R15 (pushRegisters)

  ; TO DO: Debug with GDB to check if printing is ok.
  print_registers:
      push rbp
      mov rbp, rsp
      pushRegisters

      xor r10, r10
      xor rbx, rbx
      sub rbx, length

  .loop_registers:
      sub r10, 8
      mov rdi, [registersEnd+r10]
      call print
      mov rdi, [rbp+r10]
      call printRegisterFormat
      mov rdi, 0
      call newLine
      cmp r10, rbx
      jne .loop_registers

      xor r10, r10
  .loop_registers2:
      mov rdi, [registers2+r10]
      call print
      add r10, 8
      mov rdi, [rbp+r10]
      call printRegisterFormat
      mov rdi, 0
      call newLine
      cmp r10, length2
      jne .loop_registers2

      popRegisters
      mov rsp, rbp
      pop rbp
      ret

section .data
      segmentSS db " SS = ", 0
      rflags db " RFLAGS = ", 0
      segmentCS db " CS = ", 0
      registerRIP db " RIP = ", 0
      registerRAX db " RAX= ", 0
      registerRBX db " RBX= ", 0
      registerRCX db " RCX= ", 0
      registerRDX db " RDX= ", 0
      registerRBP db " RBP= ", 0
      registerRDI db " RDI= ", 0
      registerRSI db " RSI= ", 0
      registerRSP db " RSP= ", 0
      registerR8 db  " R8 = ", 0
      registerR9 db  " R9 = ", 0
      registerR10 db " R10= ", 0
      registerR11 db " R11= ", 0
      registerR12 db " R12= ", 0
      registerR13 db " R13= ", 0
      registerR14 db " R14= ", 0
      registerR15 db " R15= ", 0
      registers dq  registerR15, registerR14, registerR13, registerR12, registerR11, registerR10,registerR9, registerR8, registerRSI, registerRDI, registerRBP, registerRDX, registerRCX, registerRBX, registerRAX
      registersEnd equ $
      length equ registersEnd-registers
      registers2 dq registerRIP, segmentCS, rflags, registerRSP, segmentSS
      length2 equ $-registers2