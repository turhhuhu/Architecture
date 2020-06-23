global schedule_func
extern printf
extern end_co
extern CORS
extern resume
extern R
extern K
extern numCos

section .rodata
    str_format: db "%s", 0
    int_format: db "%d", 10, 0 
    float_format: db "%.2f", 10, 0

section .data
    currI: dd 0
section .bss
    SCHEDULER_CO: equ 0
    PRINT_CO: equ 1
    TARGET_CO: equ 2
    droneNum: resd 1
    STK_OFF: equ 4
    shouldStop_OFF: equ 8
    SCORE_OFF: equ 60
    numActiveDrones: resd 1
    M: resd 1
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
    imul ecx, 12
    add ebx, ecx
    pop ecx
%endmacro

%macro printfM 2
    pushad
    push %2
    push %1
    call printf
    add esp, 8
    popad
%endmacro

%macro Mod 2
    push eax
    push ebx
    push ecx

    mov edx, 0
    mov eax, dword %1
    mov ebx, dword %2
    div ebx
    
    pop ecx
    pop ebx
    pop eax
%endmacro

section .text

schedule_func:
    printInfo "scheduler"
    ; mov ecx, 4
    ; .loop:
    ; cmp ecx, 0
    ; je .end_loop
    ; get_co ecx
    ; call resume
    ; dec ecx
    ; jmp .loop
    ; .end_loop:
    mov ebx, dword [numCos]
    sub ebx, 3
    mov dword [numActiveDrones], ebx
    .scheduler_loop:
    printfM int_format, dword [currI]
    mov ebx, dword [numCos]
    sub ebx, 3
    Mod dword [currI], ebx
    mov ecx, edx
    add ecx, 3
    get_co ecx
    cmp dword [ebx + shouldStop_OFF], 1
    je .no_drone
    call resume
    .no_drone:
    Mod dword [currI], dword [K]
    cmp edx, 0
    jne .not_print
    get_co PRINT_CO
    call resume
    .not_print:
    mov eax, dword [currI]
    mov ebx, dword [numCos]
    sub ebx, 3
    div ebx
    Mod eax, dword [R]
    cmp edx, 0
    jne .not_turn_off
    mov ebx, dword [numCos]
    sub ebx, 3
    Mod dword [currI], ebx
    cmp edx, 0
    jne .not_turn_off
    printInfo "here"
    call find_M_drone
    printInfo "here1"
    get_co eax
    mov dword [ebx + shouldStop_OFF], 1
    dec dword [numActiveDrones]
    .not_turn_off:
    printfM int_format, dword [numActiveDrones]
    inc dword [currI]
    cmp dword [numActiveDrones], 1
    je end_co
    jmp .scheduler_loop

find_M_drone:
    push ebp
    mov ebp, esp
    push ebx
    push ecx
    push edx

    mov ecx, dword [numCos]
    dec ecx
    
    get_co ecx
    mov ebx, [ebx + STK_OFF]
    mov eax, [ebx + SCORE_OFF]
    mov dword [M], eax
    mov dword [droneNum], ecx
    
    dec ecx

    .low_loop:
    cmp ecx, 2
    je .end_low_loop

    get_co ecx
    mov ebx, [ebx + STK_OFF]
    mov eax, [ebx + SCORE_OFF]
    cmp dword [M], eax
    jb .no_swap_M
    mov dword [M], eax
    mov dword [droneNum], ecx
    .no_swap_M:
    
    dec ecx
    jmp .low_loop
    .end_low_loop:
    
    mov eax, dword [droneNum]
    printInfo "here2"

    pop edx
    pop ecx
    pop ebx
    pop ebp
    ret
