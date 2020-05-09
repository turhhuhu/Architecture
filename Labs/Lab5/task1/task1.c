#include "LineParser.h"
#include <stdlib.h>
#include <stdio.h>
#include <string.h>
#include <unistd.h>
#include <linux/limits.h>
#include <sys/wait.h>
#define MAX_INPUT_SIZE 2048

void execute(cmdLine *pCmdLine, char isInDebugMode)
{
    if(strncmp("cd", pCmdLine->arguments[0], 2) == 0)
    {
        if(chdir(pCmdLine->arguments[1]) < 0)
        {
            perror("error when using chdir");
        }
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
        //can add sscanf here
        if (strcmp(input, "quit\n") == 0)
        {
            break;
        }
        head = parseCmdLines(input);
        execute(head, isInDebugMode);
        freeCmdLines(head);
        head = NULL;
    }
    if(head != NULL)
    {
        freeCmdLines(head);
    }

    return 0;
}