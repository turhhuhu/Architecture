hexeditplus: hexeditplus.o
	gcc -m32 -g -Wall -fno-pie -o hexeditplus hexeditplus.o

hexeditplus.o: hexeditplus.c
	gcc -m32 -g -Wall -fno-pie -c -o hexeditplus.o hexeditplus.c

.PHONY: clean

clean:
	rm -f *.o hexeditplus
