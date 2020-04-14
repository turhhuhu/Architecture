
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#define SIGNATURES_MAX_FILE_SIZE 1024
#define MAX_INPUT_SIZE 1024
#define BUFFER_MAX_SIZE 10000
#define SIG_SIZE_PLUS_VIRUS_NAME 18

typedef struct virus
{
    unsigned short SigSize;
    char virusName[16];
    unsigned char *sig;
} virus;

typedef struct link link;

struct link
{
    link *nextVirus;
    virus *vir;
};

void printHex(unsigned char *sig, FILE *output, int size)
{
    int i;
    for (i = 0; i < size; i++)
    {
        fprintf(output, "%X ", sig[i]);
    }
}

void printVirus(virus *virus, FILE *output)
{
    fputs("Virus name: ", output);
    fputs(virus->virusName, output);
    fputc('\n', output);
    fputs("Virus size: ", output);
    fprintf(output, "%d", virus->SigSize);
    fputc('\n', output);
    fputs("signature:\n", output);
    printHex(virus->sig, output, virus->SigSize);
    fputc('\n', output);
    fputc('\n', output);
}

void freeVirus(virus *virus)
{
    free(virus->sig);
    free(virus);
}

void printMenu()
{
    printf("1) Load signatures\n");
    printf("2) Print signatures\n");
    printf("3) Detect viruses\n");
    printf("4) Fix file\n");
    printf("5) Quit\n");
}

void list_print(link *virus_list, FILE *output)
{
    if (virus_list->vir != NULL)
    {
        printVirus(virus_list->vir, output);
    }
    virus_list = virus_list->nextVirus;
    while (virus_list != NULL)
    {
        if (virus_list->vir != NULL)
        {
            printVirus(virus_list->vir, output);
        }
        virus_list = virus_list->nextVirus;
    }
}

// adding to the end of the list. (can change to adding to the start)
link *list_append(link *virus_list, virus *data)
{
    link *newLink = NULL;
    link *head = virus_list;
    if (virus_list == NULL)
    {
        newLink = (link *)malloc(sizeof(link));
        newLink->vir = data;
        newLink->nextVirus = NULL;
        return newLink;
    }
    while (virus_list->nextVirus != NULL)
    {
        virus_list = virus_list->nextVirus;
    }
    newLink = (link *)malloc(sizeof(link));
    newLink->vir = data;
    newLink->nextVirus = NULL;
    virus_list->nextVirus = newLink;
    return head;
}

virus *readVirus(FILE *buffer)
{
    if (!feof(buffer))
    {
        unsigned short size;
        virus *virusPtr = (virus *)malloc(sizeof(virus));
        if (fread(virusPtr, 1, SIG_SIZE_PLUS_VIRUS_NAME, buffer) != SIG_SIZE_PLUS_VIRUS_NAME)
        {
            free(virusPtr);
            return NULL;
        }
        size = virusPtr->SigSize;
        virusPtr->sig = (unsigned char *)malloc(size);
        fread(virusPtr->sig, 1, size, buffer);
        return virusPtr;
    }
    return NULL;
}

FILE *loadSignatures()
{
    char input[SIGNATURES_MAX_FILE_SIZE];
    char fileName[SIGNATURES_MAX_FILE_SIZE];
    fgets(input, SIGNATURES_MAX_FILE_SIZE, stdin);
    sscanf(input, "%s *[^\n]", fileName);
    return fopen(fileName, "r");
}

void list_free(link *virus_list)
{
    if (virus_list != NULL)
    {
        list_free(virus_list->nextVirus);
        freeVirus(virus_list->vir);
        free(virus_list);
    }
}

void printSignatures(link *head)
{
    list_print(head, stdout);
}

link *createVirusList(FILE *buffer)
{
    link *head = NULL;
    virus *v = NULL;
    if (!feof(buffer))
    {
        v = readVirus(buffer);
        if (v != NULL)
        {
            head = list_append(head, v);
        }
    }
    while (!feof(buffer))
    {
        v = readVirus(buffer);
        if (v != NULL)
        {
            head = list_append(head, v);
        }
    }
    return head;
}

unsigned int loadSuspected(char *charBuffer, char *fileName)
{
    FILE *buffer = fopen(fileName, "r");
    unsigned int i = fread(charBuffer, 1, BUFFER_MAX_SIZE, buffer);
    charBuffer[i] = '\0';
    fclose(buffer);
    return i;
}

void detect_virus(char *buffer, unsigned int size, link *virus_list)
{
    char *bufferHead = buffer;
    link *head = virus_list;
    int i, compare;
    for (i = 0; i < size; i++)
    {
        while (head != NULL)
        {
            if((i + head -> vir -> SigSize) <= size)
            {
                compare = memcmp(bufferHead, head->vir->sig, head->vir->SigSize);
                if (compare == 0)
                {
                    printf("starting byte: %d\n", i);
                    printf("virus name: %s\n", head->vir->virusName);
                    printf("virus signature size: %d\n\n", head->vir->SigSize);
                    i += head -> vir -> SigSize;
                    bufferHead = bufferHead + head -> vir -> SigSize;
                }
            }

            head = head->nextVirus;
        }
        head = virus_list;
        bufferHead++;
    }
}

void kill_virus(char *fileName, int signitureOffset, int signitureSize)
{
    int i;
    char nopArray[signitureSize];
    FILE* buffer = fopen(fileName, "r+");
    fseek(buffer, signitureOffset, SEEK_SET);
    for (i = 0; i< signitureSize; i++)
    {
        nopArray[i] = 0x90;
    }
    fwrite(nopArray, 1, signitureSize, buffer);
    fclose(buffer);
}

int main(int argc, char **argv)
{
    int input;
    unsigned int size;
    char* infectedFIle = argv[1];
    FILE *buffer = NULL;
    char *charBuffer = NULL;
    link *head = NULL;
    char inputAsString[MAX_INPUT_SIZE];
    int sigOffset = 0;
    int sigSize = 0;
    char quit = 0;
    while (!quit)
    {
        printf("please choose a function: \n");
        printMenu();
        fgets(inputAsString, MAX_INPUT_SIZE, stdin);
        sscanf(inputAsString, "%d *[^\n]", &input);
        switch (input)
        {
        case 1:
            if (buffer != NULL)
            {
                fclose(buffer);
            }
            buffer = loadSignatures();
            if (head != NULL)
            {
                list_free(head);
            }
            head = createVirusList(buffer);
            break;

        case 2:
            if (head != NULL)
            {
                printSignatures(head);
            }
            break;

        case 3:
            if (charBuffer != NULL)
            {
                free(charBuffer);
            }
            charBuffer = (char *)malloc(BUFFER_MAX_SIZE);
            size = loadSuspected(charBuffer, infectedFIle);
            detect_virus(charBuffer, size, head);
            break;
        case 4:
            printf("enter signature offset: \n");
            fgets(inputAsString, MAX_INPUT_SIZE, stdin);
            sscanf(inputAsString, "%d *[^\n]", &sigOffset);
            printf("enter signature size: \n");
            fgets(inputAsString, MAX_INPUT_SIZE, stdin);
            sscanf(inputAsString, "%d *[^\n]", &sigSize);
            kill_virus(infectedFIle, sigOffset, sigSize);
            break;
        case 5:
            quit = 1;
            break;

        default:
            printf("input out of boundary\n");
        }
    }
    if (charBuffer != NULL)
    {
        free(charBuffer);
    }
    if (head != NULL)
    {
        list_free(head);
    }
    if (buffer != NULL)
    {
        fclose(buffer);
    }

    return 0;
}