myElf: myElf.o
	gcc -m32 -g -Wall -o myElf myElf.o

myElf.o: task3.c
	gcc -m32 -g -Wall -c -o myElf.o task3.c

.PHONY: clean

clean:
	rm -f *.o myElf
