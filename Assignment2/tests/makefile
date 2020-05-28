all: calc test calc2 generator

calc: calc.c
	gcc -m32 -g -c -Wall -o calc.o calc.c
	gcc -m32 -g -o calc calc.o

test: test.c
	gcc -m32 -g -c -Wall -o test.o test.c
	gcc -m32 -g -o test test.o

calc2: calc.s
	nasm -g -f elf calc.s -o calc2.o
	gcc -m32 -Wall -g calc2.o -o calc2

generator: generate_tests.c
	gcc -m32 -g -c -Wall -o generator.o generate_tests.c
	gcc -m32 -g -o generator generator.o

.PHONY: clean

clean:
	rm *.o calc test calc2 generator