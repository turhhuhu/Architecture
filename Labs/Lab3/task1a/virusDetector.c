
#include <stdio.h>
#include <stdlib.h>

typedef struct virus
{
    unsigned short SigSize;
    char virusName[16];
    unsigned char *sig;
} virus;

virus *readVirus(FILE *buffer)
{
    if (!feof(buffer))
    {
        unsigned short size;
        virus *virusPtr = (virus *)malloc(sizeof(virus));
        if(fread(virusPtr, 1, 18, buffer) != 18)
        {
            free(virusPtr);
            return NULL;
        }
        printf("%d\n", virusPtr -> SigSize);
        size = virusPtr->SigSize;
        virusPtr->sig = (unsigned char *)malloc(size);
        fread(virusPtr->sig, 1, size, buffer);
        return virusPtr;
    }
    return NULL;
}

void printVirus(virus *virus, FILE *output)
{
    fputs("Virus name: ", output);
    fputs(virus->virusName, output);
    fputc('\n', output);
    fputs("Virus size: ", output);
    fprintf(output, "%d", virus->SigSize);
    fputc('\n', output);
    fputs("signature:\n", output);
    fwrite(virus->sig, 1, virus->SigSize, output);
}

int main(int argc, char **argv)
{

    FILE *buffer = fopen(argv[1], "r");
    FILE *output = fopen(argv[2], "w");
    virus* v = NULL;
    if (!feof(buffer))
    {
        v = readVirus(buffer);
        if (v != NULL)
        {
            printVirus(v, output);
            free(v -> sig);
            free(v);
        }
    }
    while (!feof(buffer))
    {
        v = readVirus(buffer);
        if (v != NULL)
        {
            fputc('\n', output);
            printVirus(v, output); 
            free(v -> sig);
            free(v);
        } 
    }

    fclose(buffer);
    fclose(output);
    return 0;
}