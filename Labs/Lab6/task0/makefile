mypipe: mypipe.o 
	gcc -m32 -Wall -g -o mypipe mypipe.o 

mypipe.o: mypipe.c
	gcc -m32 -Wall -g -c -o mypipe.o mypipe.c

.PHONY: clean

clean:
	rm -f *.o mypipe