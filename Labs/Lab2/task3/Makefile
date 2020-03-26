menu: menu.o
	gcc -m32 -Wall -g -o menu menu.o

menu.o: menu.c
	gcc -m32 -Wall -g -c -o menu.o menu.c

.PHONY: clean

clean:
	rm -f *.o menu