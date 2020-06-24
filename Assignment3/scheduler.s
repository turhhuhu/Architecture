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
    currDroneI: dd 0
section .bss
    SCHEDULER_CO: equ 0
    PRINT_CO: equ 1
    TARGET_CO: equ 2
    droneNum: resd 1
    STK_OFF: equ 4
    shouldStop_OFF: equ 8
    SCORE_OFF: equ 40
    numActiveDrones: resd 1
    M: resd 1
%macro printInfo 1
jmp %%skip_print
section .data
    %%str_to_print: db %1 , 0
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
    mov ebx, dword [numCos]
    sub ebx, 3
    mov dword [numActiveDrones], ebx
    .scheduler_loop:
    ;printfM int_format, dword [numActiveDrones]
    ;printfM int_format, dword [currI]
    mov ebx, dword [numCos]
    sub ebx, 3
    Mod dword [currDroneI], ebx
    mov ecx, edx
    add ecx, 3
    get_co ecx
    cmp dword [ebx + shouldStop_OFF], 1
    jne .active_drone
    inc dword [currDroneI]
    jmp .scheduler_loop
    .active_drone:
    call resume
    Mod dword [currI], dword [K]
    cmp edx, 0
    jne .not_print
    get_co PRINT_CO
    call resume
    .not_print:
    mov edx, 0
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
    call find_M_drone
    get_co eax
    mov dword [ebx + shouldStop_OFF], 1
    dec dword [numActiveDrones]
    .not_turn_off:
    ;printfM int_format, dword [numActiveDrones]
    cmp dword [numActiveDrones], 1
    je .end_game
    inc dword [currI]
    inc dword [currDroneI]
    jmp .scheduler_loop

    .end_game:
    mov ecx, dword [numCos]
    dec ecx
    .first_loop:
    get_co ecx
    cmp dword [ebx + shouldStop_OFF], 1
    je .next_loop
    jmp .end_first_loop
    .next_loop:
    dec ecx
    jmp .first_loop
    .end_first_loop:
    sub ecx, 2

    printInfo "The winner is drone:"
    printfM int_format, ecx
    jmp end_co

find_M_drone:
    push ebp
    mov ebp, esp
    push ebx
    push ecx
    push edx


    mov ecx, dword [numCos]
    dec ecx
    .first_loop:
    get_co ecx
    cmp dword [ebx + shouldStop_OFF], 1
    je .next_loop
    mov ebx, [ebx + STK_OFF]
    mov eax, [ebx + SCORE_OFF]
    mov dword [M], eax
    mov dword [droneNum], ecx
    jmp .end_first_loop
    .next_loop:
    dec ecx
    jmp .first_loop
    .end_first_loop:


    dec ecx
    .low_loop:
    cmp ecx, 2
    je .end_low_loop
    get_co ecx
    cmp dword [ebx + shouldStop_OFF], 1
    je .no_swap_M
    get_co ecx
    mov ebx, [ebx + STK_OFF]
    mov eax, [ebx + SCORE_OFF]
    cmp dword [M], eax
    jbe .no_swap_M
    mov dword [M], eax
    mov dword [droneNum], ecx
    .no_swap_M:
    dec ecx
    jmp .low_loop
    .end_low_loop:
    
    mov eax, dword [droneNum]
    

    pop edx
    pop ecx
    pop ebx
    pop ebp
    ret
