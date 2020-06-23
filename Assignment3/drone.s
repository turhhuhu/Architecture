global drone_func
extern printf
extern CORS
extern resume
extern float_temp
section .rodata
    str_format: db "%s", 0
    int_format: db "%d", 10, 0 
    float_format: db "%.2f", 10, 0
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

extern target_X
extern target_Y
extern board_size_max
extern board_size_min

section .bss
    X_Coord: resd 1
    Y_Coord: resd 1
    speed: resd 1
    heading: resd 1
    delta_X: resd 1
    delta_Y: resd 1
    target_distance: resd 1
    MAX_distance: resd 1
    distance: resd 1
section .data
    one_eighty: dd 180.0
    ninety: dd 90.0
    score: dd 0
    temp1: dd 10.0
    temp2: dd 20.0

section .text

drone_func:
    pop dword [score]
    pop dword [X_Coord]
    pop dword [Y_Coord]
    pop dword [speed]
    pop dword [heading]
    
    ; printfM int_format, dword [score]
    print_float_num dword [X_Coord]
    ; print_float_num dword [Y_Coord]
    ; print_float_num dword [speed]
    ; print_float_num dword [heading]

    call moveDrone
    call generateRandoms
    print_float_num dword [X_Coord]
    
    ; fld dword [temp2]
    ; fst dword [X_Coord]
    ; fst dword [MAX_distance]
    ; fstp dword [Y_Coord]
    ; fld dword [temp1]
    ; fst dword [target_X]
    ; fstp dword [target_Y]


    call mayDestroy
    printfM int_format, eax

    printInfo "drone"
    mov ebx, dword [CORS]
    call resume

generateRandoms:


moveDrone:
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
    printInfo "here"
    fsub dword [X_Coord]
    .x_not_above:
    fstp dword [temp1]
    print_float_num dword [temp1]
    fld dword [Y_Coord]
    fcomi
    jb .y_not_above
    fsub dword [Y_Coord]
    .y_not_above:
    fstp
    fstp
    fld dword [board_size_min]
    fld dword [X_Coord]
    fcomi
    ja .x_not_below
    fld dword [board_size_max]
    fadd dword [X_Coord]
    .x_not_below:
    fstp
    fstp
    fld dword [Y_Coord]
    fcomi
    ja .y_not_below
    fld dword [board_size_max]
    fadd dword [Y_Coord]
    .y_not_below:


    ret

mayDestroy:
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
    print_float_num dword [target_distance]
    fld dword [MAX_distance]
    mov eax, 0
    fcomip
    jb .no_destroy
    mov eax, 1
    .no_destroy:
    ret