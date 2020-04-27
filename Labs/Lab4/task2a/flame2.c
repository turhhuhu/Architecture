
#include "lab4_util.h"
#define O_RDONLY 0
#define O_WRONLY 1
#define O_RDWR 2
#define O_CREAT 64
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
#define MAX_INPUT_SIZE 1024
#define LOWER_CASE_LOW_BOUND 97
#define LOWER_CASE_HIGH_BOUND 122
#define LOWER_CASE_TO_HIGHER_CASE_DIFF 32
#define NULL 0
#define MAX_DIRECTORY_DATA 8192
#define GETDENTS 141

enum
{
    DT_UNKNOWN = 0,
#define DT_UNKNOWN DT_UNKNOWN
    DT_FIFO = 1,
#define DT_FIFO DT_FIFO
    DT_CHR = 2,
#define DT_CHR DT_CHR
    DT_DIR = 4,
#define DT_DIR DT_DIR
    DT_BLK = 6,
#define DT_BLK DT_BLK
    DT_REG = 8,
#define DT_REG DT_REG
    DT_LNK = 10,
#define DT_LNK DT_LNK
    DT_SOCK = 12,
#define DT_SOCK DT_SOCK
    DT_WHT = 14
#define DT_WHT DT_WHT
};

typedef struct ent
{
    long inode;
    int offset;
    unsigned short len;
    char name[];
} ent;

void debug_sys(int SysCallNumber, int isInDebugMode, char *msg, int returnValue)
{
    if (isInDebugMode)
    {
        char *sysCallNumberAsString = NULL;
        char *sysCallReturnValueAsString = NULL;
        system_call(SYS_WRITE, STDERR, "\n", sizeof(char));
        system_call(SYS_WRITE, STDERR, msg, strlen(msg));
        sysCallNumberAsString = itoa(SysCallNumber);
        system_call(SYS_WRITE, STDERR, " call number: ", 13);
        system_call(SYS_WRITE, STDERR, sysCallNumberAsString, strlen(sysCallNumberAsString));
        sysCallReturnValueAsString = itoa(returnValue);
        system_call(SYS_WRITE, STDERR, " return value: ", 15);
        system_call(SYS_WRITE, STDERR, sysCallReturnValueAsString, strlen(sysCallReturnValueAsString));
        system_call(SYS_WRITE, STDERR, "\n", sizeof(char));
    }
}

void debug_dirent(unsigned short length, char *fileName, int isInDebugMode)
{
    if (isInDebugMode)
    {
        char *lengthAsString = itoa(length);
        system_call(SYS_WRITE, STDERR, "\n", sizeof(char));
        system_call(SYS_WRITE, STDERR, "File length: ", 14);
        system_call(SYS_WRITE, STDERR, lengthAsString, strlen(lengthAsString));
        system_call(SYS_WRITE, STDERR, " File name: ", 12);
        system_call(SYS_WRITE, STDERR, fileName, strlen(fileName));
        system_call(SYS_WRITE, STDERR, "\n", sizeof(char));
        system_call(SYS_WRITE, STDERR, "\n", sizeof(char));
    }
}

void sys_write(int fileDescriptor, char *msg, int length, int isInDebugMode)
{
    int returnValue;
    returnValue = system_call(SYS_WRITE, fileDescriptor, msg, length);
    if (returnValue < 0)
    {
        system_call(SYS_EXIT, 0x55);
    }
    debug_sys(SYS_WRITE, isInDebugMode, "WRITE: ", returnValue);
}

int main(int argc, char **argv)
{
    int i;
    char isInDebugMode = 0;
    char dirBuf[MAX_DIRECTORY_DATA];
    ent *entPointer = NULL;
    int count;
    int curretDirectoryFileDescriptor;
    for (i = 1; i < argc; i++)
    {
        if (strncmp("-D", argv[i], 2) == 0)
        {
            isInDebugMode = 1;
        }
    }
    if ((curretDirectoryFileDescriptor = system_call(SYS_OPEN, ".", O_RDONLY)) < 0)
    {
        system_call(SYS_EXIT, 0x55);
    }
    if ((count = system_call(GETDENTS, curretDirectoryFileDescriptor, dirBuf, MAX_DIRECTORY_DATA)) < 0)
    {
        system_call(SYS_EXIT, 0x55);
    }
    i = 0;
    while (i < count)
    {
        entPointer = (ent *)(dirBuf + i);
        debug_dirent(entPointer->len, entPointer->name, isInDebugMode);
        if(strcmp(entPointer -> name, ".") != 0 && strcmp(entPointer -> name, ".."))
        {
            sys_write(STDOUT, entPointer->name, strlen(entPointer->name), isInDebugMode);
            sys_write(STDOUT, "\n", 1, isInDebugMode);
        }
        i += entPointer -> len;
    }
    system_call(SYS_CLOSE, curretDirectoryFileDescriptor);

    return 0;
}