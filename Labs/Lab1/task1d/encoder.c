#include <stdio.h>
#include <string.h>
#define LOWER_CASE_LOW_BOUND 97
#define LOWER_CASE_HIGH_BOUND 122
#define LOWER_CASE_TO_HIGHER_CASE_DIFF 32
#define SPACE_ASCII 32

void debug(FILE *debugOutput, int isInDebug, char *string)
{
    if (isInDebug)
    {
        fprintf(debugOutput, "%s\n", string);
    }
}

void debugCompute(FILE *debugOutput, int isInDebug, char beforeComputeation, char afterComputation)
{
    if (isInDebug)
    {
        fprintf(debugOutput, "%d\t%d\n", beforeComputeation, afterComputation);
    }
}

int main(int argc, char **argv)
{
    FILE *output = stdout;
    FILE *input = stdin;
    FILE *debugOutput = stderr;
    int i, encryptChar, inputChar, inputcharCompute;
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
        else if (strncmp(argv[i], "-o", 2) == 0)
        {
            fileName = argv[i] + 2;
            output = fopen(fileName, "w");
        }
    }

    for (i = 1; i < argc; i++)
    {
        debug(debugOutput, isInDebug, argv[i]);
    }

    while (!feof(input))
    {
        inputChar = getc(input);
        if (inputChar == '\n' || inputChar == SPACE_ASCII)
        {
            debug(debugOutput, isInDebug, "");
            encryptionIndex = 0;
            fprintf(output, "%s", "\n");
        }
        else if (inputChar == -1)
        {
            continue;
        }
        else if (isEncrypted)
        {
            encryptionIndex = (encryptionString[encryptionIndex] == '\0') ? 0 : encryptionIndex;
            encryptChar = encryptionString[encryptionIndex];
            encryptChar -= '0';
            inputcharCompute = (isSubEncryption) ? inputChar - encryptChar : inputChar + encryptChar;
            debugCompute(debugOutput, isInDebug, inputChar, inputcharCompute);
            fprintf(output, "%c", inputcharCompute);
            encryptionIndex++;
        }
        else
        {
            inputcharCompute = (inputChar >= LOWER_CASE_LOW_BOUND && inputChar <= LOWER_CASE_HIGH_BOUND) ? \
             inputChar - LOWER_CASE_TO_HIGHER_CASE_DIFF : inputChar;
            debugCompute(debugOutput, isInDebug, inputChar, inputcharCompute);
            fprintf(output, "%c", inputcharCompute);
        }
    }
    fclose(output);
    fclose(input);
}
