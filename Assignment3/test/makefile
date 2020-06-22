test: test.o
	gcc -g -Wall -m32 -o test test.o

test.o: test.c
	gcc -g -Wall -m32 -c -o test.o test.c

.PHONY: clean

clean:
	rm -f *.o test
