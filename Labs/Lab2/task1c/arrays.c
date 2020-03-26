#include <stdio.h>
#include <stdlib.h>
int main(int argc, char **argv)
{
    int iarray[3];
    float farray[3];
    double darray[3];
    char carray[3];
    printf("0x%x\n", iarray);
    printf("0x%x\n", iarray + 1);
    printf("0x%x\n", farray);
    printf("0x%x\n", farray + 1);
    printf("0x%x\n", darray);
    printf("0x%x\n", darray + 1);
    printf("0x%x\n", carray);
    printf("0x%x\n", carray + 1);
}
