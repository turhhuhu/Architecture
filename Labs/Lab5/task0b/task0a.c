#include "LineParser.h"
#include <stdlib.h>
#include <stdio.h>
#include <string.h>
#include <unistd.h>
#include <linux/limits.h>
#define MAX_INPUT_SIZE 2048

void execute(cmdLine *pCmdLine)
{

    if (execvp(pCmdLine->arguments[0], pCmdLine->arguments) < 0)
    {
        perror("error in execv");
        exit(1);
    }
}

int main(int argc, char **argv)
{
    char cwd[PATH_MAX];
    char input[MAX_INPUT_SIZE];
    cmdLine *head = NULL;
    while (1)
    {
        getcwd(cwd, PATH_MAX);
        puts(cwd);
        fgets(input, MAX_INPUT_SIZE, stdin);
        //can add sscanf here
        if(strcmp(input, "quit\n") == 0)
        {
            break;
        }
        head = parseCmdLines(input);
        execute(head);
        freeCmdLines(head);
    }
    freeCmdLines(head);

    return 0;
}