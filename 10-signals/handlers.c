#include <unistd.h>
#include <stdio.h>
#include <stdlib.h>
#include <signal.h>

void signal_handler(int signo)
{
	if (signo == SIGINT || signo == SIGTERM) {
		printf("Caught signal: %s\n", sys_siglist[signo]);
	} else {
		fprintf(stderr, "Unexpected signal: %d\n", signo);
		exit(EXIT_FAILURE);
	}
	exit(EXIT_SUCCESS);
}

void register_signal(int signo, void (*func) (int))
{
	if (signal(signo, func) == SIG_ERR) {
		fprintf(stderr, "Cannot register signal handler for: %d\n", signo);
		exit(EXIT_FAILURE);
	}
}

int main()
{
	register_signal(SIGINT, signal_handler);
	register_signal(SIGTERM, signal_handler);
	register_signal(SIGPROF, SIG_DFL);
	register_signal(SIGHUP, SIG_IGN);

	while (1) {
		pause();
	}
}
