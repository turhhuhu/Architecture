extern malloc
extern calloc
extern schedule_func
extern drone_func
extern print_func
extern target_func
extern printf
global main
global end_co
global CORS
global CURR
global resume
global do_resume
    
section .rodata
    str_format: db "%s", 0
    format: db "%d", 10, 0
section .bss
    global CURR
    global SPT
    global SPMAIN
    global STKSZ
    CURR: resd 1
    SPT: resd 1 ; temporary stack pointer
    SPMAIN: resd 1 ; stack pointer of main
    STKSZ: equ 16*1024
    SCHEDULER_CO: equ 0
    CORS: resd 1
    numCos: resd 1

    STK_OFF: equ 4

;returns co-routine address in ebx
%macro get_co 1
    push ecx
    mov ebx, dword [CORS]
    mov ecx, %1
    imul ecx, 8
    add ebx, ecx
    pop ecx
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

section .text
main:
    push ebp
    mov ebp, esp
    mov dword [numCos], 5
    call initAllCors
    call start_schedule
    pop ebp
    ret

initAllCors:
    push ebp
    mov ebp, esp
    pushad

    mov ecx, dword [numCos]
    push ecx

    push ecx
    push 8
    call calloc
    add esp, 8
    
    pop ecx
    mov dword [CORS], eax

    dec ecx
    mov ebx, eax
    .cors_loop:
    cmp ecx, -1
    je .end_cors_loop
    cmp ecx, 0
    jne .not_scheduler
    alloc_co ecx, schedule_func
    jmp .init_co
    .not_scheduler:
    cmp ecx, 1
    jne .not_printer
    alloc_co ecx, print_func
    jmp .init_co
    .not_printer:
    cmp ecx, 2
    jne .not_target
    alloc_co ecx, target_func
    jmp .init_co
    .not_target:
    alloc_co ecx, drone_func
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
    mov edx, [CURR]
    mov [edx + STK_OFF], esp ; save current ESP
do_resume: ; load ESP for resumed co-routine
    mov esp, [ebx + STK_OFF]
    mov [CURR], ebx
    popad ; restore resumed co-routine state
    popfd
    ret ; "return" to resumed co-routine  