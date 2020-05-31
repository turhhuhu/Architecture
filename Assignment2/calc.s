section .text 
  global main
  extern printf
  extern fprintf 
  extern fflush
  extern malloc 
  extern calloc 
  extern free 
  extern getchar 
  extern fgets
  extern stdin
  extern stderr
  extern stdout

%define NODE_SIZE 5
%define MAX_USER_INPUT 80
section .data
    p: dd 0
    opsp: dd opStack
    debug: db 0
    size: dd 5
section .bss
    opStack: resd 256
    inputBuffer: resb MAX_USER_INPUT

section .rodata
    format: db "%d" , 10, 0
    hex_format: db "%X", 0
    str_format: db "%s", 0
    char_format: dd "%c", 0
    hex_format_withNl: db "%X", 10, 0
    calc_string: db "calc: ", 0

%macro isFullOpStack 0
    push ebx
    mov ebx, [size]
    imul ebx, 4
    add ebx, opStack
    mov eax, 0
    cmp ebx, [opsp]
    jne %%nfull
    mov eax, 1
    %%nfull:
    pop ebx
%endmacro

%macro isEmptyOpStack 0
    push ebx
    mov ebx, opStack
    mov eax, 0
    cmp ebx, [opsp]
    jne %%nempty
    mov eax, 1
    %%nempty:
    pop ebx
%endmacro

%macro hasAtleast2Ops 0
    push ebx
    mov ebx, 8
    add ebx, opStack
    mov eax, 0
    cmp ebx, [opsp]
    ja %%no2ops
    mov eax, 1
    %%no2ops:
    pop ebx
%endmacro


; this macro changes edx!!!
; DO NOT USE EDX AS PUSH ARGUMENT
%macro pushOp 1
    push edx
    push eax
    mov ebx, 1
    isFullOpStack
    cmp eax, 0
    jne %%pusherr
    pop eax
    mov edx, [opsp]
    mov [edx], %1
    add edx, 4
    mov [opsp], edx
    jmp %%pushend
    %%pusherr:
        pop eax
    %%pushend:
    pop edx
%endmacro

; this macro changes edx!!!
; DO NOT USE EDX AS POP ARGUMENT
; this macro returns 1 in ebx if pop was successfull and 0 otherwise.
%macro popOp 1
    push eax
    isEmptyOpStack
    cmp eax, 0
    jne %%poperr
    pop eax
    mov edx, [opsp]
    sub edx, 4
    mov [opsp], edx
    mov %1, [edx]
    jmp %%popend
    %%poperr:
    pop eax
    %%popend:
%endmacro


%macro peekOp 1
    push edx
    push eax
    isEmptyOpStack
    cmp eax, 0
    jne %%peekerr
    pop eax
    mov edx, [opsp]
    sub edx, 4
    mov %1, [edx]
    jmp %%peekend
    %%peekerr:
    pop eax
    %%peekend:
    pop edx
%endmacro

%macro allocate 1
    push ecx
    push ebx
    push edx
    push %1
    call malloc
    add esp, 4
    mov dword [eax], 0
    pop edx
    pop ebx
    pop ecx
%endmacro

    
%macro createListAndPush 1
    pushad
    allocate 4
    pushOp eax
    push eax
    push %1
    call createListFromHexString
    add esp, 8
    popad
%endmacro

%macro createListFromNumberAndPush 1
    pushad
    allocate 4
    pushOp eax
    push eax
    push %1
    call createListFromNumber
    add esp, 8
    popad
%endmacro

%macro hexCharConvertor 1
    cmp %1, 65
    jb %%cont
    sub %1, 7
    %%cont:
    sub %1, '0'
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


%macro debugM 0
jmp %%skip_print
section .data
    %%str_to_print: db "debug: ", 0
section .text
    %%skip_print:
    pushad
    cmp byte [debug], 0
    je %%noDebgug
    push %%str_to_print
    push str_format
    push dword [stderr]
    call fprintf
    add esp, 12
    peekOp eax
    push eax
    push dword [stderr]
    call printListWithoutLeadingZeros
    add esp, 8
    printInfo ""
    %%noDebgug:
    popad
%endmacro

%macro fprintfM 3
    pushad
    push %3
    push %2
    push %1
    call fprintf
    add esp, 12
    popad
%endmacro

