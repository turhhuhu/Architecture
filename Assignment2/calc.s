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

section .rodata
    format: db "%d" , 10, 0
    hex_format: db "%X", 0
    str_format: db "%s", 0

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


