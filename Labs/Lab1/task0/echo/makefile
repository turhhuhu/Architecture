my_echo: echo.o
	gcc -g -Wall -m32 -o my_echo echo.o

echo.o: echo.c
	gcc -g -Wall -m32 -c -o echo.o echo.c

.PHONY: clean

clean:
	rm -f *.o my_echo
