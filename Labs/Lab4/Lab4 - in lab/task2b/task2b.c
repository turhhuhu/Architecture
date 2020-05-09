#include "lab4_util.h"
#define O_RDONLY 0x0
#define O_WRONLY 0x1
#define O_RDWR 0x2
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
#define ERROR 0x55

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
    if (system_call(SYS_WRITE, fileDescriptor, msg, length) < 0)
    {
        system_call(SYS_EXIT, 0x55);
    }
    debug_sys(SYS_WRITE, isInDebugMode, "WRITE: ", fileDescriptor, (int)msg, length);
}

int main(int argc, char **argv)
{
    int i;
    char isInDebugMode = 0;
    char dirBuf[MAX_DIRECTORY_DATA];
    ent *entPointer = NULL;
    int count;
    int currentDirectoryFileDescriptor;
    char *prefix = NULL;
    char *type = NULL;
    int fileType;
    for (i = 1; i < argc; i++)
    {
        if (strncmp("-D", argv[i], 2) == 0)
        {
            isInDebugMode = 1;
        }
    }
    for (i = 1; i < argc; i++)
    {
        if (strncmp("-p", argv[i], 2) == 0)
        {
            prefix = argv[i] + 2;
        }
    }
    if((currentDirectoryFileDescriptor = system_call(SYS_OPEN, ".", O_RDONLY)) < 0)
    {
        debug_sys(SYS_OPEN, isInDebugMode, "Error in open: ", (int)".", O_RDONLY, -1);
        system_call(SYS_EXIT, 0x55);
    }
    debug_sys(SYS_OPEN, isInDebugMode, "OPEN: ", (int)".", O_RDONLY, -1);
    if((count = system_call(GETDENTS, currentDirectoryFileDescriptor, dirBuf, MAX_DIRECTORY_DATA)) < 0)
    {
        debug_sys(SYS_OPEN, isInDebugMode, "Error in GETDENTS: ", currentDirectoryFileDescriptor, (int)dirBuf, MAX_DIRECTORY_DATA);
        system_call(SYS_EXIT, 0x55);
    }
    debug_sys(GETDENTS, isInDebugMode, "GETDENTS ", currentDirectoryFileDescriptor, (int)dirBuf, MAX_DIRECTORY_DATA);
    i = 0;
    while(i < count)
    {
        entPointer = (ent*)(dirBuf + i);
        debug_dirent(entPointer -> len, entPointer -> name, isInDebugMode);
        fileType = *(dirBuf + entPointer -> len - 1);
        if(strcmp(entPointer -> name, ".") != 0 && strcmp(entPointer -> name, "..") != 0)
        {
            if(prefix != NULL)
            {
                if(strncmp(entPointer -> name, prefix, strlen(prefix)) != 0)
                {
                    i += entPointer -> len;
                    continue;
                }
            }
            sys_write(STDOUT, "File name: ", 12, isInDebugMode);
            sys_write(STDOUT, entPointer -> name, strlen(entPointer -> name), isInDebugMode);
            sys_write(STDOUT, "\n", 1, isInDebugMode);
            sys_write(STDOUT, "File type: ", 12, isInDebugMode);
            type =
                    (fileType == DT_REG) ? "regular" 
                    : (fileType == DT_DIR) ? "directory" 
                    : (fileType == DT_FIFO) ? "FIFO" 
                    : (fileType == DT_SOCK) ? "socket" 
                    : (fileType == DT_LNK) ? "symlink" 
                    : (fileType == DT_BLK) ? "block dev" 
                    : (fileType == DT_CHR) ? "char dev" 
                    : (fileType == DT_UNKNOWN) ? "unknown" 
                    : "???";
            sys_write(STDOUT, type, strlen(type), isInDebugMode);
            sys_write(STDOUT, "\n", sizeof(char), isInDebugMode);
            sys_write(STDOUT, "\n", sizeof(char), isInDebugMode);
        }
        i += entPointer -> len;
    }
    if(system_call(SYS_CLOSE, currentDirectoryFileDescriptor) < 0)
    {
        debug_sys(SYS_CLOSE, isInDebugMode, "error in closing dir descriptor", currentDirectoryFileDescriptor, -1 ,-1);
    }
    system_call(SYS_EXIT, 0);
    return 0;
}