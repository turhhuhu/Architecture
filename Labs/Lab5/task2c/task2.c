#include "LineParser.h"
#include <stdlib.h>
#include <stdio.h>
#include <string.h>
#include <unistd.h>
#include <linux/limits.h>
#include <wait.h>
#include <sys/types.h>
#include <signal.h>
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

void suspendProcess(int pid)
{
    if(kill(pid, SIGTSTP) < 0)
    {
        perror("error in kill");
    }
}

void killProcess(int pid)
{
    if(kill(pid, SIGINT) < 0)
    {
        perror("error in kill");
    }
}

void wakeProcess(int pid)
{
    if(kill(pid, SIGCONT) < 0)
    {
        perror("error in kill");
    }
}



void freeProcessList(process* process_list)
{
    if(process_list -> next != NULL)
    {
        freeProcessList(process_list -> next);
    }
    freeCmdLines(process_list->cmd);
    free(process_list);
}

void updateProcessStatus(process* process_list, int pid, int status)
{
    process *head = process_list;
    while(head != NULL && head -> pid != pid)
    {
        head = head -> next;
    }
    if(head != NULL)
    {
        head -> status = status;
    }
}

void updateProcessList(process **process_list)
{
    int status = -1;
    process *head = NULL;
    if(process_list == NULL)
    {
        return;
    }
    head = *process_list;
    while(head != NULL)
    {
        waitpid(head -> pid, &status, WNOHANG | WUNTRACED);
        if(WIFSIGNALED(status) || WIFEXITED(status))
        {
            updateProcessStatus(*process_list, head -> pid, TERMINATED);
        }
        else if(WIFSTOPPED(status))
        {
            updateProcessStatus(*process_list, head -> pid, SUSPENDED);
        }
        else
        {
            updateProcessStatus(*process_list, head -> pid, RUNNING);
        }
        head = head -> next;
    }
}

void freeTerminatedProcess(process** process_list, int pid)
{
    process *head = *process_list;
    process *beforeHead = *process_list;
    if(head == NULL)
    {
        return;
    }
    if(head != NULL && head -> pid == pid)
    {
        *process_list = head -> next;
        freeCmdLines(head -> cmd);
        free(head);
        return;
    }
    head = head -> next;
    while(head != NULL && head -> pid != pid)
    {
        head = head -> next;
        beforeHead = beforeHead -> next;
    }
    if(head != NULL)
    {
        beforeHead -> next = head -> next;
        freeCmdLines(head -> cmd);
        free(head);
    }
}

void printProcessList(process** process_list)
{
    process* head = *process_list;
    process* processHolder = NULL;
    if(head == NULL)
    {
        printf("no running process\n");
        return;
    }
    updateProcessList(process_list);
    int i;
    int status;
    char *statusAsString = NULL;
    printf("Index   PID       command(with arguments)  STATUS\n");
    for (i = 1; head != NULL; i++)
    {
        status = head -> status;
        statusAsString = status == TERMINATED ? "Terminated" : status == RUNNING ? "Running" : "Suspended";
        printf("%d)      %d    %s                       %s\n", i, head->pid, *(head->cmd->arguments), statusAsString);
        processHolder = head -> next;
        if(status == TERMINATED)
        {
            freeTerminatedProcess(process_list, head -> pid);
        }
        head = processHolder;
    }
}


void addProcess(process** process_list, cmdLine* cmd, pid_t pid)
{
    process* nextProcess = (process*)malloc(sizeof(process));
    nextProcess -> cmd = cmd;
    nextProcess -> pid = pid;
    nextProcess -> status = RUNNING;
    nextProcess -> next = NULL;
    if(*process_list == NULL)
    {
        *process_list = nextProcess;
    }
    else
    {
        process *head = *process_list;
        while(head -> next != NULL)
        {
            head = head -> next;
        }
        head -> next = nextProcess;
    }
}

void execute(cmdLine *pCmdLine, char isInDebugMode, process** processList)
{
    if(strncmp("cd", pCmdLine->arguments[0], 2) == 0)
    {
        if(chdir(pCmdLine->arguments[1]) < 0)
        {
            perror("error when using chdir");
        }
        freeCmdLines(pCmdLine);
        return;
    }
    if(strcmp("procs", pCmdLine->arguments[0]) == 0)
    {
        printProcessList(processList);
        freeCmdLines(pCmdLine);
        return;
    }
    if(strcmp("kill", pCmdLine -> arguments[0]) == 0)
    {
        killProcess(atoi(pCmdLine -> arguments[1]));
        freeCmdLines(pCmdLine);
        return;
    }
    if(strcmp("suspend", pCmdLine -> arguments[0]) == 0)
    {
        suspendProcess(atoi(pCmdLine -> arguments[1]));
        freeCmdLines(pCmdLine);
        return;
    }
    if(strcmp("wake", pCmdLine -> arguments[0]) == 0)
    {
        wakeProcess(atoi(pCmdLine -> arguments[1]));
        freeCmdLines(pCmdLine);
        return;
    }
    int pid = fork();
    if(pid > 0)
    {
        addProcess(processList, pCmdLine, pid);
    }
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
    process** processList = (process**)malloc(sizeof(process*));
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
        execute(head, isInDebugMode, processList);
    }
    if(processList != NULL)
    {
        if(*processList != NULL)
        {
            freeProcessList(*processList);
        }
        free(processList);
    }

    return 0;
}