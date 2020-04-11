menu_map: menu_map.o
	gcc -m32 -Wall -g -o menu_map menu_map.o

menu_map.o: menu_map.c
	gcc -m32 -Wall -g -c -o menu_map.o menu_map.c

.PHONY: clean

clean:
	rm -f *.o menu_map