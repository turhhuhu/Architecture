count: count.o
	gcc -m32 -Wall -g -o count count.o

count.o: count-words.c
	gcc -m32 -Wall -g -c -o count.o count-words.c

.PHONY: clean

clean:
	rm -f *.o count