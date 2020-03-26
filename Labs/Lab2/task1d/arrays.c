#include <stdio.h>
#include <stdlib.h>
int main(int argc, char **argv)
{
    int iarray[] = {1,2,3};
    char carray[] = {'a','b','c'};
    int* iarrayPtr = iarray;
    char* carrayPtr = carray;
    void *p;
    int i;
    for (i=0; i < 3; i++){
        printf("%d", *(iarrayPtr + i));
    }
    printf("\n");
    for (i=0; i < 3; i++){
        printf("%c", *(carrayPtr + i));
    }
    printf("\n0x%x\n", p);

}
