#include <stdio.h>
#include <unistd.h>
#include <signal.h>
#define _GNU_SOURCE
#include <string.h>

void signal_handler(int sig)
{
	printf("received signal: ");
	char * sign = (char *)strsignal(sig);
	printf("%s\n", sign);
	if(sig == SIGCONT)
	{
		signal(SIGTSTP, signal_handler);
	}
	else if(sig == SIGTSTP)
	{
		signal(SIGCONT, signal_handler);
	}
	signal(sig, SIG_DFL);
	raise(sig);
}
int main(int argc, char **argv)
{
	printf("Starting the program\n");
	signal(SIGINT, signal_handler);
	signal(SIGTSTP, signal_handler);
	signal(SIGCONT, signal_handler);
	while (1)
	{
		sleep(2);
	}

	return 0;
}