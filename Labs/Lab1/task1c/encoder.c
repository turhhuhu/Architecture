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
    int i, encryptChar, inputChar;
    char *encryptionString;
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
    }
    for (i = 1; i < argc; i++)
    {
        debug(isInDebug, argv[i]);
    }

    while (!feof(stdin))
    {
        inputChar = getc(stdin);
        if (inputChar == '\n')
        {
            debug(isInDebug, "");
            encryptionIndex = 0;
            printf("%s", "\n");
        }
        else if (inputChar == -1)
        {
            printf("%c", inputChar);
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
                printf("%c", inputChar - encryptChar);
            }
            else
            {
                debugCompute(isInDebug, inputChar, inputChar + encryptChar);
                printf("%c", inputChar + encryptChar);
            }
            encryptionIndex++;
        }
        else
        {
            if (inputChar >= LOWER_CASE_LOW_BOUND && inputChar <= LOWER_CASE_HIGH_BOUND)
            {
                debugCompute(isInDebug, inputChar, inputChar - LOWER_CASE_TO_HIGHER_CASE_DIFF);
                printf("%c", inputChar - LOWER_CASE_TO_HIGHER_CASE_DIFF);
            }
            else
            {
                debugCompute(isInDebug, inputChar, inputChar);
                printf("%c", inputChar);
            }
        }
    }
    printf("%s", "\n");
}
