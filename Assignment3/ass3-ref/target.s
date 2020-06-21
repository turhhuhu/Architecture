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

    global targetFunc

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
    extern createTarget

    extern printf

targetFunc:
    call createTarget
    mov ebx, [schedulerCor]
	call dword resume
    jmp targetFunc