main:
    push ebp 
    mov ebp, esp
    pushad
    
    mov ecx, [ebp + 8]
    mov ebx, 1
    .flag_loop:
    cmp ebx, ecx
    je .end_flag_loop
    mov eax, [ebp + 12]
    mov eax,dword [eax + 4*ebx]
    cmp byte [eax], '-'
    jne .not_debug
    cmp byte [eax + 1], 'd'
    jne .not_debug
    mov byte [debug], 1
    jmp .debug
    .not_debug:
    push eax
    call strToDecimal
    add esp, 4
    mov dword [size], eax
    .debug:
    inc ebx
    jmp .flag_loop
    .end_flag_loop:

    call myCalc
    mov ebx, [stdout]
    fprintfM ebx, format, eax

    popad
    pop ebp
    ret

myCalc:
    push ebp
    mov ebp, esp
    push ebx
    push ecx
    push edx
    sub esp, 4

    mov dword [ebp - 4], 0
    .calc_loop:
    mov eax, [stdout]
    fprintfM eax, str_format, calc_string
    inc dword [ebp - 4]
    mov dword [inputBuffer], 0
    push dword [stdin]
    push MAX_USER_INPUT
    push inputBuffer
    call fgets
    add esp, 12
    cmp byte [inputBuffer], 'q'
    je .end_calc_loop
    cmp byte [inputBuffer], 'p'
    jne .notPrint
    isEmptyOpStack
    cmp eax, 1 
    je .error_insufficent
    call popAndPrint
    jmp .calc_loop
    .notPrint:
    cmp byte [inputBuffer], 'd'
    jne .notDup
    isEmptyOpStack
    cmp eax, 1 
    je .error_insufficent
    call duplicate
    debugM
    jmp .calc_loop
    .notDup:
    cmp byte [inputBuffer], '|'
    jne .notOr
    hasAtleast2Ops
    cmp eax, 0 
    je .error_insufficent
    call orOperator
    debugM
    jmp .calc_loop
    .notOr:
    cmp byte [inputBuffer], '&'
    jne .notAnd
    hasAtleast2Ops
    cmp eax, 0 
    je .error_insufficent
    call andOperator
    debugM
    jmp .calc_loop
    .notAnd:
    cmp byte [inputBuffer], '+'
    jne .notPlus
    hasAtleast2Ops
    cmp eax, 0 
    je .error_insufficent
    call plusOperator
    debugM
    jmp .calc_loop
    .notPlus:
    cmp byte [inputBuffer], 'n'
    jne .notNumOfDig
    isEmptyOpStack
    cmp eax, 1 
    je .error_insufficent
    call numberOfDigits
    debugM
    jmp .calc_loop
    .notNumOfDig:
    .loop_cont:
    isFullOpStack
    cmp eax, 1
    je .error_full
    createListAndPush inputBuffer
    debugM
    dec dword [ebp - 4]
    jmp .calc_loop

   
    .error_insufficent:
    printInfo "Error: Insufficient Number of Arguments on Stack"
    dec dword [ebp - 4]
    jmp .calc_loop
    .error_full:
    printInfo "Error: Operand Stack Overflow"
    dec dword [ebp - 4]
    jmp .calc_loop
    
    .end_calc_loop:

    dec dword [ebp - 4]
    
    call freeStack
    
    mov eax, dword [ebp - 4]
    
    add esp, 4
    pop edx
    pop ecx
    pop ebx
    pop ebp
    ret
;;ebx length
;;edx num of zeros
listLength:
    push ebp
    mov ebp, esp
    push ebx
    push ecx
    push edx

    mov ebx, 0
    mov edx, 0
    mov eax, [ebp + 8]
    mov eax, [eax]
    
    .for_loop:
    cmp eax, 0
    je .end_loop
    cmp byte [eax], 0
    jne .no_zero
    inc edx
    jmp .cont
    .no_zero:
    inc ebx
    add ebx, edx
    mov edx, 0
    .cont:
    mov eax, dword [eax + 1]
    jmp .for_loop
    .end_loop:

    cmp ebx, 0
    jne .end
    cmp edx, 0
    je .end
    add ebx, 1 
    .end:
    mov eax, ebx

    pop edx
    pop ecx
    pop ebx
    pop ebp
    ret
    

