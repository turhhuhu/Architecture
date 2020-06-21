    STKSZ equ 16*1024

section .text
    align 16
    global schedulerCor
	global resume
	global get_random
	global distance
	global beta
    global t_x
    global t_y
    global target
    global double_for_printing
    global printer_drone_format_string
    global printer_target_format_string
    global N
    global droneArr
    global d_x
    global d_y
    global d_angle
    global d_points
    global finishCond
    global corArr
    global K
    global printerCor
    global end_co
    global ok_print_format_string
    global droneIndex
    global createTarget
    global drone_winner_format_string
    global T
    global targetCor
    global scaledRand
    global poiner_print_format_string
    global helperInt
    global float_print_format_string
    global helperFloat

section .data
    align 16
    ;;PROPERTIES {
    d_x equ 0
    d_y equ 4
    d_angle equ 8
    d_points equ 12
	CODEP equ 0
	FLAGSP equ 4
	SPP equ 8
    t_x equ 0
    t_y equ 4
    ;;END PROPERTIES }
    ;;FROMAT STRINGS {
    sscanf_int_format_string: db "%d", 0
    sscanf_float_format_string: db "%f", 0
    int_print_format_string: db "%d", 10, 0
    int_hexa_print_format_string: db "%#X", 10, 0
    poiner_print_format_string: db "%p", 10, 0
    float_print_format_string: db "%.2f", 10 ,0
    string_print_format_string: db "%s", 10, 0
    ok_print_format_string: db "OK %d", 10, 0
    printer_target_format_string: db "%.2f,%.2f", 10, 0
    printer_drone_format_string: db "%d,%.2f,%.2f,%.2f,%d", 10, 0
    drone_winner_format_string: db "Drone id %d: I am a winner", 10, 0
    ;;END FORMAT STRINGS }
    MAXINT16: dd 65535

section .bss
    align 16
    ;;ARGUMENTS {
    N: resd 1
    T: resd 1
    K: resd 1
    beta: resd 1
    distance: resd 1
    seed: resd 1
    ;;END ARGUMENTS }
    currRand: resd 1
    scaledRand: resd 1
    double_for_printing: resq 1
    ;;HELPERS {
    helperInt: resd 1
    helperFloat: resd 1 
    ;;END HELPERS }
    droneArr: resd 1
    target: resd 1
    corArr: resd 1
    schedulerCor: resd 1
    targetCor: resd 1
    printerCor: resd 1
    dronesStack: resd 1
    schedulerStack: resd 1
    targetStack: resd 1
    printerStack: resd 1
    droneIndex: resd 1
    CURR:	resd 1
    SPT:	resd 1
    SPMAIN:	resd 1

section .text
	align 16
	global main

    extern droneFunc
    extern schedulerFunc
    extern printerFunc
    extern targetFunc

	extern printf
    extern sscanf
	extern malloc
	extern free
	extern fgets

;; string_to_int(int *i, string s)
%macro string_to_int 2
    pushad
    push %1
    push sscanf_int_format_string
    push %2
    call sscanf
    add esp, 12
    popad
%endmacro

;; string_to_float(float *f, string s)
%macro string_to_float 2
    pushad
    push %1
    push sscanf_float_format_string
    push %2
    call sscanf
    add esp, 12
    popad
%endmacro

;; print_int(int i)
%macro print_int 1
    pushad
    push %1
    push int_print_format_string
    call printf
    add esp, 8
    popad
%endmacro

;; print_int_hexa(int i)
%macro print_int_hexa 1
    pushad
    push %1
    push int_hexa_print_format_string
    call printf
    add esp, 8
    popad
%endmacro

;; print_pointer(int i)
%macro print_pointer 1
    pushad
    push %1
    push poiner_print_format_string
    call printf
    add esp, 8
    popad
%endmacro

