#include <errno.h>
#include <stdio.h>
#include <signal.h>
#include <stdbool.h>
#include <unistd.h>
// Der Signal Handler
void signal_handler (int signum) {
    printf("Caught signal %d\n", signum);
}


int main(int argc, char* argv){

	// in main() registrieren für das Signal
	// SIGUSR1 (Signal User1)
	if (SIG_ERR == signal (SIGUSR1, &signal_handler)){
		printf("Error: signal %d\n", errno);
	}
	while(true){
		sleep(2);
		// do nothing
	}	
}
// Nun man kann sich das Signal schicken:
// Auf der Kommandozeile:
// $> kill -10 PID
