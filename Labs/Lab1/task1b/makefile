encoder: encoder.o
	gcc -g -Wall -m32 -o encoder encoder.o

encoder.o: encoder.c
	gcc -g -Wall -m32 -c -o encoder.o encoder.c

.PHONY: clean

clean:
	rm -f *.o encoder
