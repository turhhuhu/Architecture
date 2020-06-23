global target_func
extern printf
extern CORS
extern resume
section .rodata
    str_format: db "%s", 0


global target_X
global target_Y

section .bss
    target_X: resd 1
    target_Y: resd 1

section .data
section .text
%macro printInfo 1
jmp %%skip_print
    %%str_to_print: db %1, 10, 0
    %%skip_print:
    pushad
    push %%str_to_print
    push str_format
    call printf
    add esp, 8
    popad
%endmacro
target_func:
    printInfo "target"
    mov ebx, dword [CORS]
    call resume






