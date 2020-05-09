task1: task1.o lineParser.o
	gcc -m32 -Wall -g -o task1 task1.o lineParser.o

task1.o: task1.c
	gcc -m32 -Wall -g -c -o task1.o task1.c
lineParser.o: LineParser.c
	gcc -m32 -Wall -g -c -o lineParser.o LineParser.c

.PHONY: clean

clean:
	rm -f *.o task1