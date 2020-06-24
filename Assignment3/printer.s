global print_func
extern printf
extern CORS
extern resume
extern target_X
extern target_Y
extern numCos

%macro printInfo 1
jmp %%skip_print
section .rodata
    %%str_to_print: db %1, 10, 0
section .text
    %%skip_print:
%endmacro

%macro printfM 2
    pushad
    push %2
    push %1
    call printf
    add esp, 8
    popad
%endmacro

%macro print_float_num 1
    pushad
    insert_float %1
    push float_format
    call printf
    add esp, 12
    popad
%endmacro

%macro insert_float 1
    pushad
    fld %1
    fstp qword [float_temp]    
    popad
    push dword [float_temp + 4]
    push dword [float_temp]  
%endmacro

%macro get_co 1
    push ecx
    mov ebx, dword [CORS]
    mov ecx, %1
    imul ecx, 12
    add ebx, ecx
    pop ecx
%endmacro

section .rodata
    str_format: db "%s", 0
    target_format: db "%.2f,%.2f", 10, 0
    drone_format: db "%d,%.2f,%.2f,%.2f,%.2f,%d", 10, 0
    int_format: db "%d", 10, 0 
    float_format: db "%.2f", 10, 0

section .bss
    float_temp: rest 1
    SCHEDULER_CO: equ 0
    TARGET_CO: equ 2
    STK_OFF: equ 4
    SCORE_OFF: equ 40
    X_OFF: equ 44
    Y_OFF: equ 48
    SPEED_OFF: equ 52
    HEADINGS_OFF:equ 56
    shouldStop_OFF: equ 8

section .text
print_func:
    .endless_loop:
    finit
    insert_float dword [target_Y]
    insert_float dword [target_X]
    push target_format
    call printf
    add esp, 20
    mov ecx, 3
    mov eax, dword [numCos]
    .print_loop:
    cmp ecx, eax
    je .end_print_loop
    get_co ecx
    
    cmp dword [ebx + shouldStop_OFF], 1
    je .next_loop

    mov ebx, [ebx + STK_OFF]
    pushad
    push dword [ebx + SCORE_OFF]
    insert_float dword [ebx + SPEED_OFF]
    insert_float dword [ebx + HEADINGS_OFF]
    insert_float dword [ebx + Y_OFF]
    insert_float dword [ebx + X_OFF]    
    mov edx, ecx
    sub edx, 2
    push edx
    push drone_format
    call printf
    add esp, 44
    popad
    
    .next_loop:
    inc ecx
    jmp .print_loop
    .end_print_loop:
    get_co SCHEDULER_CO
    call resume
    jmp .endless_loop

