arrays: arrays.o
	gcc -m32 -Wall -o arrays arrays.o

arrays.o: arrays.c
	gcc -m32 -Wall -c -o arrays.o arrays.c

.PHONY: clean

clean:
	rm -f *.o arrays