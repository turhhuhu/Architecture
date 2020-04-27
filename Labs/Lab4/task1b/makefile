encoder: encoder.o start.o util.o
	ld -m elf_i386 -g start.o encoder.o util.o -o encoder

encoder.o: encoder.c
	gcc -g -m32 -Wall -ansi -c -nostdlib -fno-stack-protector encoder.c -o encoder.o

util.o: lab4_util.c
	gcc -g -m32 -Wall -ansi -c -nostdlib -fno-stack-protector lab4_util.c -o util.o

start.o: lab4_start.s
	nasm -g -f elf32 -w+all -o start.o lab4_start.s

.PHONY: clean

clean:
	rm -f *.o main