task2: task2.o lineParser.o
	gcc -m32 -Wall -g -o task2 task2.o lineParser.o

task2.o: task2.c
	gcc -m32 -Wall -g -c -o task2.o task2.c
lineParser.o: LineParser.c
	gcc -m32 -Wall -g -c -o lineParser.o LineParser.c

.PHONY: clean

clean:
	rm -f *.o task2