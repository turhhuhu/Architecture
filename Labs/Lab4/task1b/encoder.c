
#include "lab4_util.h"

#define O_RDONLY 0
#define O_WRONLY 1
#define O_RDWR 2
#define O_CREAT 64
#define O_APPEND 1024
#define  S_IRWXU  00700
#define O_TRUNC 01000
#define SYS_EXIT 1
#define SYS_READ 3
#define SYS_WRITE 4
#define SYS_OPEN 5
#define SYS_CLOSE 6
#define STDIN 0
#define STDOUT 1
#define STDERR 2
#define MAX_INPUT_SIZE 1024
#define LOWER_CASE_LOW_BOUND 97
#define LOWER_CASE_HIGH_BOUND 122
#define LOWER_CASE_TO_HIGHER_CASE_DIFF 32
#define NULL 0

void debug_sys(int SysCallNumber, int isInDebugMode, char *msg, int returnValue)
{
    if (isInDebugMode)
    {
        char *sysCallNumberAsString = NULL; 
        char *sysCallReturnValueAsString = NULL;
        system_call(SYS_WRITE, STDERR, msg, strlen(msg));
        system_call(SYS_WRITE, STDERR, "\n", sizeof(char));
        sysCallNumberAsString = itoa(SysCallNumber);
        system_call(SYS_WRITE, STDERR, "call number: ", 13);
        system_call(SYS_WRITE, STDERR, sysCallNumberAsString, strlen(sysCallNumberAsString));
        sysCallReturnValueAsString = itoa(returnValue);
        system_call(SYS_WRITE, STDERR, " return value: ", 15);
        system_call(SYS_WRITE, STDERR, sysCallReturnValueAsString, strlen(sysCallReturnValueAsString));
        system_call(SYS_WRITE, STDERR, "\n", sizeof(char));
    }
}

int main(int argc, char **argv)
{
    int i, returnValue;
    char isInDebugMode = 0;
    char input[2];
    input[1] = '\0';
    int inputFileDescriptor = STDIN;
    int outputFileDescriptor = STDOUT;
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
                debug_sys(SYS_OPEN, isInDebugMode, "Error while opening input file ", inputFileDescriptor);
                system_call(SYS_EXIT, 0x55);
            }
            debug_sys(SYS_OPEN, isInDebugMode, "Opening input file: ", inputFileDescriptor);
        }
        if (strncmp("-o", argv[i], 2) == 0)
        {
            if ((outputFileDescriptor = system_call(SYS_OPEN, argv[i] + 2, O_WRONLY|O_TRUNC|O_CREAT, S_IRWXU)) < 0)
            {
                debug_sys(SYS_OPEN, isInDebugMode, "Error while opening output file ", outputFileDescriptor);
                system_call(SYS_EXIT, 0x55);
            }
            debug_sys(SYS_OPEN, isInDebugMode, "Opening output file: ", outputFileDescriptor);
        }
    }
    while ((returnValue = system_call(SYS_READ, inputFileDescriptor, input, 1)) != 0)
    {
        if (returnValue < 0)
        {
            debug_sys(SYS_READ, isInDebugMode, "Error IN READ: ", returnValue);
            system_call(SYS_EXIT, 0x55);
        }
        debug_sys(SYS_READ, isInDebugMode, "READ: ", returnValue);
        char inputChar = input[0];
        input[0] = (inputChar >= LOWER_CASE_LOW_BOUND && inputChar <= LOWER_CASE_HIGH_BOUND)
                       ? inputChar - LOWER_CASE_TO_HIGHER_CASE_DIFF
                       : inputChar;
        if ((returnValue = system_call(SYS_WRITE, outputFileDescriptor, input, sizeof(char))) < 0)
        {
            debug_sys(SYS_WRITE, isInDebugMode, "Error IN WRITE: ", returnValue);
            system_call(SYS_EXIT, 0x55);
        }
        debug_sys(SYS_WRITE, isInDebugMode, "WRITE: ", returnValue);
    }

    if (inputFileDescriptor != STDIN)
    {
        if ((returnValue = system_call(SYS_CLOSE, inputFileDescriptor)) < 0)
        {
            debug_sys(SYS_CLOSE, isInDebugMode, "Error when trying to close input file: ", returnValue);
            system_call(SYS_EXIT, 0x55);
        }
        debug_sys(SYS_CLOSE, isInDebugMode, "Closing input file: ", returnValue);
    }
    if (outputFileDescriptor != STDOUT)
    {
        if ((returnValue = system_call(SYS_CLOSE, outputFileDescriptor)) < 0)
        {
            system_call(SYS_EXIT, 0x55);
        }
        debug_sys(SYS_CLOSE, isInDebugMode, "Closing output file: ", returnValue);
    }
    system_call(SYS_EXIT, 0);

    return 0;
}