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
  
%define NODE_SIZE 5
%define MAX_USER_INPUT 80
section .data
    strp: db "ABCD", 0
    strp1: db "8DF2", 0
    p: dd 0
    opsp: dd opStack

section .bss
    opStack: resd 256
    size: resd 1
    inputBuffer: resb MAX_USER_INPUT

section .rodata
    format: db "%d" , 10, 0
    hex_format: db "%X", 0
    str_format: db "%s", 0
    char_format: dd "%c", 0

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

;; this macro changes edx!!!
;; DO NOT USE EDX AS PUSH ARGUMENT
%macro pushOp 1
    pushad
    push eax
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
        printErr "Error: Operand Stack Overflow"
        pop eax
    %%pushend:
    popad
%endmacro

;; this macro changes edx!!!
;; DO NOT USE EDX AS POP ARGUMENT
;; this macro returns 1 in ebx if pop was successfull and 0 otherwise.
%macro popOp 1
    isEmptyOpStack
    cmp eax, 0
    mov ebx, 1
    jne %%poperr
    mov edx, [opsp]
    sub edx, 4
    mov [opsp], edx
    mov %1, [edx]
    jmp %%popend
    %%poperr:
        mov ebx, 0
        printErr "Error: Insufficient Number of Arguments on Stack"
    %%popend:
%endmacro

%macro peekOp 1
    isEmptyOpStack
    cmp eax, 0
    mov ebx, 1
    jne %%peekerr
    mov edx, [opsp]
    sub edx, 4
    mov %1, [edx]
    jmp %%peekend
    %%peekerr:
        mov ebx, 0
        printErr "Error: Insufficient Number of Arguments on Stack"
    %%peekend:
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

%macro hexCharConvertor 1
    cmp %1, 65
    jl %%cont
    sub %1, 7
    %%cont:
    sub %1, '0'
%endmacro

%macro printErr 1
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

%macro printStr 1
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

%macro printfMacro 2
    pushad
    push %1
    push %2
    call printf
    add esp, 8
    popad
%endmacro

main:
    push ebp 
    mov ebp, esp
    pushad

    mov eax, 0
    mov eax, [ebp + 8] ;; eax holds argc
    cmp eax, 2
    jge .userOpStackSizeInput
    mov eax, 5 ;; eax holds default stack size
    jmp .initStack
    .userOpStackSizeInput:
    mov eax, [ebp + 12] ;; argv
    mov eax, [eax + 4] ;; eax holds user string operand stack size
    push eax
    call strToDecimal
    add esp, 4
    ;; eax now has decimal stack size
    .initStack:
    mov [size], eax
    
   

    .calc_loop:
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
    call popAndPrint
    jmp .calc_loop
    .notPrint:
    cmp byte [inputBuffer], 'd'
    jne .notDup
    call duplicate
    jmp .calc_loop
    .notDup:
    .loop_cont:
    createListAndPush inputBuffer
    jmp .calc_loop
    .end_calc_loop:


    popad
    pop ebp
    ret

popAndPrint:
    push ebp
    mov ebp, esp
    pushad

    popOp eax
    cmp ebx, 0
    je .end
    mov ecx, eax
    mov eax, [eax]
    push eax
    call printList
    add esp, 4
    push ecx
    call freeList
    add esp, 4
    printStr ""
    .end:
    
    popad
    pop ebp
    ret
    
duplicate:
    push ebp
    mov ebp, esp
    pushad

    peekOp ecx ; headp of the list to dup 
    cmp ebx, 0
    je .end

    allocate 4
    mov ebx, eax ; headP of the dup list
    pushOp ebx ;need to check for overflow!! 
    mov ecx, [ecx] 
    cmp ecx, 0
    je .end
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
    popad
    pop ebp
    ret
    
strToDecimal:
    push ebp
    mov ebp, esp

    mov ecx, dword [ebp+8]
	mov eax, 0
	mov ebx, 0
	for_loop: ;converting the string to a decimal number
		cmp byte [ecx], 10
		jz end_loop
		cmp byte [ecx], 0
		jz end_loop
		mov bl, [ecx]
		sub bl, '0'
		imul eax, 10
		add eax, ebx
		inc ecx
		jmp for_loop
	end_loop:

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

    mov eax, [ebp + 8]
    mov ebx, 0
    mov ecx, 0
    mov bl, [eax]
    mov cl, bl
    and cl, 00001111b
    shr bl ,4
    cmp ebx, 0
    jne .printHalfByte
    pushad
    push eax
    call printList
    add esp, 4
    popad
    jmp .end

    .printHalfByte:
    mov eax, [eax + 1]
    pushad
    push eax
    call printList
    add esp, 4
    popad
    printfMacro ecx, hex_format
    .end:

    popad
    pop ebp
    ret
    
printList:
    push ebp
    mov ebp, esp
    pushad

    mov eax, [ebp + 8]
    cmp eax, 0
    je .end
    mov ebx, [eax + 1]
    push ebx
    call printList
    add esp, 4
    mov ebx, 0
    mov ecx, 0
    mov bl, [eax]
    mov cl, bl
    and cl, 00001111b
    shr bl, 4
    printfMacro ebx, hex_format
    printfMacro ecx, hex_format
    .end:
    popad
    pop ebp
    ret


printListReverse:
    push ebp
    mov ebp, esp
    pushad

    mov eax, [ebp + 8]
    mov eax, [eax]
    .printLoop:
        cmp eax, 0
        je .endPrintLoop
        mov ebx, 0
        mov bl, [eax]
        pushad
        push ebx
        push hex_format
        call printf
        add esp, 8
        popad
        mov eax, [eax + 1]
        jmp .printLoop
    .endPrintLoop:
    popad
    pop ebp
    ret


