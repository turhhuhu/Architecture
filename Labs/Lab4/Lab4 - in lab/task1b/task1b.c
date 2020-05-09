
#include "lab4_util.h"

#define O_RDONLY 0x0
#define O_WRONLY 0x1
#define O_RDWR 0x2
#define O_CREAT 0x64
#define S_IRWXU 00700
#define O_TRUNC 01000
#define SYS_EXIT 1
#define SYS_READ 3
#define SYS_WRITE 4
#define SYS_OPEN 5
#define SYS_CLOSE 6
#define STDIN 0
#define STDOUT 1
#define STDERR 2
#define LOWER_CASE_LOW_BOUND 97
#define LOWER_CASE_HIGH_BOUND 122
#define LOWER_CASE_TO_HIGHER_CASE_DIFF 32
#define NULL 0
#define ERROR 0x55
extern int system_call(int, ...);

void debug_sys(int sysCallNumber, int isInDebugMode, char *msg, int ebx, int ecx, int edx)
{
    if (isInDebugMode)
    {
        char *sysCallNumberAsString = itoa(sysCallNumber);
        char *ebxAsString = NULL;
        char *ecxAsString = NULL;
        char *edxAsString = NULL;
        system_call(SYS_WRITE, STDERR, msg, strlen(msg));
        system_call(SYS_WRITE, STDERR, "\n", sizeof(char));
        system_call(SYS_WRITE, STDERR, "call number (eax): ", 19);
        system_call(SYS_WRITE, STDERR, sysCallNumberAsString, strlen(sysCallNumberAsString));
        if (ebx != -1)
        {
            system_call(SYS_WRITE, STDERR, " ebx: ", 5);
            ebxAsString = itoa(ebx);
            system_call(SYS_WRITE, STDERR, ebxAsString, strlen(ebxAsString));
        }
        if (ecx != -1)
        {
            system_call(SYS_WRITE, STDERR, " ecx: ", 5);
            ecxAsString = itoa(ebx);
            system_call(SYS_WRITE, STDERR, ecxAsString, strlen(ecxAsString));
        }
        if (edx != -1)
        {
            system_call(SYS_WRITE, STDERR, " edx: ", 5);
            edxAsString = itoa(ebx);
            system_call(SYS_WRITE, STDERR, edxAsString, strlen(edxAsString));
        }
        system_call(SYS_WRITE, STDERR, "\n", sizeof(char));
    }
}

int main(int argc, char **argv)
{
    int i, returnValue;
    char isInDebugMode = 0;
    char input[2];
    input[1] = '\0';
    int inputFileDescriptor;
    int outputFileDescriptor;
    for (i = 1; i < argc; i++)
    {
        if (strncmp("-D", argv[i], 2) == 0)
        {
            isInDebugMode = 1;
        }
    }
    for (i = 1; i < argc; i++)
    {
        if (strncmp("-i", argv[i], 2) == 0)
        {
            if ((inputFileDescriptor = system_call(SYS_OPEN, argv[i] + 2, O_RDONLY)) < 0)
            {
                debug_sys(SYS_OPEN, isInDebugMode, "Error in open: ", (int)argv[i] + 2, O_RDONLY, -1);
                system_call(SYS_EXIT, 0x55);
            }
        }
        debug_sys(SYS_OPEN, isInDebugMode, "Opening input file: ", (int)argv[i] + 2, O_RDONLY, -1);
        if (strncmp("-o", argv[i], 2) == 0)
        {
            if ((outputFileDescriptor = system_call(SYS_OPEN, argv[i] + 2, O_WRONLY|O_TRUNC|O_CREAT, S_IRWXU)) < 0)
            {
                debug_sys(SYS_OPEN, isInDebugMode, "Error in close: ", (int)argv[i] + 2, O_WRONLY|O_TRUNC|O_CREAT, S_IRWXU);
                system_call(SYS_EXIT, 0x55);
            }
        }
        debug_sys(SYS_OPEN, isInDebugMode, "opening output file: ", (int)argv[i] + 2, O_WRONLY|O_TRUNC|O_CREAT, S_IRWXU);
    }
    while ((returnValue = system_call(SYS_READ, inputFileDescriptor, input, 1)) != 0)
    {
        if (returnValue < 0)
        {
            debug_sys(SYS_READ, isInDebugMode, "Error in READ: ", inputFileDescriptor, (int)input, 1);
            system_call(SYS_EXIT, ERROR);
        }
        debug_sys(SYS_READ, isInDebugMode, "READ: ", inputFileDescriptor, (int)input, 1);
        char inputChar = input[0];
        input[0] = (inputChar >= LOWER_CASE_LOW_BOUND && inputChar <= LOWER_CASE_HIGH_BOUND)
                       ? inputChar - LOWER_CASE_TO_HIGHER_CASE_DIFF
                       : inputChar;
        if (system_call(SYS_WRITE, outputFileDescriptor, input, sizeof(char)) < 0)
        {
            debug_sys(SYS_WRITE, isInDebugMode, "Error in WRITE: ", (int)input, sizeof(char), -1);
            system_call(SYS_EXIT, ERROR);
        }
        debug_sys(SYS_WRITE, isInDebugMode, "WRITE: ", (int)input, sizeof(char), -1);
    }
    if (inputFileDescriptor != STDIN)
    {
        if (system_call(SYS_CLOSE, inputFileDescriptor) < 0)
        {
            debug_sys(SYS_CLOSE, isInDebugMode, "Error while trying to close input file", inputFileDescriptor, -1, -1);
            system_call(SYS_EXIT, ERROR);
        }
        debug_sys(SYS_CLOSE, isInDebugMode, "Closing input file: ", inputFileDescriptor, -1, -1);
    }

    if (outputFileDescriptor != STDIN)
    {
        if (system_call(SYS_CLOSE, outputFileDescriptor) < 0)
        {
            debug_sys(SYS_CLOSE, isInDebugMode, "Error while trying to close input file", outputFileDescriptor, -1, -1);
            system_call(SYS_EXIT, ERROR);
        }
        debug_sys(SYS_CLOSE, isInDebugMode, "Closing output file: ", outputFileDescriptor, -1, -1);
    }
    system_call(SYS_EXIT, 0);
    return 0;
}