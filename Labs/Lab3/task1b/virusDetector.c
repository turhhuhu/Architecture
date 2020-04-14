
#include <stdio.h>
#include <stdlib.h>
#define SIGNATURES_MAX_FILE_SIZE 1024

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

void printHex(unsigned char* sig, FILE *output, int size)
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
    printHex(virus -> sig, output, virus->SigSize);
    fputc('\n', output);
    fputc('\n', output);
}

void freeVirus(virus *virus)
{
    free(virus -> sig);
    free(virus);
}

void printMenu()
{
    printf("1) Load signatures\n");
    printf("2) Print signatures\n");
    printf("3) Quit\n");
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
        if (fread(virusPtr, 1, 18, buffer) != 18)
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
        list_free(virus_list -> nextVirus);
        freeVirus(virus_list -> vir);
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

int main(int argc, char **argv)
{
    int input;
    FILE *buffer = NULL;
    link *head = NULL;
    char inputAsString[3];
    char quit = 0;
    while (!quit)
    {
        printf("please choose a function: \n");
        printMenu();
        fgets(inputAsString, 3, stdin);
        sscanf(inputAsString, "%d *[^\n]", &input);
        switch (input)
        {
        case 1:
            if (buffer != NULL)
            {
                fclose(buffer);
            }
            buffer = loadSignatures();
            if(head != NULL)
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
            quit = 1;
            break;

        default:
            printf("input out of boundary\n");
        }
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