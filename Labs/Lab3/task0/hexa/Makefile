hexaPrint: hexaPrint.o
	gcc -m32 -Wall -g -o hexaPrint hexaPrint.o

hexaPrint.o: hexaPrint.c
	gcc -m32 -Wall -g -c -o hexaPrint.o hexaPrint.c

.PHONY: clean

clean:
	rm -f *.o hexaPrint