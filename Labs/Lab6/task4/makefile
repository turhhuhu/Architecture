task4: task4.o lineParser.o
	gcc -m32 -Wall -g -o task4 task4.o lineParser.o

task4.o: task4.c
	gcc -m32 -Wall -g -c -o task4.o task4.c
lineParser.o: LineParser.c
	gcc -m32 -Wall -g -c -o lineParser.o LineParser.c

.PHONY: clean

clean:
	rm -f *.o task4