#include <stdio.h>
#include <string.h>
#define LOWER_CASE_LOW_BOUND 97
#define LOWER_CASE_HIGH_BOUND 122
#define LOWER_CASE_TO_HIGHER_CASE_DIFF 32
#define EOF (-1)

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
    int i, encryptChar, inputChar, inputCharAfterComputation;
    int isInDebug = 0, isEncrypted = 0, encryptionIndex = 0, \
        isSubEncryption = 0;
    char *encryptionString;

    for (i=1; i < argc; i++)
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

    }

    for (i = 1; i < argc; i++)
    {
        debug(debugOutput, isInDebug, argv[i]);
    }

    while (!feof(input))
    {
        inputChar = getc(input);
        if (inputChar == '\n')
        {
            debug(debugOutput, isInDebug, "");
            encryptionIndex = 0;
            fprintf(output, "%s", "\n");
        }
        else if (inputChar == EOF)
        {
            continue;
        }
        else if (isEncrypted)
        {
            encryptionIndex = (encryptionString[encryptionIndex] == '\0') ? 0 : encryptionIndex;
            encryptChar = encryptionString[encryptionIndex];
            encryptChar -= '0';
            inputCharAfterComputation = (isSubEncryption) ? inputChar - encryptChar : inputChar + encryptChar;
            debugCompute(debugOutput, isInDebug, inputChar, inputCharAfterComputation);
            fprintf(output, "%c", inputCharAfterComputation);
            encryptionIndex++;
        }
        else
        {
            inputCharAfterComputation = (inputChar >= LOWER_CASE_LOW_BOUND && inputChar <= LOWER_CASE_HIGH_BOUND) ? \
             inputChar - LOWER_CASE_TO_HIGHER_CASE_DIFF : inputChar;
            debugCompute(debugOutput, isInDebug, inputChar, inputCharAfterComputation);
            fprintf(output, "%c", inputCharAfterComputation);
        }
    }
}