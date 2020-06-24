extern malloc
extern calloc
extern schedule_func
extern drone_func
extern print_func
extern target_func
extern printf
extern sscanf
extern createTarget
extern free
global main
global end_co
global CORS
global CURR
global resume
global do_resume
global float_temp
global CURR
global SPT
global SPMAIN
global STKSZ
global board_size_max
global board_size_min
global get_scaled_random
global RAND
global PREV
global numCos
global R
global K
global Max_distance

section .rodata
    str_format: db "%s", 0
    int_format: db "%d", 10, 0 
    float_format: db "%.2f", 10, 0
    float_input_format: db "%f", 0
    int_input_format: db "%d", 0
    MAX_INT: dd 65535
    board_size_max: dd 100.0
    board_size_min: dd 0.0

section .data
    SEED: dd 0
    RAND: dd 0
    float_temp: dd 0
    X_Coord: dd 0
    Y_Coord: dd 0
    speed: dd 0
    heading: dd 0
    R: dd 0
    K: dd 0
    Max_distance: dd 0
    numCos: dd 0
    CURR: dd 0
    PREV: dd 0
    SPT: dd 0 ; temporary stack pointer
    SPMAIN: dd 0 ; stack pointer of main
    CORS: dd 0
section .bss
    ; CURR: resd 1
    ; PREV: resd 1
    ; SPT: resd 1 ; temporary stack pointer
    ; SPMAIN: resd 1 ; stack pointer of main
    STKSZ: equ 16*1024
    SCHEDULER_CO: equ 0
    PRINTER_CO: equ 1
    TARGET_CO: equ 2
    ;CORS: resd 1
    ;numCos: resd 1
    STK_OFF: equ 4
    shouldStop_OFF: equ 8
    ; SEED: resd 1
    ; RAND: resd 1
    ; float_temp: resq 1
    ; X_Coord: resd 1
    ; Y_Coord: resd 1
    ; speed: resd 1
    ; heading: resd 1
    ; R: resd 1
    ; K: resd 1
    ; Max_distance: resd 1

;returns co-routine address in ebx
%macro get_co 1
    push ecx
    mov ebx, dword [CORS]
    mov ecx, %1
    imul ecx, 12
    add ebx, ecx
    pop ecx
%endmacro

%macro print_float_num 1
    pushad
    fld %1
    fstp qword [float_temp]    
    push dword [float_temp + 4]
    push dword [float_temp]    
    push float_format      
    call printf
    add esp, 12
    popad
%endmacro

%macro alloc_co 2
    pushad
    push ecx
    push STKSZ
    call malloc
    add esp, 4
    pop ecx
    add eax, STKSZ
    get_co %1
    mov dword [ebx], %2
    mov dword [ebx + STK_OFF], eax
    popad
%endmacro

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

%macro printfM 2
    pushad
    push %2
    push %1
    call printf
    add esp, 8
    popad
%endmacro

%macro sscanfM 3
    push eax
    push %1
    push %2
    push %3
    call sscanf
    add esp, 12
    pop eax
%endmacro

section .text
main:
    push ebp
    mov ebp, esp
    mov eax, dword [ebp + 8]
    cmp eax, 6
    jae .no_err
    printInfo "not enough arguments!"
    jmp .end
    .no_err:
    mov eax, dword [ebp + 12]
    sscanfM numCos, int_input_format, dword [eax + 4]
    add dword [numCos], 3
    
    sscanfM R, int_input_format, dword [eax + 8]
    sscanfM K, int_input_format, dword [eax + 12]
    sscanfM Max_distance, float_input_format, dword [eax + 16]
    sscanfM SEED, int_input_format, dword [eax + 20]

    call initAllCors
    call start_schedule
    call freeAllCors
    .end:
    pop ebp
    ret


initDrone:
    push ebp
    mov ebp, esp
    pushad
    mov ecx, [ebp + 8]


    push 100
    call get_scaled_random
    add esp, 4
    mov dword [X_Coord], eax

    push 100
    call get_scaled_random
    add esp, 4
    mov dword [Y_Coord], eax

    push 100
    call get_scaled_random
    add esp,4
    mov dword [speed], eax

    push 360
    call get_scaled_random
    add esp, 4
    mov dword [heading], eax
    

    get_co ecx
    mov dword [ebx + shouldStop_OFF], 0
    mov dword [SPT], esp
    mov esp, dword [ebx + STK_OFF]
    push dword [heading]
    push dword [speed]
    push dword [Y_Coord]
    push dword [X_Coord]
    push 0
    mov dword [ebx + STK_OFF], esp
    mov esp, dword [SPT]
    popad
    pop ebp
    ret
    
