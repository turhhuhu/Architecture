#include <stdio.h>
#include <sys/wait.h>
#include <stdlib.h>
#include <unistd.h>
#include <string.h>

int main(int argc, char *argv[])
{
    int pipefd[2];
    int cpid;
    char buf;

    if (pipe(pipefd) == -1)
    {
        perror("pipe");
        exit(EXIT_FAILURE);
    }

    cpid = fork();
    if (cpid == -1)
    {
        perror("fork");
        exit(EXIT_FAILURE);
    }

    if (cpid == 0)
    {                     /* Child reads from pipe */
        close(pipefd[0]); /* Close unused write end */
        write(pipefd[1], "hello", strlen("hello"));
        close(pipefd[1]);
        _exit(EXIT_SUCCESS);
    }
    else
    {                     /* Parent writes argv[1] to pipe */
        close(pipefd[1]); /* Close unused read end */
        
         while (read(pipefd[0], &buf, 1) > 0)
            write(STDOUT_FILENO, &buf, 1);
        write(STDOUT_FILENO, "\n", 1);
        close(pipefd[0]); /* Reader will see EOF */
        wait(NULL);       /* Wait for child */
        exit(EXIT_SUCCESS);
    }
}