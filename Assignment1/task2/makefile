task2Assignment1: Assignment1.o assembly.o
	gcc -m32 -Wall -g -o task2Assignment1 Assignment1.o assembly.o

Assignment1.o: main_task2Assignment1.c
	gcc -m32 -Wall -g -c -o Assignment1.o main_task2Assignment1.c

assembly.o: asm_task2Assignment1.s
	nasm -g -f elf -w+all -o assembly.o asm_task2Assignment1.s


.PHONY: clean

clean:
	rm -f *.o task1Assignment1
