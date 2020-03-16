#include <stdio.h>
#include <string.h>
#define LOWER_CASE_LOW_BOUND 97
#define LOWER_CASE_HIGH_BOUND 122
#define LOWER_CASE_TO_HIGHER_CASE_DIFF 32

void debug(int isInDebug, char *string)
{
    if (isInDebug)
    {
        FILE *errOutput = stderr;
        fprintf(errOutput, "%s\n", string);
    }
}

void debugCompute(int isInDebug, char beforeComputeation, char afterComputation)
{
    if (isInDebug)
    {
        FILE *errOutput = stderr;
        fprintf(errOutput, "%d\t%d\n", beforeComputeation, afterComputation);
    }
}

int main(int argc, char **argv)
{
    FILE *output = stdout;
    FILE *input = stdin;
    int i, encryptChar, inputChar;
    char *encryptionString;
    char *fileName;
    int isInDebug = 0;
    int isEncrypted = 0;
    int encryptionIndex = 0;
    int isSubEncryption = 0;
    for (i = 1; i < argc; i++)
    {
        if (strcmp(argv[i], "-D") == 0)
        {
            isInDebug = 1;
        }
        else if (strncmp(argv[i], "+e", 2) == 0)
        {
            isEncrypted = 1;
            encryptionString = argv[i] + 2;
        }
        else if (strncmp(argv[i], "-e", 2) == 0)
        {
            isEncrypted = 1;
            isSubEncryption = 1;
            encryptionString = argv[i] + 2;
        }
        else if(strncmp(argv[i], "-o", 2) == 0){
            fileName = argv[i] + 2;
            output = fopen(fileName, "w");
        }
        else if(strncmp(argv[i], "-i", 2) == 0){
            fileName = argv[i] + 2;
            input = fopen(fileName, "r");
            if(input == NULL){
                fprintf(stderr, "cannot open \"%s\", exiting program\n", fileName);
                return 0;
            }
        }      
    }
    for (i = 1; i < argc; i++)
    {
        debug(isInDebug, argv[i]);
    }

    while (!feof(input))
    {
        inputChar = getc(input);
        if (inputChar == '\n')
        {
            debug(isInDebug, "");
            encryptionIndex = 0;
            fprintf(output, "%s", "\n");
        }
        else if (inputChar == -1)
        {
            
        }
        else if (isEncrypted)
        {
            if (encryptionString[encryptionIndex] == '\0')
            {
                encryptionIndex = 0;
            }
            encryptChar = encryptionString[encryptionIndex];
            encryptChar -= '0';
            if (isSubEncryption)
            {
                debugCompute(isInDebug, inputChar, inputChar - encryptChar);
                fprintf(output, "%c", inputChar - encryptChar);
            }
            else
            {
                debugCompute(isInDebug, inputChar, inputChar + encryptChar);
                fprintf(output, "%c", inputChar + encryptChar);
            }
            encryptionIndex++;
        }
        else
        {
            if (inputChar >= LOWER_CASE_LOW_BOUND && inputChar <= LOWER_CASE_HIGH_BOUND)
            {
                debugCompute(isInDebug, inputChar, inputChar - LOWER_CASE_TO_HIGHER_CASE_DIFF);
                fprintf(output, "%c", inputChar - LOWER_CASE_TO_HIGHER_CASE_DIFF);
            }
            else
            {
                debugCompute(isInDebug, inputChar, inputChar);
                fprintf(output, "%c", inputChar);
            }
        }
    }
    printf("%s", "\n");
    fclose(output);
    fclose(input);
}
