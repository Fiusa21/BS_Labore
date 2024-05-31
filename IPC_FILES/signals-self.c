#include <errno.h>
#include <stdio.h>
#include <signal.h>
#include <stdbool.h>
#include <unistd.h>

// The Signal Handler, called, once the signal is received
void signal_handler (int signum) {
    printf("Caught signal %d\n", signum);
}

int main(int argc, char* argv){
	// Register signal handler for SIGUSR1 (Signal User1)
	if (SIG_ERR == signal (SIGUSR1, &signal_handler)){
		printf("Error: signal() returned errno:%d\n", errno);
	}
	while(true){
		sleep(1);
		kill(getpid(), SIGUSR1);	
		// do nothing
	}	
}
// Now, you may send signal from the command-line
// $> kill -10 PID