;; print_float(float f)
%macro print_float 1
    pushad
    fld %1
    fstp qword [double_for_printing]    ; convert float to double

    push dword [double_for_printing+4]
    push dword [double_for_printing]    ; put double (8 bytes) on stack
    push float_print_format_string      ; printf expects double not float
    call printf
    add esp, 12
    popad
%endmacro

;; fild_through_reg(int i)
%macro fild_through_reg 1
    pushad
    mov eax, %1
    mov [helperInt], eax
    fild dword [helperInt]
    popad
%endmacro


main:
    push ebp
    mov ebp, esp

    mov ebx, [ebp+12]                       ; ebx = argv
    string_to_int N, dword [ebx+1*4]        ; convert args to ints and floats
    string_to_int T, dword [ebx+2*4]
    string_to_int K, dword [ebx+3*4]
    string_to_float beta, dword [ebx+4*4]
    string_to_float distance, dword [ebx+5*4]
    string_to_int seed, dword [ebx+6*4]

    mov eax, [seed]
    mov [currRand], eax                     ; init seed
    call init_structs
    call init_cors

    push dword [schedulerCor]
    call startCo
    end_co:
    mov esp, [SPMAIN]
    popad
    call free_structs
    call free_cors
    pop ebp
    mov eax, 1 ; system call 1 - sys_exit
    mov ebx, 0
    int 0x80


;; Init All Structs - target, drone array and drones.
init_structs:
    push ebp
    mov ebp, esp
    pushad
    
    ;; TARGET STRUCT
    call new_target
    mov [target], eax
    mov ebx, eax
    call createTarget

    ;;DRONE ARR
    pushad
    mov eax, [N]
    shl eax, 2                              ; eax = N * sizeof(drone*) = 4N
    push eax
    call malloc
    add esp, 4
    mov [droneArr], eax                     ; droneArr = malloc(4 * N)
    popad

    ;;DRONE ARR MEMBERS
    mov ecx, [droneArr]                     ; ecx = droneArr
    mov edx, 0                              ; edx = i
    start_init_drone_arr_loop:
    cmp edx, dword [N]                      ; for (edx = 0; edx < N; edx++)
    jz end_init_drone_arr_loop

    call new_drone                          ; eax = address of new drone
    mov [ecx+4*edx], eax                    ; droneArr[i] = new drone address

    mov ebx, eax                            ; ebx = address of drone
    ;;generate x
    push dword 100
    call get_random
    add esp, 4
    mov [ebx+d_x], eax
    ;;generate y
    push dword 100
    call get_random
    add esp, 4
    mov [ebx+d_y], eax
    ;;generate angle
    push dword 360
    call get_random
    add esp, 4
    mov [ebx+d_angle], eax
    ;;points
    mov dword [ebx+d_points], 0

    mov ebx, [droneArr]			; ecx = address of arr
	mov ebx, [ebx+4*edx]		; ecx+4*edx is address of member and new ecx is the member

    inc edx
    jmp start_init_drone_arr_loop
    end_init_drone_arr_loop:

    popad
    pop ebp
    ret

;; Inits All CoRoutines - drone cor array, drones, printer, scheduler and target
init_cors:
    push ebp
    mov ebp, esp
    pushad 

    ;;drone coroutine arr
    mov eax, [N]
    shl eax, 2                              ; eax = N * sizeof(cor*) = 4N
    pushad
    push eax
    call malloc
    add esp, 4
    mov [corArr], eax
    popad

    pushad
    push eax
    call malloc
    add esp, 4
    mov [dronesStack], eax
    popad

    ;;drone coroutine members
    mov ecx, [corArr]                       ; ecx = corArr
    mov edx, 0                              ; edx = i
    mov ebx, [dronesStack]
    start_init_cor_arr_loop:
    cmp edx, dword [N]                      ; for (edx = 0; edx < N; edx++)
    jz end_init_cor_arr_loop

    call new_cor
    mov [ecx+4*edx], eax                    ; corArr[i] = new cor address
    push ebx
    push dword droneFunc
    push eax
    call init_cor
    add esp, 12

    add ebx, 4
    inc edx
    jmp start_init_cor_arr_loop
    end_init_cor_arr_loop:

    ;;scheduler
    call new_cor
    mov [schedulerCor], eax
    push schedulerStack
    push schedulerFunc
    push eax
    call init_cor
    add esp, 12

    ;;printer
    call new_cor
    mov [printerCor], eax
    push printerStack
    push printerFunc
    push eax
    call init_cor
    add esp, 12

    ;;target
    call new_cor
    mov [targetCor], eax
    push targetStack
    push targetFunc
    push eax
    call init_cor
    add esp, 12

    popad
    pop ebp
    ret

