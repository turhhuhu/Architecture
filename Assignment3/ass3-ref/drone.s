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

	global droneFunc

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
	extern ok_print_format_string
	extern droneIndex
	extern drone_winner_format_string
	extern T
	extern targetCor
	extern scaledRand
	extern poiner_print_format_string
	extern helperInt
	extern float_print_format_string
	extern helperFloat

    extern printf


;; fild_through_reg(int i)
%macro fild_through_reg 1
    pushad
    mov eax, %1
    mov [helperInt], eax
    fild dword [helperInt]
    popad
%endmacro

droneFunc:
	mov edx, [droneIndex]
	mov ecx, [droneArr]			; ecx = address of arr
	mov ecx, [ecx+4*edx]		; ecx+4*edx is address of member and new ecx is the member

	call calculate_random_angle
	mov ebx, eax
	call calculate_random_distance
	mov ecx, eax

	push ecx
	push ebx
	call calculate_new_position
	add esp, 8

	mov edx, [droneIndex]
	mov ecx, [droneArr]			; ecx = address of arr
	mov ecx, [ecx+4*edx]		; ecx+4*edx is address of member and new ecx is the member

	call mayDestroy

	cmp eax, 1
	jne failed_destroy

	mov edx, [droneIndex]
	mov ecx, [droneArr]
	mov ecx, [ecx+4*edx]
	inc dword [ecx+d_points]

	mov edx, dword [T]
	cmp dword [ecx+d_points], edx
	jnz continue_game

	pushad
    push dword [droneIndex]
    push drone_winner_format_string
    call printf
    add esp, 8
    popad
	jmp end_co

	continue_game:
	mov ebx, [targetCor]
	call dword resume
	failed_destroy:
	mov ebx, [schedulerCor]
	call dword resume
	jmp droneFunc


;generate a random number in range [-60,60] 16 bit and put it in random_angle
;input-none
;output-eax=random_angle
calculate_random_angle:
	push ebp
	mov ebp, esp
	push edx
	push ecx
	push ebx

	push dword 120
	call get_random
	add esp, 4	

	fld dword [scaledRand]
	fild_through_reg 60		; st0=60.0 st1=rand
	fsubp st1				; stack=rand-60
	fstp dword [scaledRand]
	mov eax, [scaledRand]

	pop ebx
	pop ecx
	pop edx
	pop ebp
	ret


;generate a random number in range [0,50] 16 bit and put it in random_distance
;input-none
;output-eax=random_distance
calculate_random_distance:
	push ebp
	mov ebp, esp
	push edx
	push ecx
	push ebx

	push dword 50
	call get_random		; use macro to get random number in range [0,50]
	add esp, 4

	pop ebx
	pop ecx
	pop edx
	pop ebp
	ret


;compute the new position of drone acording to random_distance and random_angle
;intput-random_distance and random angle
;output-none
calculate_new_position:
	push ebp
	mov ebp, esp
	pushad

	mov edx, [droneIndex]
	mov ecx, [droneArr]			; ecx = address of arr
	mov ecx, [ecx+4*edx]		; ecx+4*edx is address of member and new ecx is the member

	fld dword [ecx+d_angle]
	fld dword [ebp+8]   
	faddp st1					; stack = currAngle+randomAngle
	
	fild_through_reg 360		; st0=360 st1=angle
	fcomip st1					; cmp st0, st1, st0=angle
	jnb	angle_neg_check			; if 360>=angle go check neg
	fild_through_reg 360
	fsubp st1					; stack=angle-360
	jmp angle_in_range

	angle_neg_check:
	fild_through_reg 0			; st0=0, st1=angle
	fcomip st1					; cmp st0, st1
	jnae angle_in_range			; if 0<angle angle in range
	fild_through_reg 360
	faddp st1					; stack=angle+360

	angle_in_range:
	fst dword [ecx+d_angle]     ; drone.angle = newAngle !!!OK!!!

	fldpi						; st0=pi st1=angle
	fild_through_reg 180		; st0=180 st1=pi st2=angle
	fdivp st1					; st0=pi/180 st1=angle
	fmulp st1					; st0=angleInRadians
	fsincos						; st0=cos st1=sin

	fld dword [ebp+12]    		; st0=randomDistance  st1=cos st2=sin
	fmulp st1              		; st0=randomDistance*cos st1=sin
	fld dword [ecx+d_x]			; st0=drone.x st1=dist*cos st2=sin
	faddp st1					; st0=dist*cos+x st1=sin !!!OK!!!
	
	fild_through_reg 100		; st0=100 st1=x st2=sin
	fcomip st1					; cmp st0, st1
	jnb	x_neg_check				; if 100>=x go check neg
	fild_through_reg 100
	fsubp st1					; st0=x-100 st1=sin
	jmp x_in_range

	x_neg_check:
	fild_through_reg 0			; st0=0, st1=x st2=sin
	fcomip st1					; cmp st0, st1
	jnae x_in_range				; if 0<x x in range
	fild_through_reg 100
	faddp st1					; st0=x+100 st1=sin

	x_in_range:
	fstp dword [ecx+d_x]		; st0=sin !!!OK!!!

	fld dword [ebp+12]    		; st0=randomDistance  st1=sin
	fmulp st1              		; st0=randomDistance*sin
	fld dword [ecx+d_y]			; st0=drone.y st1=dist*sin
	faddp st1					; st0=dist*sin+y
	
	fild_through_reg 100		; st0=100 st1=x st2=sin
	fcomip st1					; cmp st0, st1
	jnb y_neg_check				; if 100>=x go check neg
	fild_through_reg 100
	fsubp st1					; st0=x-100 st1=sin
	jmp y_in_range

	y_neg_check:
	fild_through_reg 0			; st0=0, st1=x st2=sin
	fcomip st1					; cmp st0, st1
	jnae y_in_range				; if 0<x x in range
	fild_through_reg 100
	faddp st1					; st0=x+100 st1=sin

	y_in_range:
	fstp dword [ecx+d_y]

	popad
	pop ebp
	ret



