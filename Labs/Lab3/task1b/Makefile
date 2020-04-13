virusDetector: virusDetector.o
	gcc -m32 -Wall -g -o virusDetector virusDetector.o

virusDetector.o: virusDetector.c
	gcc -m32 -Wall -g -c -o virusDetector.o virusDetector.c

.PHONY: clean

clean:
	rm -f *.o virusDetector