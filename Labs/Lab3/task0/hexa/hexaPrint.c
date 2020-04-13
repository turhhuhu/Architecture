#include <stdlib.h>
#include <stdio.h>
#define CHAR_TO_PRINT_LOWER_BOUND 32

void printHex(FILE *buffer, int length)
{
    int i;
    int *byte = (int *)calloc(length, 1);
    fread(byte, 1, length, buffer);
    for (i = 0; i < length; i++)
    {

        if (byte[i] > CHAR_TO_PRINT_LOWER_BOUND)
        {
            printf("%X ", byte[i]);
        }
    }
    free(byte);
}

int main(int argc, char **argv)
{
    FILE *byteFileBuffer = fopen(argv[1], "r");
    while (!feof(byteFileBuffer))
    {
        printHex(byteFileBuffer, 2);
    }
    fclose(byteFileBuffer);
    printf("\n");
}