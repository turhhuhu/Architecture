#include "LineParser.h"
#include <stdlib.h>
#include <stdio.h>
#include <string.h>
#include <unistd.h>
#include <linux/limits.h>
#include <sys/wait.h>
#define MAX_INPUT_SIZE 2048
#define TERMINATED  -1
#define RUNNING 1
#define SUSPENDED 0

typedef struct process{
    cmdLine* cmd;                         /* the parsed command line*/
    pid_t pid; 		                  /* the process id that is running the command*/
    int status;                           /* status of the process: RUNNING/SUSPENDED/TERMINATED */
    struct process *next;	                  /* next process in chain */
} process;

void addProcess(process** process_list, cmdLine* cmd, pid_t pid)
{
    process* nextProcess = malloc(sizeof(process));
    nextProcess -> cmd = cmd;
    nextProcess -> pid = pid;
    nextProcess -> status = RUNNING;
    nextProcess -> next = NULL;
    process  *head = *process_list;
    if(head == NULL)
    {
        *process_list = nextProcess;
    }
    else
    {
        while(head -> next != NULL)
        {
            head = head -> next;
        }
        head -> next = nextProcess;
    }
}

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
    process** processList = malloc(sizeof(process*));
    *processList = NULL;
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