    d_x equ 0
    d_y equ 4
    d_angle equ 8
    d_points equ 12
	CODEP equ 0
	FLAGSP equ 4
	SPP equ 8
    t_x equ 0
    t_y equ 4

section .text
	align 16

    global printerFunc

	extern schedulerCor
	extern resume
	extern get_random
	extern distance
	extern beta
    extern target
    extern double_for_printing
    extern printer_drone_format_string
    extern printer_target_format_string
    extern N
    extern droneArr
    extern finishCond
    extern corArr
    extern K
    extern printerCor
    extern end_co

    extern printf


%macro float_to_stack 1
    fld %1
    fstp qword [double_for_printing]    ; convert float to double
    push dword [double_for_printing+4]
    push dword [double_for_printing]    ; put double (8 bytes) on stack
%endmacro

printerFunc:
    ;mov eax, [ecx+8]
    mov ecx, [target]
    float_to_stack dword [ecx+t_y]
    float_to_stack dword [ecx+t_x]
    push printer_target_format_string
    call printf
    add esp, 20

    mov edx, 0
    start_printing_loop:
    cmp edx, [N]
    jz end_printing_loop
    
    push edx
    call print_drone_info
    add esp, 4

    inc edx
    jmp start_printing_loop
    end_printing_loop:

    mov ebx, [schedulerCor]     ;resume scheduler
    call dword resume
    jmp printerFunc

print_drone_info: 
    push ebp
    mov ebp, esp
    pushad 

    mov ecx, [ebp+8]
    mov ebx, [droneArr]
    mov ebx, [ebx+4*ecx]

    push dword [ebx+d_points]
    float_to_stack dword [ebx+d_angle]
    float_to_stack dword [ebx+d_y]
    float_to_stack dword [ebx+d_x]
    push ecx
    push printer_drone_format_string
    call printf
    add esp, 36

    popad
    pop ebp
    ret