/**************************************************
/benchmark based on                            */


#include <unistd.h>
#include <stdio.h>
#include <stdlib.h>
#include <stdint.h>
#include <sys/types.h>
#include <err.h>
#include <errno.h>
#include <string.h>
#include <limits.h>
#include <sys/time.h>

#include "bench_utils.h"

#define SLEEP_TIME 1

// #define USE_MEMSET

int main(int argc, char * argv[])
{
    const int sizes[] = {64, 128, 256, 512, 1024, 2048, 4096, 8192, 16384, 32768, 65536, 131072, 262144, 524288, 1048576, 2097152, 4194304, 8388608, 16777216};
    const int sizes_num = sizeof(sizes)/sizeof(sizes[0]);
    pid_t pid = getpid();
    pid_t pid_child;
    int pipe_fd[2]; // File descriptors for the pipe
    int i;

    // Create a pipe
    if (pipe(pipe_fd) == -1) {
        perror("pipe");
        exit(EXIT_FAILURE);
    }

    int ret = pid_child = fork(); // Fork a child process

    if (-1 == ret)
        ERROR("fork", errno);

    if (0 == ret)  {
        /* CHILD PROCESS */
        char * buffer;
        buffer = malloc(sizes[sizes_num - 1]);
        if (NULL == buffer)
            ERROR("malloc", ENOMEM);
        memset(buffer, 0, sizes[sizes_num - 1]);
        pid_child = getpid();
	

	while (read(pipe_fd[0], buffer, sizes[sizes_num - 1]) > 0){
		//Do nothing, just keep reading from pipe
	}
	
	printf("PID %d (CHILD): COPY DONE\n", pid_child);
	return (EXIT_SUCCESS);
     }


        

    /* PARENT PROCESS: measure the writing into the pipe */
    for (i = 0; i < sizes_num; i++) {
        int current_size = sizes[i];
        int j;
        struct timeval tv_start;
        struct timeval tv_stop;
        double time_delta_sec;

        sleep(SLEEP_TIME);

        gettimeofday(&tv_start, NULL);
	//maybe add ticks here aswell (minimum and max duration per message in microseconds)

#ifdef USE_MEMSET
            {
                char buffer[current_size];
                memset(buffer, 'a', current_size);
                // Write data to the pipe
                if (write(pipe_fd[1], buffer, current_size) == -1) {
                    perror("write");
                    exit(EXIT_FAILURE);
                }
            }
#else
            {
                char c = 'a';
                int k;
                for (k = 0; k < current_size; k++) {
                    // Write data to the pipe
                    if (write(pipe_fd[1], &c, 1) != 1) {
                        perror("write");
                        exit(EXIT_FAILURE);
                    }
                }
            }
#endif
           
        }
        gettimeofday(&tv_stop, NULL);

        // Calculating the time elapsed
        time_delta_sec = ((tv_stop.tv_sec - tv_start.tv_sec) + ((tv_stop.tv_usec - tv_start.tv_usec)/(1000.0*1000.0)));

        // Output results
        printf ("PID:%d, Time: %lf seconds (for %d measurements) for %d Bytes (%.2f MB/s)\n",
                pid, time_delta_sec, MEASUREMENTS, current_size,
                ((double) current_size * MEASUREMENTS)  / (1024.0*1024.0*time_delta_sec) );
    }

    close(pipe_fd[0]); // Close the read end of the pipe in the parent process
    close(pipe_fd[1]); // Close the write end of the pipe in the parent process

    return (EXIT_SUCCESS);
}
