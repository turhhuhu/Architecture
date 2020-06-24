global drone_func
extern printf
extern CORS
extern resume
extern float_temp
extern target_X
extern target_Y
extern board_size_max
extern board_size_min
extern get_scaled_random
extern RAND
extern createTarget
extern Max_distance
section .rodata
    str_format: db "%s", 0
    int_format: db "%d", 10, 0 
    float_format: db "%.2f", 10, 0
%macro printInfo 1
jmp %%skip_print
section .rodata
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

%macro printfM 2
    pushad
    push %2
    push %1
    call printf
    add esp, 8
    popad
%endmacro

%macro push_all_stats 0
    push dword [heading]
    push dword [speed]
    push dword [Y_Coord]
    push dword [X_Coord]
    push dword [score]
%endmacro

%macro pop_all_stats 0
    pop dword [score]
    pop dword [X_Coord]
    pop dword [Y_Coord]
    pop dword [speed]
    pop dword [heading]
%endmacro

section .bss
    X_Coord: resd 1
    Y_Coord: resd 1
    speed: resd 1
    heading: resd 1
    delta_X: resd 1
    delta_Y: resd 1
    target_distance: resd 1
    distance: resd 1
    delta_speed: resd 1
    delta_heading: resd 1
    SCHEDULER_CO: equ 0
    TARGET_CO: equ 2
    score: resd 1
section .rodata
    one_eighty: dd 180.0
    ninety: dd 90.0
    sixty: dd 60.0
    ten: dd 10.0
    hundred: dd 100.0
    max_angle: dd 360.0
    min_angle: dd 0.0
    max_speed: dd 100.0
    min_speed: dd 0.0

section .text

drone_func:
    pop_all_stats
    
    call moveDrone
    call generateNewAngle
    call generateNewSpeed

    .drone_loop:
    call mayDestroy
    cmp eax, 1
    jne .no_destroy
    inc dword [score]
    push_all_stats
    get_co TARGET_CO
    call resume
    pop_all_stats
    .no_destroy:
    call moveDrone
    call generateNewAngle
    call generateNewSpeed
    push_all_stats
    get_co SCHEDULER_CO
    call resume
    pop_all_stats
    jmp .drone_loop
    .end_drone_loop:


generateNewAngle:
    push ebp
    mov ebp, esp

    push 120
    call get_scaled_random
    add esp, 4
    
    finit
    fld dword [RAND]
    fld dword [sixty]
    fsubp
    fld dword [heading]
    faddp
    fst dword [heading]
    fld dword [max_angle]
    fcomi
    ja .angle_not_above
    fsub 
    fstp dword [heading]
    jmp .after_change
    .angle_not_above:
    fstp st0
    fld dword [min_angle]
    fcomi
    jb .after_change
    fstp st0
    fld dword [max_angle]
    faddp
    fstp dword [heading]
    .after_change:

    add eax, dword [heading]
    pop ebp
    ret

generateNewSpeed:
    push ebp
    mov ebp, esp

    push 20
    call get_scaled_random
    add esp, 4

    finit
    fld dword [RAND]
    fld dword [ten]
    fsubp
    fld dword [speed]
    faddp
    fst dword [speed]
    fld dword [max_speed]
    fcomi
    ja .speed_not_above
    fstp dword [speed]
    fstp st0
    jmp .after_change
    .speed_not_above:
    fstp st0
    fld dword [min_speed]
    fcomi
    jb .after_change
    fstp dword [speed]
    fstp st0
    .after_change:
    

    pop ebp
    ret

moveDrone:
    finit
    fld dword [heading]
    fldpi
    fmulp
    fld dword [one_eighty]
    fdivp
    fsincos
    fld dword [speed]
    fmulp
    fld dword [Y_Coord]
    faddp
    fstp dword [Y_Coord]
    fld dword [speed]
    fmulp
    fld dword [X_Coord]
    faddp
    fstp dword [X_Coord]
    fld dword [ninety]
    fadd dword [heading]
    fstp dword [heading]


    fld dword [board_size_max]
    fld dword [X_Coord]
    fcomi
    jb .x_not_above
    ;x above
    fsub st1
    fstp dword [X_Coord]
    .x_not_above:
    fstp st0
    fld dword [Y_Coord]
    fcomi
    jb .y_not_above
    ;y above
    fsub st1
    fstp dword [Y_Coord]
    .y_not_above:
    fstp st0
    fstp st0
    
    
    fld dword [board_size_min]
    fld dword [X_Coord]
    fcomi
    ja .x_not_below
    ;x below
    fld dword [board_size_max]
    fadd st1
    fstp dword [X_Coord]
    .x_not_below:
    fstp st0
    fld dword [Y_Coord]
    fcomi
    ja .y_not_below
    ;y below
    fld dword [board_size_max]
    fadd st1
    fstp dword [Y_Coord]
    .y_not_below:
    fstp st0
    fstp st0

    ret

mayDestroy:
    finit
    fld dword [target_X]
    fld dword [X_Coord]
    fsubp ;  (target_X - caller_X) in st(0)
    fst dword [delta_X]
    fld dword [delta_X]
    fmulp ; (dx)^2 in st(0)
    fld dword [target_Y]
    fld dword [Y_Coord]
    fsubp ; (target_Y - caller_Y) in st(0)
    fst dword [delta_Y]
    fld dword [delta_Y]
    fmulp ; (dy)^2 in st(0), (dx)^2 in st(1)
    faddp ; (dy)^2 + (dx)^2 in st(0)
    fsqrt
    fst dword [target_distance]
    fld dword [Max_distance]
    mov eax, 0
    fcomip
    jb .no_destroy
    mov eax, 1
    .no_destroy:
    ret