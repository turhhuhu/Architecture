#include <stdio.h>
#include <string.h>
#define LOWER_CASE_LOW_BOUND 97
#define LOWER_CASE_HIGH_BOUND 122
#define LOWER_CASE_TO_HIGHER_CASE_DIFF 32

int main(int argc, char **argv)
{
    FILE *output = stdout;
    FILE *input = stdin;
    int inputChar, inputcharCompute;

    while (!feof(input))
    {
        inputChar = getc(input);
        if (inputChar == '\n')
        {       
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
            fprintf(output, "%c", inputcharCompute);
        }
    }

}