numberOfDigits:
    push ebp
    mov ebp, esp
    pushad

    popOp eax
    mov ecx, eax
    mov ebx, [eax]
    mov edx, 0
    push eax
    call listLength
    add esp, 4
    imul eax, 2
    .advance_loop:
    cmp dword [ebx + 1], 0
    je .end_advance_loop
    mov ebx, dword [ebx + 1]
    jmp .advance_loop
    .end_advance_loop:
    mov edx, 0
    mov dl, byte [ebx]
    cmp byte [ebx], 16
    jae .2Digits
    sub eax, 1
    jmp .end
    .2Digits:
    .end:
    mov edx, eax
    createListFromNumberAndPush edx

    push ecx
    call freeList
    add esp, 4

    popad
    pop ebp
    ret


freeStack:
    push ebp
    mov ebp, esp
    pushad
    
    .free_loop:
    isEmptyOpStack
    cmp eax, 0
    jne .end_free_loop
    popOp eax
    push eax
    call freeList
    add esp, 4
    jmp .free_loop
    .end_free_loop:

    popad
    pop ebp
    ret

createListFromNumber:
    push ebp
    mov ebp, esp
    pushad

    mov ebx, [ebp + 8]
    mov ecx, [ebp + 12]

    .for_loop:
    cmp ebx, 0
    je .end_loop
    mov edx, 0
    mov dl, bl
    push edx
    push ecx
    call addToList
    add esp, 8
    shr ebx, 8
    .end_loop:

    popad
    pop ebp
    ret
    
plusOperator:
    push ebp
    mov ebp, esp
    pushad
    sub esp, 16

    popOp eax
    mov [ebp - 4], eax
    popOp ecx
    mov [ebp - 8], ecx

    mov eax, [ebp - 4] 
    mov ecx, [ebp - 8] 
    push eax
    call listLength
    add esp, 4
    mov ebx, eax ; ebx now hold length of first list
    push ecx
    call listLength
    add esp, 4
    cmp ebx, eax
    jae .firstListBigger
    mov edx, [ebp - 4]
    mov ecx, [ebp - 8]
    mov [ebp - 4], ecx
    mov [ebp - 8], edx
    mov edx, [edx]
    mov ecx, [ecx]
    mov [ebp - 16], edx
    mov [ebp - 12], ecx
    mov ecx, ebx
    mov ebx, eax
    mov eax, ecx
    jmp .end_length_compare
    .firstListBigger:
    mov edx, [ebp - 4]
    mov edx, [edx]
    mov ecx, [ebp - 8]
    mov ecx, [ecx]
    mov [ebp - 12], edx
    mov [ebp - 16], ecx

    .end_length_compare:
    mov ecx, eax
    mov dh, 0
    clc
    pushf
    .add_loop:
    cmp ecx, 0
    je .end_add_loop
    mov eax, [ebp - 12]
    mov ebx, [ebp - 16]
    mov dl, [ebx]
    popf
    adc dl, byte [eax]
    pushf
    mov byte [eax], dl
    cmp ecx, 1
    je .end_add_loop
    mov eax, [eax + 1]
    mov ebx, [ebx + 1]
    mov [ebp - 12], eax
    mov [ebp - 16], ebx
    sub ecx, 1
    jmp .add_loop
    .end_add_loop:

    popf
    setc dh
    cmp dh, 0
    je .noCarry
    cmp dword [eax + 1], 0
    je .newNode
    .noNewNode:
    mov eax, dword [eax + 1]
    add byte [eax], 1
    mov dword [ebp - 12], eax
    jnc .noCarry
    .newNode:
    allocate NODE_SIZE
    mov byte [eax], 1
    mov dword [eax + 1], 0
    mov ebx, [ebp - 12]
    mov dword [ebx + 1], eax
    jmp .noCarry
    .noCarry:

    
    mov eax, [ebp - 4]
    pushOp eax

    mov eax, [ebp - 8]
    push eax
    call freeList
    add esp, 4

    .end:
    add esp, 16
    popad
    pop ebp
    ret

popAndPrint:
    push ebp
    mov ebp, esp
    pushad

    popOp eax
    mov ecx, eax
    push eax
    push dword [stdout]
    call printListWithoutLeadingZeros
    add esp, 8
    push ecx
    call freeList
    add esp, 4
    printInfo ""
    .end:
    
    popad
    pop ebp
    ret

