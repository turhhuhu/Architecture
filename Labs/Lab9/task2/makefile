virus: virus.o
	ld  -g -m elf_i386 -g virus.o -o virus

virus.o: virus.s
	nasm -g -f elf32 -w+all -o virus.o virus.s

.PHONY: clean

clean:
	rm -f *.o virus