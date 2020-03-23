#include <stdio.h>
#include <string.h>
#define LOWER_CASE_LOW_BOUND 97
#define LOWER_CASE_HIGH_BOUND 122
#define LOWER_CASE_TO_HIGHER_CASE_DIFF 32

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
    int i, inputChar, inputcharCompute;
    int isInDebug = 0;
    for (i = 1; i < argc; i++)
    {
        if (strcmp(argv[i], "-D") == 0)
        {
            isInDebug = 1;
            break;
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
            fprintf(output, "%s", "\n");
        }
        else if (inputChar == -1)
        {
            continue;
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