orOperator:
    push ebp
    mov ebp, esp
    pushad
    sub esp, 16
    popOp eax
    mov [ebp - 4], eax
    popOp ecx
    mov [ebp - 8], ecx

    mov eax, [ebp - 4] 
    mov ecx, [ebp - 8] 
    push eax
    call listLength
    add esp, 4
    mov ebx, eax ; ebx now hold length of first list
    push ecx
    call listLength
    add esp, 4
    cmp ebx, eax
    jae .firstListBigger
    mov edx, [ebp - 4]
    mov ecx, [ebp - 8]
    mov [ebp - 4], ecx
    mov [ebp - 8], edx
    mov edx, [edx]
    mov ecx, [ecx]
    mov [ebp - 16], edx
    mov [ebp - 12], ecx
    mov ecx, ebx
    mov ebx, eax
    mov eax, ecx
    jmp .end_length_compare
    .firstListBigger:
    mov edx, [ebp - 4]
    mov edx, [edx]
    mov ecx, [ebp - 8]
    mov ecx, [ecx]
    mov [ebp - 12], edx
    mov [ebp - 16], ecx


    .end_length_compare:
    mov ecx, eax
    .or_loop:
    cmp ecx, 0
    je .end_or_loop
    mov eax, [ebp - 12]
    mov ebx, [ebp - 16]
    mov dl, [ebx]
    or dl, byte [eax]
    mov byte [eax], dl
    mov eax, [eax + 1]
    mov ebx, [ebx + 1]
    mov [ebp - 12], eax
    mov [ebp - 16], ebx
    sub ecx, 1
    jmp .or_loop
    .end_or_loop:

    mov eax, [ebp - 4]
    pushOp eax

    mov eax, [ebp - 8]
    push eax
    call freeList
    add esp, 4

    .end:
    add esp, 16
    popad
    pop ebp
    ret


andOperator:
    push ebp
    mov ebp, esp
    pushad
    sub esp, 16
    popOp eax
    mov [ebp - 4], eax
    popOp ecx
    mov [ebp - 8], ecx

    mov eax, [ebp - 4] 
    mov ecx, [ebp - 8] 
    push eax
    call listLength
    add esp, 4
    mov ebx, eax ; ebx now hold length of first list
    push ecx
    call listLength
    add esp, 4
    cmp ebx, eax
    jae .firstListBigger
    mov edx, [ebp - 4]
    mov ecx, [ebp - 8]
    mov [ebp - 4], ecx
    mov [ebp - 8], edx
    mov edx, [edx]
    mov ecx, [ecx]
    mov [ebp - 16], edx
    mov [ebp - 12], ecx
    mov ecx, ebx
    mov ebx, eax
    mov eax, ecx
    jmp .end_length_compare

    .firstListBigger:
    mov edx, [ebp - 4]
    mov edx, [edx]
    mov ecx, [ebp - 8]
    mov ecx, [ecx]
    mov [ebp - 12], edx
    mov [ebp - 16], ecx

    .end_length_compare:
    mov ecx, eax
    .or_loop:
    cmp ecx, 0
    je .end_or_loop
    mov eax, [ebp - 16]
    mov ebx, [ebp - 12]
    mov dl, [ebx]
    and dl, byte [eax]
    mov byte [eax], dl
    mov eax, [eax + 1]
    mov ebx, [ebx + 1]
    mov [ebp - 16], eax
    mov [ebp - 12], ebx
    sub ecx, 1
    jmp .or_loop
    .end_or_loop:

    mov eax, [ebp - 8]
    pushOp eax

    mov eax, [ebp - 4]
    push eax
    call freeList
    add esp, 4

    .end:
    add esp, 16
    popad
    pop ebp
    ret


duplicate:
    push ebp
    mov ebp, esp
    pushad
    sub esp, 4

    peekOp ecx ; headp of the list to dup 

    allocate 4
    mov [ebp - 4], eax
    pushOp eax ;need to check for overflow!!
    cmp ebx, 0
    je .end
    mov ecx, [ecx] 
    cmp ecx, 0
    je .end
    mov ebx, [ebp - 4]
    allocate NODE_SIZE
    mov dword [ebx], eax
    mov dl, byte [ecx]
    mov byte [eax], dl
    mov dword [eax + 1], 0
    mov ebx, eax
    mov ecx, dword [ecx + 1]
    .for_loop:
    cmp ecx, 0
    je .end
    allocate NODE_SIZE
    mov dword [ebx + 1], eax
    mov dl, byte [ecx]
    mov byte [eax], dl
    mov dword [eax + 1], 0
    mov dword ebx, [ebx + 1]
    mov ecx, [ecx + 1]
    jmp .for_loop 
    .end:
    add esp, 4
    popad
    pop ebp
    ret
    
