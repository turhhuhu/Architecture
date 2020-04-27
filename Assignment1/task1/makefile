task1Assignment1: main_task1Assignment1.o asm_task1Assignment1.o
	gcc -m32 -Wall -g -o task1Assignment1 main_task1Assignment1.o asm_task1Assignment1.o

main_task1Assignment1.o: main_task1Assignment1.c
	gcc -m32 -Wall -g -c -o main_task1Assignment1.o main_task1Assignment1.c

asm_task1Assignment1.o: asm_task1Assignment1.s
	nasm -g -f elf -w+all -o asm_task1Assignment1.o asm_task1Assignment1.s


.PHONY: clean

clean:
	rm -f *.o task1Assignment1
