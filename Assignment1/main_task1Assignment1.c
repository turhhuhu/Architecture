#include <stdio.h>
#include <stdlib.h>

#define STRING_MAX_SIZE 32

extern void assFunc(int x, int y);

char c_checkValidity(int x, int y){
    return (x >= y) ? 1 : 0;
}

int main(int argc, char **argv)
{
    char c1[STRING_MAX_SIZE];
    char c2[STRING_MAX_SIZE];
    int x, y;
    sscanf(fgets(c1, STRING_MAX_SIZE, stdin), "%d", &x);
    sscanf(fgets(c2, STRING_MAX_SIZE, stdin), "%d", &y);
    assFunc(x, y);
}
