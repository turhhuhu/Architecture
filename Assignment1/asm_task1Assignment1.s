section .rodata
    format: db "%d", 10, 0

section .text 
    global assFunc
    extern printf
    extern c_checkValidity
    
assFunc:
    push ebp
    mov ebp, esp
    push dword [ebp + 12]
    push dword [ebp + 8]
    call c_checkValidity
    add esp, 8
    cmp dword eax, 1
    jnz smaller
    mov dword ecx, [ebp + 8]
    sub dword ecx, [ebp + 12]
    jmp print
    smaller:
        mov dword ecx, [ebp + 8]
        add dword ecx, [ebp + 12]
    print:
    push ecx
    push format
    call printf
    add esp, 8
    mov esp, ebp
    pop ebp
    ret


