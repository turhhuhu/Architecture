#include "LineParser.h"
#include <stdlib.h>
#include <stdio.h>
#include <string.h>
#include <unistd.h>
#include <linux/limits.h>
#include <sys/wait.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <fcntl.h>
#define MAX_INPUT_SIZE 2048

typedef struct pairList{
    char *name;
    char *value;
    struct pairList *next;
}pairList;

void freePairList(pairList *pairs)
{
    if(pairs != NULL)
    {
        freePairList(pairs -> next);
        free(pairs -> name);
        free(pairs -> value);
        free(pairs);
    }
}

void addPair(pairList **pairsP, char* name, char* value)
{
    pairList *pairs = *pairsP;
    if(pairs == NULL)
    {
        pairs = malloc(sizeof(pairList));
        char *nameCpy = malloc(strlen(name)+ 1);
        char *valueCpy = malloc(strlen(value) + 1);
        strcpy(nameCpy, name);
        strcpy(valueCpy, value);
        pairs -> name = nameCpy;
        pairs -> value = valueCpy;
        pairs -> next = NULL;
        *pairsP = pairs;
        return;
    }
    while(pairs -> next != NULL && strcmp(pairs -> name, name) != 0)
    {
        pairs = pairs -> next;
    }
    if(strcmp(pairs -> name, name) == 0)
    {
        char *valueCpy = malloc(strlen(value) + 1);
        strcpy(valueCpy, value);
        pairs -> value = valueCpy;
        return;
    }
    pairList *newPair = malloc(sizeof(pairList));
    char *nameCpy = malloc(strlen(name) + 1);
    char *valueCpy = malloc(strlen(value) + 1);
    strcpy(nameCpy, name);
    strcpy(valueCpy, value);
    newPair -> name = nameCpy;
    newPair -> value = valueCpy;
    newPair -> next = NULL;
    pairs -> next = newPair;

}

void printPairs(pairList **pairsP)
{
    pairList *pairs = *pairsP;
    while(pairs != NULL)
    {
        printf("name: %s\n", pairs -> name);
        printf("value: %s\n\n", pairs -> value);
        pairs = pairs -> next;
    }
}
char* getValueFromName(pairList *pairs, char *name)
{
    while(pairs != NULL)
    {
        if(strcmp(name, pairs->name) == 0)
        {
            return pairs -> value;
        }
    }
    return NULL;
}

void execute(cmdLine *pCmdLine, char isInDebugMode, pairList **pairs)
{
    int i;
    for(i = 1; i < pCmdLine -> argCount; i++)
    {
        if(strncmp("$", pCmdLine->arguments[i], 1) == 0)
        {
            char *value = getValueFromName(*pairs, pCmdLine -> arguments[i] + 1);
            if(value == NULL)
            {
                fprintf(stderr, "variable not found\n");
                return;
            }
            replaceCmdArg(pCmdLine, i, value);
        }
    }
    if(strncmp("cd", pCmdLine->arguments[0], 2) == 0)
    {
        if(strcmp("~", pCmdLine -> arguments[1]) == 0)
        {
            char *path = getenv("HOME");
            puts(path);
            if(chdir(path) < 0)
            {
                perror("error when using chdir with ~");
            }
            return;
        }
        if(chdir(pCmdLine->arguments[1]) < 0)
        {
            perror("error when using chdir");
        }
        return;
    }
    if(strncmp("set", pCmdLine->arguments[0], 3) == 0)
    {
        if(pCmdLine -> argCount != 3)
        {
            perror("error when using chdir");
            return;
        }
        addPair(pairs, pCmdLine -> arguments[1], pCmdLine -> arguments[2]);
        return;
    }
    if(strncmp("vars", pCmdLine->arguments[0], 4) == 0)
    {
        printPairs(pairs);
        return;
    }
    int pid = fork();
    if (pid < -1)
    {
        if (isInDebugMode)
        {
            printf("error when using fork with pid");
        }
        return;
    }
    if (isInDebugMode)
    {
        fprintf(stderr, "pid: %d\n", pid);
        fprintf(stderr, "pid: %s\n", pCmdLine->arguments[0]);
    }
    if (pid == 0)
    {   
        if(pCmdLine -> inputRedirect != NULL)
        {
            close(STDIN_FILENO);
            open(pCmdLine -> inputRedirect, O_RDONLY);
        }
        if(pCmdLine -> outputRedirect != NULL)
        {
            close(STDOUT_FILENO);
            open(pCmdLine -> outputRedirect, O_WRONLY|O_CREAT, S_IRWXU);
        }    
        if (execvp(pCmdLine->arguments[0], pCmdLine->arguments) < 0)
        {
            perror("error in execv");
            _exit(1);
        }
    }
    if(pCmdLine -> blocking == 1)
    {
        waitpid(pid, NULL, 0);
    }
}

int main(int argc, char **argv)
{
    char cwd[PATH_MAX];
    char input[MAX_INPUT_SIZE];
    cmdLine *head = NULL;
    char isInDebugMode = 0;
    int i;
    pairList **pairs = malloc(sizeof(pairList*));
    *pairs = NULL;
    for (i = 0; i < argc; i++)
    {
        if (strcmp("-d", argv[i]) == 0)
        {
            isInDebugMode = 1;
        }
    }
    while (1)
    {
        getcwd(cwd, PATH_MAX);
        puts(cwd);
        fgets(input, MAX_INPUT_SIZE, stdin);
        if (strcmp(input, "quit\n") == 0)
        {
            break;
        }
        head = parseCmdLines(input);
        execute(head, isInDebugMode, pairs);
        freeCmdLines(head);
        head = NULL;
    }
    if(head != NULL)
    {
        freeCmdLines(head);
    }
    if(*pairs != NULL)
    {
        freePairList(*pairs);
    }
    free(pairs);

    return 0;
}