;check if target in range of current drone
;input:coordinates(x2,y2) of the target
;output: return true iff target in the range of drone
mayDestroy:
	push ebp
	mov ebp, esp
	push edx
	push ecx
	push ebx

	mov edx, [target]  		; edx = target
	mov ebx, [droneIndex]
	mov ecx, [droneArr]
	mov ecx, [ecx+4*ebx]	; ecx = drone

	fld dword [edx+t_x]		; st0=target.x
	fld dword [ecx+d_x]		; st0=drone.x st1=target.x
	fsubp st1				; st0=x1-x2
	fmul st0				; st0=(x1-x2)^2
	fld dword [edx+t_y]		; st0=target.y st1=(x1-x2)^2
	fld dword [ecx+d_y]		; st0=drone.y st1=target.y st2=(x1-x2)^2
	fsubp st1				; st0=y1-y2 st1=(x1-x2)^2
	fmul st0				; st0=(y1-y2)^2 st1=(x1-x2)^2
	faddp st1				; st0=(x1-x2)^2+(y1-y2)^2
	fsqrt					; st0=distance

	fld dword [distance]	; st0=maxDistance st1=d
	fcomip st1				; cmp st0 st1
	jna cant_destroy		; d>=maxDistance 	
	
	fstp dword [helperFloat]; emptyStack
	fld dword [edx+t_x]		; st0=target.x
	fld dword [ecx+d_x]		; st0=drone.x st1=target.x
	fsubp st1				; st0=x1-x2
	fld dword [edx+t_y]		; st0=target.y st1=x1-x2
	fld dword [ecx+d_y]		; st0=drone.y st1=target.y st2=x1-x2
	fsubp st1				; st0=y1-y2 st1=x1-x2
	fxch					; st0=x1-x2 st1=y1-y2
	fpatan					; st0=gammaInRad

	fild_through_reg 180	; st0=180 st1=gammaRad 
	fldpi					; st0=pi st1=180 st2=gammaRad
	fdivp st1				; st0=180/pi st1=gammaRad
	fmulp st1				; st0=gamma
	fld st0					; st0=gamma st1=gamma

	fld dword [ecx+d_angle]	; st0=alpha st1=gamma st2=gamma
	fxch					; st0=gamma st1=alpha st2=gamma
	fld dword [ecx+d_angle]	; st0=alpha st1=gamma...

	fsubp st1				; st0=gamma-alpha...
	fabs					; st0=|alpha-gamma|... 
	fild_through_reg 180	; st0=180 st1=|alpha-gamma|...
	fcomip st1				; cmp st0 st1. st0=|alpha-gamma|...
	jnb difference_in_range	; 180>=|alpha-gamma|
	fstp dword [helperFloat]; st0=alpha st1=gamma		
	fcomi st1				; cmp st0 st1. st0=alpha st1=gamma
	jnae gamma_is_bigger	; alpha<gamma

	fxch					; st0=gamma st1=alpha
	fild_through_reg 360
	faddp st1				; st0=newGamma st1=alpha
	fsubp st1				; st0=alph-newGamma
	fabs					; st0=|alpha-gamma|

	fild_through_reg 1
	fild_through_reg 1
	fxch st2				; st0=|alpha-gamma| and 2 trash
	jmp difference_in_range

	gamma_is_bigger:
	fild_through_reg 360
	faddp st1				; st0=newAlpha st1=gamma
	fsubp st1				; st0=gamma-newAlpha
	fabs					; st0=|alpha-gamma|

	fild_through_reg 1
	fild_through_reg 1
	fxch st2				; st0=|alpha-gamma| and 2 trash
	difference_in_range:	; COME HERE WITH sizeStack = 3 st0=|alpha-gamma|
	fxch st2				; st2=|alpha-gamma|
	fstp dword [helperFloat]
	fstp dword [helperFloat]; st0=|alpha-gamma|
	fld dword [beta]		; st0=beta st1=|alpha-gamma|
	fcomip st1				; cmp st0 st1. st0=|alpha-gamma|
	jna cant_destroy		; |alpha-gamma|>=beta	

	fstp dword [helperFloat]
	mov eax, 1
	jmp end

	cant_destroy: 			; COME HERE WITH sizeStack = 1
	fstp dword [helperFloat]
	mov eax, 0
	jmp end

	end:
	pop ebx
	pop ecx
	pop edx
	pop ebp
	ret