;; Init CoRoutine
;; gets an address of a coroutine struct and a coroutine function and initializes it
init_cor:
    push ebp
    mov ebp, esp
    pushad

    mov ebx, [ebp+8]
    mov ecx, [ebp+12]
    mov edx, [ebp+16]

    call new_stack
    mov [edx], eax
    add eax, STKSZ
    mov dword [ebx+CODEP], ecx
    mov dword [ebx+FLAGSP], 0
    mov dword [ebx+SPP], eax

    call co_init
    popad
    pop ebp
    ret

;; Create Target
;; gives the target struct new coordinates
createTarget:
    push ebp
    mov ebp, esp
    pushad

    mov ebx, [target]
    ;;generate x
    push dword 100
    call get_random
    add esp, 4
    mov [ebx+t_x], eax
    ;;generate y
    push dword 100
    call get_random
    add esp, 4
    mov [ebx+t_y], eax

    popad
    pop ebp
    ret

;; Free Structs
;; Frees all structs from program - target and drones
free_structs:
    push ebp
    mov ebp, esp
    pushad

    pushad 
    push dword [target]
    call free
    add esp, 4
    popad

    mov edx, [droneArr]
    mov ecx, 0
    start_free_drone_arr_loop:
    cmp ecx, [N]
    jz end_free_drone_arr_loop

    pushad
    push dword [edx+4*ecx]
    call free
    add esp, 4
    popad

    inc ecx
    jmp start_free_drone_arr_loop
    end_free_drone_arr_loop:

    push edx
    call free
    add esp, 4

    popad
    pop ebp
    ret

;; Free CoRoutines
;; Frees all coroutines in program - printer, scheduler, target and drones
free_cors:
    push ebp
    mov ebp, esp
    pushad

    push dword [schedulerStack]
    push dword [schedulerCor]
    call free_cor
    add esp, 8

    push dword [printerStack]
    push dword [printerCor]
    call free_cor
    add esp, 8

    push dword [targetStack]
    push dword [targetCor]
    call free_cor
    add esp, 8

    mov edx, [corArr]
    mov ecx, 0
    mov ebx, [dronesStack]
    start_free_drone_cors_loop:
    cmp ecx, [N]
    jz end_free_drone_cors_loop

    push dword [ebx]
    push dword [edx+4*ecx]
    call free_cor
    add esp, 8

    add ebx, 4
    inc ecx
    jmp start_free_drone_cors_loop
    end_free_drone_cors_loop:

    pushad
    push dword [corArr]
    call free
    add esp, 4
    popad

    push dword [dronesStack]
    call free
    add esp, 4

    popad
    pop ebp
    ret

;; New Drone
;; returns address to new drone struct
new_drone:
    push ebp
    mov ebp, esp
    push edx
    push ecx
    push ebx

    push dword 16
    call malloc
    add esp, 4

    pop ebx
    pop ecx
    pop edx
    pop ebp
    ret

;; New CoRoutine
;; returns address to new coroutine struct
new_cor:
    push ebp
    mov ebp, esp
    push edx
    push ecx
    push ebx

    push dword 12
    call malloc
    add esp, 4

    pop ebx
    pop ecx
    pop edx
    pop ebp
    ret

