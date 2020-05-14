#include <stdio.h>
#include <sys/wait.h>
#include <stdlib.h>
#include <unistd.h>
#include <string.h>

void debug(char isInDebugMode, char *msg)
{
    if(isInDebugMode)
    {
        fprintf(stderr,"%s", msg);
        fprintf(stderr, "\n");
    }
}
int main(int argc, char **argv)
{
    char output[256];
    int pipefd[2];
    int cpid1;
    int cpid2;
    char isInDebugMode = 0;
    int i;
    for (i = 0; i < argc; i++)
    {
        if(strcmp("-d", argv[i]) == 0)
        {
            isInDebugMode = 1;
            break;
        }
    }

    if (pipe(pipefd) == -1)
    {
        perror("pipe");
        exit(EXIT_FAILURE);
    }
    debug(isInDebugMode, "(parent_process>forking…)");
    cpid1 = fork();
    sprintf(output, "(parent_process>created process with id: %d)", cpid1);
    debug(isInDebugMode, output);
    if (cpid1 == -1)
    {
        perror("fork");
        exit(EXIT_FAILURE);
    }
    if (cpid1 == 0)
    {
        debug(isInDebugMode, "(child1>redirecting stdout to the write end of the pipe…)");                     
        close(STDOUT_FILENO); 
        dup(pipefd[1]);
        close(pipefd[1]);
        char * arguments[3];
        arguments[0] = "ls";
        arguments[1] = "-l";
        arguments[2] = 0;
        sprintf(output,"(child1>going to execute cmd: %s %s)", arguments[0], arguments[1]);
        debug(isInDebugMode, output);
        if (execvp(arguments[0], arguments) < 0)
        {
            perror("error in execv");
            _exit(1);
        }
        _exit(EXIT_SUCCESS);
    }
    else{
        debug(isInDebugMode, "(parent_process>closing the write end of the pipe…)");
        close(pipefd[1]);
    }
    debug(isInDebugMode, "(parent_process>forking…)");
    cpid2 = fork();
    sprintf(output, "(parent_process>created process with id: %d)", cpid2);
    debug(isInDebugMode, output);
    if (cpid2 == -1)
    {
        perror("fork");
        exit(EXIT_FAILURE);
    }
    if (cpid2 == 0)
    {                     
        debug(isInDebugMode, "(child2>redirecting stdin to the read end of the pipe…)");  
        close(STDIN_FILENO); 
        dup(pipefd[0]);
        close(pipefd[0]);
        char * arguments[4];
        arguments[0] = "tail";
        arguments[1] = "-n";
        arguments[2] = "2";
        arguments[3] = 0;
        sprintf(output,"(child2>going to execute cmd: %s %s %s)", arguments[0], arguments[1], arguments[2]);
        debug(isInDebugMode, output);
        if (execvp(arguments[0], arguments) < 0)
        {
            perror("error in execv");
            _exit(1);
        }
        _exit(EXIT_SUCCESS);
    }
    else{
        debug(isInDebugMode, "(parent_process>closing the read end of the pipe…)");
        close(pipefd[0]);
    }
    debug(isInDebugMode, "(parent_process>waiting for child processes to terminate…)");
    waitpid(cpid1, NULL, 0);
    waitpid(cpid2, NULL, 0);
    debug(isInDebugMode, "(parent_process>exiting…)");
    return 0;
}