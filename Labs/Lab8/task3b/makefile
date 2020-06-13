myElf: myElf.o
	gcc -no-pie -m32 -g -Wall -o myElf myElf.o

myElf.o: task3.c
	gcc -no-pie -m32 -g -Wall -c -o myElf.o task3.c

.PHONY: clean

clean:
	rm -f *.o myElf