;; New Stack for CoRoutine
;; return address to new stack
new_stack:
    push ebp
    mov ebp, esp
    push edx
    push ecx
    push ebx

    push dword STKSZ
    call malloc
    add esp, 4

    pop ebx
    pop ecx
    pop edx
    pop ebp
    ret

;; New Target
;; return address of target struct
new_target:
    push ebp
    mov ebp, esp
    push edx
    push ecx
    push ebx

    push dword 8
    call malloc
    add esp, 4

    pop ebx
    pop ecx
    pop edx
    pop ebp
    ret

;; Free CoRoutine
;; Frees Coroutine and stack
free_cor:
    push ebp
    mov ebp, esp
    pushad
    
    mov ebx, [ebp+8]
    mov edx, [ebp+12]

    pushad
    push edx
    call free
    add esp, 4
    popad

    push ebx
    call free
    add esp, 4

    popad
    pop ebp
    ret

;; LFSR random
;; Takes the number in currRand and uses LFSR to make a new random number 
;; and store it in currRand
LFSR:
    push ebp 
    mov ebp, esp
    pushad

    mov ax, [currRand]  ; ax = currRand = x
    mov cx, ax      ; cx = x >> 0    

    mov bx, ax
    shr bx, 2       ; bx = x >> 2
    xor cx, bx      ; cx = x >> 0 ^ x >> 2

    mov bx, ax      
    shr bx, 3       ; bx = x >> 3
    xor cx, bx      ; cx = x >> 0 ^ x >> 2 ^ x >> 3

    mov bx, ax 
    shr bx, 5       ; bx = x >> 5
    xor cx, bx      ; cx = x >> 0 ^ x >> 2 ^ x >> 3 ^ x >> 5


    mov bx, ax      
    shr bx, 1       ; bx = x >> 1 
    shl cx, 15      ; cx = bit << 15 
    or bx, cx       ; bx = x >> 1 | bit << 15

    mov eax, 0
    mov ax, bx      ; eax = 0...0bx
    mov [currRand], eax

    popad
    pop ebp
    ret

;; Get Random
;; gets x and returns a random in range [0, x]
get_random:
    push ebp
    mov ebp, esp
    push edx
    push ecx
    push ebx

    mov ecx, [ebp+8]            ; ecx = x where rand is in [0, x]

    call LFSR
    fild dword [currRand]       ; stack = currRand
    fild dword [MAXINT16]       ; stack = MAXINT16, currRand
    fdivp st1                   ; stack = currRand / MAXINT16

    fild_through_reg ecx        ; stack = param.0, currRand / MAXINT16
    fmulp st1                   ; stack = param * (currRand / MAXINT16)

    fstp dword [scaledRand]
    mov eax, [scaledRand]

    pop ebx
    pop ecx
    pop edx
    pop ebp
    ret

;  EBX is pointer to co-routine structure to initialize
co_init:
	pusha
	bts	dword [EBX+FLAGSP],0  ; test if already initialized
	jc	init_done
	mov	EAX,[EBX+CODEP] ; Get initial PC
	mov	[SPT], ESP
	mov	ESP,[EBX+SPP]   ; Get initial SP
	mov	EBP, ESP        ; Also use as EBP
	push	EAX             ; Push initial "return" address
	pushf                   ; and flags
	pusha                   ; and all other regs
	mov	[EBX+SPP],ESP    ; Save new SP in structure
	mov	ESP, [SPT]      ; Restore original SP
init_done:
	popa
	ret

;; Start Coroutine
;; ebx = address of coroutine struct
startCo:
    push ebp
    mov ebp, esp
    pushad

    mov [SPMAIN], esp
    mov ebx, [ebp+8]
    jmp do_resume


; ebx = pointer to coroutine
resume:
    pushfd
    pushad
    mov edx, [CURR]
    mov [edx+SPP], esp
do_resume:
    mov ecx, [ebx+SPP]
    mov esp, [ebx+SPP]
    mov [CURR], ebx
    popad
    popfd
    ret