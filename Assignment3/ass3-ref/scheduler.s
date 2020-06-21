section .text
	align 16

    global schedulerFunc

	extern schedulerCor
	extern resume
	extern get_random
	extern distance
	extern beta
    extern t_x
    extern t_y
    extern target
    extern double_for_printing
    extern printer_drone_format_string
    extern printer_target_format_string
    extern N
    extern droneArr
    extern d_x
    extern d_y
    extern d_angle
    extern d_points
    extern finishCond
    extern corArr
    extern K
    extern printerCor
    extern end_co
    extern ok_print_format_string
    extern droneIndex
    extern poiner_print_format_string

    extern printf

schedulerFunc:
    mov ecx, 0                  ; ecx = i for drone
    mov ebx, 0                  ; ebx = i for steps
    start_scheduler_loop:
    mov edx, 0                  ; init edx:eax = i
    mov eax, ecx                ; eax = i for drone
    div dword [N]
    mov ecx, edx                ; ecx = i = edx = i % n 

    mov [droneIndex], ecx       ; update drone index for droneFunc
    push ebx
    mov ebx, [corArr]
	mov	ebx, [ebx+4*ecx]        ; ebx = address of CoRoutines[i]
	call dword resume           ; run thread
    pop ebx
    inc ebx                     ; 1 step

    cmp ebx, dword [K]          ; if K steps have passed
    jnz no_print
    mov ebx, [printerCor]
    call dword resume           ; print board
    mov ebx, 0

    no_print:
    inc ecx
    jmp start_scheduler_loop    ; restart loop
