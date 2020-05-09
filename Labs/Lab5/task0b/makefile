all: looper task0

task0: task0a.o lineParser.o
	gcc -m32 -Wall -g -o task0 task0a.o lineParser.o

task0a.o: task0a.c
	gcc -m32 -Wall -g -c -o task0a.o task0a.c

lineParser.o: LineParser.c
	gcc -m32 -Wall -g -c -o lineParser.o LineParser.c


looper: looper.o
	gcc -m32 -Wall -g -o looper looper.o

looper.o: looper.c
	gcc -m32 -Wall -g -c -o looper.o looper.c

.PHONY: clean

clean:
	rm -f *.o task0 looper