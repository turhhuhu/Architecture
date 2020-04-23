section .data
	index: db 0
	
section	.rodata			; we define (global) read-only variables in .rodata section
	format_string: db "%s", 10, 0	; format string
	format: db "%d", 10, 0
	hexStr: db "0123456789ABCDEF", 0 

section .bss			; we define (global) uninitialized variables in .bss section
	an: resb 12	; enough to store integer in [-2,147,483,648 (-2^31) : 2,147,483,647 (2^31-1)]

section .text
	global convertor
	extern printf
	
convertor:
	push ebp
	mov ebp, esp	
	pushad			

	mov ecx, dword [ebp+8]	; get function argument (pointer to string)
	mov eax, 0
	mov ebx, 0
	for_loop:
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

	mov ebx ,0 ;temp
	mov edx, 0 ;index an
	mov ecx, 0
	for_loop_hexa:
		mov ecx, hexStr
		cmp byte [index], 8
		jge end_loop_hexa
		mov bl, al
		and bl, 00001111b
		cmp bl, 0
		jz end_loop_hexa
		add ecx, ebx
		mov ecx, [ecx]
		mov edx, [index]
		add edx, an
		mov [edx], ecx
		shr eax, 4
		inc byte [index]
		jmp for_loop_hexa
	end_loop_hexa:
	mov byte [edx + 1], 0 
	push an			; call printf with 2 arguments -  
	push format_string	; pointer to str and pointer to format string
	call printf
	add esp, 8		; clean up stack after call

	popad
	mov esp, ebp	
	pop ebp
	ret
