global schedule_func
extern printf
extern end_co
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

%macro get_co 1
    push ecx
    mov ebx, dword [CORS]
    mov ecx, %1
    imul ecx, 8
    add ebx, ecx
    pop ecx
%endmacro

section .text
schedule_func:
    printInfo "scheduler"
    mov ecx, 4
    .loop:
    cmp ecx, -1
    je .end_loop
    get_co ecx
    call resume
    dec ecx
    jmp .loop
    .end_loop:
    jmp end_co

