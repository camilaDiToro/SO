GLOBAL invalidOp
GLOBAL divideByZero

; Retrivied from https://mudongliang.github.io/x86/html/file_module_x86_id_318.html
invalidOp:
    UD2
    ret

divideByZero:
    mov rdx, 1
    mov rax, 0
    div rax
    ret