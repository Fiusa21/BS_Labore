#include <errno.h>
#include <stdio.h>
#include <signal.h>
#include <stdbool.h>
#include <unistd.h>

void signal_handler (int signum) {
    printf("Caught signal %d\n", signum);
    alarm(2);
}

int main(int argc, char* argv){
	if (SIG_ERR == signal (SIGUSR1, &signal_handler)){
		printf("Error: signal() returned errno:%d\n", errno);
	}
	
	if (SIG_ERR == signal (SIGALRM, &signal_handler)){
		printf("Error: signal() returned errno:%d\n", errno);
	}

	if (SIG_ERR == signal (SIGINT, &signal_handler)){
		printf("Error: signal() returned errno:%d\n", errno);
	}

	if (SIG_ERR == signal (SIGTSTP, &signal_handler)){
		printf("Error: signal() returned errno:%d\n", errno);
	}
	alarm(5);
	while(true){
		sleep(1);
		kill(getpid(), SIGUSR1);	
		// do nothing
	}	
}