strToDecimal:
    push ebp
    mov ebp, esp
    push ebx
    push ecx
    push edx

    mov ecx, dword [ebp+8]
	mov eax, 0
	mov ebx, 0
	for_loop: ;converting the string to a decimal number
		cmp byte [ecx], 10
		jz end_loop
		cmp byte [ecx], 0
		jz end_loop
        mov ebx, 0
        mov bl, byte [ecx]
        hexCharConvertor ebx
		imul eax, 16
		add eax, ebx
		inc ecx
		jmp for_loop
	end_loop:

    pop edx
    pop ecx
    pop ebx
    pop ebp
    ret
    

createListFromHexString:
    push ebp 
    mov ebp, esp
    
    mov eax, [ebp + 8]
    mov edx, [ebp + 12]
    mov ebx, 0

    .checkEven:
    cmp byte [eax], 0
    jz .endCheckEven
    cmp byte [eax], 10
    jz .endCheckEven
    mov ecx, 1
    sub ecx, ebx
    mov ebx, ecx
    add eax, 1
    jmp .checkEven
    .endCheckEven:

    mov eax, [ebp + 8]
    cmp ebx, 0
    je .endCmp
    mov ebx, 0
    mov byte bl, [eax]
    hexCharConvertor bl
    pushad
    push ebx
    push edx
    call addToList
    add esp, 8
    popad
    add eax, 1
    jmp .endCmp
    .endCmp:
    
    .for_loop:
        cmp byte [eax], 0
        jz .end
        cmp byte [eax], 10
        jz .end
        mov ecx, 0
        mov byte cl, [eax]
        hexCharConvertor cl
        imul ecx, 16
        mov ebx, 0
        mov bl, [eax + 1]
        hexCharConvertor bl
        add cl, bl
        pushad
        push ecx
        push edx
        call addToList
        add esp, 8
        popad
        add eax, 2
        jmp .for_loop

    .end:
    pop ebp
    ret

addToList:
    push ebp
    mov ebp, esp
    pushad

    allocate NODE_SIZE
    mov ebx, [ebp + 12]
    mov byte [eax], bl
    mov dword ebx, [ebp + 8]

    cmp ebx, 0
    jne .existingList
    mov dword [eax + 1], 0
    mov [ebx], eax
    jmp .end
    .existingList:
    mov dword ebx, [ebx]
    mov dword [eax + 1], ebx   
    mov ebx, [ebp + 8]
    mov [ebx], eax
    .end:

    popad
    pop ebp
    ret


freeList:
    push ebp
    mov ebp, esp
    pushad

    mov eax, [ebp + 8]
    mov eax, [eax]
    .freeLoop:
        cmp eax, 0
        je .endFreeLoop
        mov ebx, [eax + 1]
        pushad
        push eax
        call free
        add esp, 4
        popad
        mov eax, ebx
        jmp .freeLoop
    .endFreeLoop:

    push eax
    call free
    add esp, 4

    mov eax, [ebp + 8]
    push eax
    call free
    add esp, 4

    
    popad
    pop ebp
    ret
printListWithoutLeadingZeros:
    push ebp
    mov ebp, esp
    pushad

    mov ebx, [ebp + 12]
    push ebx
    call listLength
    add esp, 4

    mov ebx, [ebx]
    push ebx
    push eax
    push dword [ebp + 8]
    call printList
    add esp, 12
    popad
    pop ebp
    ret
    
printList:
    push ebp
    mov ebp, esp
    pushad

    mov eax, [ebp + 16]
    mov edx, [ebp + 12]
    mov ebx, [stdout]
    cmp edx, 0
    je .end
    cmp eax, 0
    je .end
    dec edx
    mov ebx, [eax + 1]
    push ebx
    push edx
    push dword [ebp + 8]
    call printList
    add esp, 12
    mov ebx, 0
    mov ecx, 0
    mov bl, [eax]
    mov cl, bl
    and cl, 00001111b
    shr bl, 4
    mov edx, [ebp + 12]   
    cmp edx, 1
    jne .printBoth
    cmp ebx, 0
    jne .printBoth
    mov eax, [ebp + 8]
    fprintfM eax, hex_format, ecx 
    jmp .end
    .printBoth:
    mov eax, [ebp + 8]
    fprintfM eax, hex_format, ebx 
    fprintfM eax, hex_format, ecx
    .end:
    popad
    pop ebp
    ret


