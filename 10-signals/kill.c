#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <signal.h>

int main(int argc, char *argv[])
{
	if (argc != 3) {
		fprintf(stderr, "Usage: %s <signo> <pid>\n", argv[0]);
		return 1;
	}

	int pid = atoi(argv[1]);
	int signo = atoi(argv[2]);

	if (kill(pid, signo)) {
		perror("kill");
		return 1;
	}
}

