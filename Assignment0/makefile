#format is target-name: target dependencies
#{-tab-}actions

# All Targets
all: hello

# Tool invocations
# Executable "hello" depends on the files hello.o and run.o.
hello: hello.o start.o
	gcc -m32 -g -Wall -o hello hello.o start.o

# Depends on the source and header files
hello.o: mainAssignment0.c
	gcc -g -Wall -m32 -ansi -c -o hello.o mainAssignment0.c 
 
start.o: asmAssignment0.s
	nasm -g -f elf -w+all -o start.o asmAssignment0.s


#tell make that "clean" is not a file name!
.PHONY: clean

#Clean the build directory
clean: 
	rm -f *.o hello
