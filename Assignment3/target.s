global target_func
global createTarget
extern printf
extern CORS
extern resume
extern get_scaled_random
extern RAND
extern PREV

section .rodata
    str_format: db "%s", 0


global target_X
global target_Y

section .bss
    target_X: resd 1
    target_Y: resd 1


section .text
target_func:
    .target_loop:
    call createTarget
    mov ebx, dword [PREV]
    call resume
    jmp .target_loop
    

createTarget:
    push ebp
    mov ebp, esp
    pushad

    push 100
    call get_scaled_random
    add esp, 4


    fld dword [RAND]
    fstp dword [target_X]
    
    push 100
    call get_scaled_random
    add esp, 4

    fld dword [RAND]
    fstp dword [target_Y]

    popad
    pop ebp
    ret




