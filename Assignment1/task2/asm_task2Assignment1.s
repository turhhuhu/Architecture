section	.rodata			; we define (global) read-only variables in .rodata section
	format_string: db "%s", 10, 0	; format string
	format: db "%d", 10, 0

section .bss			; we define (global) uninitialized variables in .bss section
	an: resb 12		; enough to store integer in [-2,147,483,648 (-2^31) : 2,147,483,647 (2^31-1)]

section .text
	global convertor
	extern printf

convertor:
	push ebp
	mov ebp, esp	
	pushad			

	mov ecx, dword [ebp+8]	; get function argument (pointer to string)
	mov eax, 0
	for_loop:
		cmp byte [ecx], 10
		jz end_loop
		cmp byte [ecx], 0
		jz end_loop
		mov dword ebx, [ecx]
		sub ebx, '0'
		imul eax, 10
		add eax, ebx
		jmp for_loop
	end_loop:
	push eax
	push format
	call printf
	add esp, 8
	; your code comes here...

	push an			; call printf with 2 arguments -  
	push format_string	; pointer to str and pointer to format string
	call printf
	add esp, 8		; clean up stack after call
	
	popad			
	mov esp, ebp	
	pop ebp
	ret
