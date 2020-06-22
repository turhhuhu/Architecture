
section .rodata
    float_string_format: db "%f",10,0

section .data
    global x
    global y
    global angle
    global speed
    global target_x
    global target_y 
    global d
    seed_1: dw 0xACE1
    seed: dw 0xACE1
    max_delta_angle: dq 60.0
    min_delta_angle: dq -60.0
    max_delta_speed: dq 10.0
    min_delta_speed: dq -10.0
    max_int: dd 65535
    delta_angle: dq 0
    delta_speed: dq 0
    temp: dq 0.0
    x: dq 0
    y: dq 0
    speed: dq 0
    angle: dq 0
    target_x: dq 0.0
    target_y: dq 0.0
    d: dq 0.0

section .text
    global random_generator_1
    extern printf
    extern random_generator2


random_generator_1:
    push ebx
    push ecx
    push edx

    mov eax, 0
    mov ax, [seed]
    mov ebx, 0

    mov bx, ax

    shr ax, 2
    xor bx, ax

    shr ax, 1
    xor bx, ax

    shr ax, 2
    xor bx, ax

    shr word [seed], 1
    shl bx, 15
    or word [seed], bx
    mov ax, word [seed]

    pop edx
    pop ecx
    pop ebx
    ret

;mayDestroy:
    ; copy here a function that using the values x,y (the drone coordinates), target_x,target_y (the target coordinates)
    ; and d (the maximus distance for hit) returns true if the drone can destory the target.

;change_drone_position: 
    ; copy here a function that uses the values in x,y,angle,speed calculates the drone new location (and stores it in x and y)
    ; and also calculate the new angle and speed
    ; when generating the delta_angle and delta_speed use the random_generator2 function, otherwise the tests won't work