initAllCors:
    push ebp
    mov ebp, esp
    pushad

    mov ecx, dword [numCos]
    push ecx

    push ecx
    push 12
    call calloc
    add esp, 8
    
    pop ecx
    mov dword [CORS], eax

    dec ecx
    mov ebx, eax
    .cors_loop:
    cmp ecx, -1
    je .end_cors_loop
    cmp ecx, SCHEDULER_CO
    jne .not_scheduler
    alloc_co ecx, schedule_func
    jmp .init_co
    .not_scheduler:
    cmp ecx, PRINTER_CO
    jne .not_printer
    alloc_co ecx, print_func
    jmp .init_co
    .not_printer:
    cmp ecx, TARGET_CO
    jne .not_target
    call createTarget
    alloc_co ecx, target_func
    jmp .init_co
    .not_target:
    alloc_co ecx, drone_func
    push ecx
    call initDrone
    add esp, 4
    .init_co:
    push ecx
    call initCo
    add esp, 4
    dec ecx
    jmp .cors_loop
    .end_cors_loop:
    popad
    pop ebp
    ret
    
;maybe problematic
initCo:
    push ebp
    mov ebp, esp
    pushad

    mov ecx, [ebp+8] ; get co-routine ID number
    get_co ecx ; get pointer to COi struct
    mov eax, [ebx] ; get initial EIP value – pointer to COi function
    mov [SPT], esp ; save ESP value
    mov esp, [ebx + STK_OFF] ; get initial ESP value – pointer to COi stack
    push eax ; push initial “return” address
    pushfd ; push flags
    pushad ; push all other registers
    mov [ebx + STK_OFF], esp ; save new SPi value (after all the pushes)
    mov esp, [SPT]

    popad
    pop ebp
    ret

freeAllCors:
    push ebp
    mov ebp, esp
    pushad
    mov ecx, dword [numCos]
    dec ecx
    .free_loop:
    cmp ecx, -1
    je .end_free_loop
    get_co ecx
    mov ebx, dword [ebx + STK_OFF]
    cmp ecx, 2
    jbe .not_drone
    add ebx, 20
    .not_drone:
    add ebx, 40
    sub ebx, STKSZ
    pushad
    push ebx
    call free
    add esp, 4
    popad
    dec ecx
    jmp .free_loop
    .end_free_loop:
    ; get_co ecx
    ; mov ebx, dword [ebx + STK_OFF]
    ; sub ebx, dword [temp]
    ; printfM int_format, ebx
    push dword [CORS]
    call free
    add esp, 4
    popad
    pop ebp
    ret

start_schedule:
    push ebp
    mov ebp, esp

    pushad ; save registers of main ()
    mov [SPMAIN], esp ; save ESP of main ()
    get_co SCHEDULER_CO ; gets a pointer to a scheduler struct
    jmp do_resume ; resume a scheduler co-routine
    end_co:        
    mov esp, [SPMAIN]
    popad
    pop ebp
    ret


resume: ; save state of current co-routine
    pushfd
    pushad
    mov edx, dword [CURR]
    mov dword [PREV], edx
    mov dword [edx + STK_OFF], esp ; save current ESP
do_resume: ; load ESP for resumed co-routine
    mov esp, dword [ebx + STK_OFF]
    mov dword [CURR], ebx
    popad ; restore resumed co-routine state
    popfd
    ret ; "return" to resumed co-routine  


next_LFSR_bit:
    push ebx
    push ecx
    push edx

    mov eax, 0
    mov ax, [SEED]
    mov ebx, 0

    mov bx, ax

    shr ax, 2
    xor bx, ax

    shr ax, 1
    xor bx, ax

    shr ax, 2
    xor bx, ax

    shr word [SEED], 1
    shl bx, 15
    or word [SEED], bx

    pop edx
    pop ecx
    pop ebx
    ret

get_scaled_random:
    push ebp
    mov ebp, esp
    push ebx
    push ecx
    push edx

    mov ecx, 16
    mov ebx, 0
    .generate_num_loop:
    cmp ecx, 0
    je .end_loop
    call next_LFSR_bit
    dec ecx
    jmp .generate_num_loop
    .end_loop:

    finit
    fild dword [SEED] ; st(1)
    fild dword [MAX_INT] ; st(0)
    fdivp; st(1) / st(0)
    fild dword [ebp + 8]
    fmulp
    fstp dword [RAND]
    mov eax, dword [RAND]

    pop edx
    pop ecx
    pop ebx
    pop ebp
    ret
