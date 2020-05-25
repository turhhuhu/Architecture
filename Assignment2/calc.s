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

%define NODE_SIZE 5
section .data
    ; head: dd -1
    ; headP: dd head
    strp: db "7D12", 0
    strp1: db "8DF2", 0
    p: dd 0
    opsp: dd opStack

section .bss
    opStack: resd 256
    size: resd 1

section .rodata
    format: db "%d" , 10, 0
    hex_format: db "%X", 0
    str_format: db "%s",10, 0

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

;; this macro changed edx!!!
;; DO NOT USE EDX AS PUSH ARGUMENT
%macro pushOp 1
    push eax
    isFullOpStack
    cmp eax, 0
    jne %%pusherr
    mov edx, [opsp]
    mov [edx], %1
    add edx, 4
    mov [opsp], edx
    jmp %%pushend
    %%pusherr:
        printErr "Error: Operand Stack Overflow"
    %%pushend:
    pop eax
%endmacro

;; this macro changed edx!!!
;; DO NOT USE EDX AS POP ARGUMENT
%macro popOp 1
    push eax
    isEmptyOpStack
    cmp eax, 0
    jne %%poperr
    mov edx, [opsp]
    sub edx, 4
    mov [opsp], edx
    mov %1, [edx]
    jmp %%popend
    %%poperr:
        printErr "Error: Insufficient Number of Arguments on Stack"
    %%popend:
    pop eax
%endmacro

%macro createListAndPush 1
    jmp %%skip
section .data
    %%head: dd -1
    %%headP: dd %%head
section .text
    %%skip:
    push %%headP
    push %1
    call createListFromHexString
    add esp, 8
    pusha
    mov eax, [%%headP]
    push eax
    call printList
    add esp, 4
    popa
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
    %%str_to_print: db %1, 0
section .text
    %%skip_print:
    pusha
    push %%str_to_print
    push str_format
    call printf
    add esp, 8
    popa
%endmacro

%macro printfMacro 2
    pusha
    push %1
    push %2
    call printf
    add esp, 8
    popa
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
    pushOp ebx
    pushOp ebx
    mov eax, [size]

    

    ; push 0x7D
    ; push headP
    ; call addToList
    ; add esp, 8

    ; push 0x12
    ; push headP
    ; call addToList
    ; add esp, 8

 
    mov eax, strp
    createListAndPush eax

    mov eax, strp1
    createListAndPush eax


    ; push headP
    ; call freeList
    ; add esp, 4
    
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
    .for_loop:
        cmp byte [eax], 0
        jz .end
        cmp byte [eax + 1], 0
        jz .one_digit
        mov ecx, 0
        mov byte cl, [eax]
        hexCharConvertor cl
        imul ecx, 16
        mov ebx, 0
        mov bl, [eax + 1]
        hexCharConvertor bl
        add cl, bl
        pusha
        push ecx
        push edx
        call addToList
        add esp, 8
        popa
        add eax, 2
        jmp .for_loop

    .one_digit:
        mov ebx, 0
        mov byte bl, [eax]
        hexCharConvertor bl
        pusha
        push ebx
        push edx
        call addToList
        add esp, 8
        popa
        add eax, 1
        jmp .for_loop

    .end:
    pop ebp
    ret

addToList:
    push ebp
    mov ebp, esp
    pushad

    push NODE_SIZE
    call malloc
    add esp, 4
    mov ebx, [ebp + 12]
    mov byte [eax], bl
    mov dword ebx, [ebp + 8]
    mov dword ebx, [ebx]
    mov dword [eax + 1], ebx   
    mov ebx, [ebp + 8]
    mov [ebx], eax

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
        cmp byte [eax], -1
        je .endFreeLoop
        mov ebx, [eax + 1]
        pusha
        push eax
        call free
        add esp, 4
        popa
        mov eax, ebx
        jmp .freeLoop
    .endFreeLoop:

    ; push eax
    ; call free
    ; add esp, 4

    ; mov eax, [ebp + 8]
    ; push eax
    ; call free
    ; add esp, 4

    
    popad
    pop ebp
    ret


printList:
    push ebp
    mov ebp, esp
    pushad

    mov eax, [ebp + 8]
    cmp byte [eax], -1
    je .end
    mov ebx, [eax + 1]
    push ebx
    call printList
    add esp, 4
    mov ebx, 0
    mov bl, [eax]
    pusha
    push ebx
    push hex_format
    call printf
    add esp, 8
    popa
    
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
        cmp byte [eax], -1
        je .endPrintLoop
        mov ebx, 0
        mov bl, [eax]
        pusha
        push ebx
        push hex_format
        call printf
        add esp, 8
        popa
        mov eax, [eax + 1]
        jmp .printLoop
    .endPrintLoop:
    popad
    pop ebp
    ret


