global drone_func
extern printf
extern CORS
extern resume
section .rodata
    str_format: db "%s", 0
%macro printInfo 1
jmp %%skip_print
section .data
    %%str_to_print: db %1, 10, 0
section .text
    %%skip_print:
    pushad
    push %%str_to_print
    push str_format
    call printf
    add esp, 8
    popad
%endmacro



section .text

drone_func:
    printInfo "drone"
    mov ebx, dword [CORS]
    call resume

