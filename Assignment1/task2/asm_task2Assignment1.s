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

	; eax contains the number to convert to hex, in decimal value
	mov ebx ,0 ; temp
	mov edx, 0 ; "an" index
	mov ecx, 0 ; pointer to the hex string
	for_loop_hexa:
		mov ecx, hexStr ; making ecx point to the start of the hex string
		cmp byte [index], 8 ;checking we run not more than 8 times (eax size)
		jge end_loop_hexa ; if we did jump to the end of the loop
		mov bl, al ; moving the 8 bytes from al (start of eax) to bl
		and bl, 00001111b ; making sure bl has only the 4 bit at the start of al(to transfom to hex)
		add ecx, ebx ; taking the value from ebx to ecx (ebx equals to the value of bl) and adding it to ecx (hex string pointer)
		mov ecx, [ecx] ; taking the value of the char at [ecx] and inserting it into ecx  
		mov edx, [index] ; moving to edx the value of index
		add edx, an ; adding to edx the pointer of an (an + index)
		mov [edx], ecx ; moving to the [an + index] the value of the char
		shr eax, 4 ;shifting eax 4 bits to get the next char
		inc byte [index] ; incrementing index
		cmp eax, 0 ; 
		jz end_loop_hexa ; 
		jmp for_loop_hexa ; jumping to the start
	end_loop_hexa:
	mov byte [index], 0
	mov byte [edx + 1], 0 ; making it only hex string

	mov ebx, an
	mov ecx, 0
	mov eax, 0
	reverse_loop:
		cmp ebx, edx
		jge end_reverse_loop
		mov cl, [edx]
		mov al, [ebx]
		mov [edx], al
		mov [ebx], cl
		inc ebx
		dec edx
		jmp reverse_loop
	end_reverse_loop:


	push an			; call printf with 2 arguments -  
	push format_string	; pointer to str and pointer to format string
	call printf
	add esp, 8		; clean up stack after call

	popad
	mov esp, ebp	
	pop ebp
	ret
