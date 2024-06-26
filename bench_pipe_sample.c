/*
 * Small benchmark of UNIX-Pipes for IPC.
 */

#include <unistd.h>
#include <stdio.h>
#include <stdlib.h>
#include <stdint.h>
#include <sys/types.h>
#include <sys/mman.h>
#include <err.h>
#include <errno.h>
#include <fcntl.h>
#include <string.h>
#include <limits.h>
#include <sys/time.h>

#include "bench_utils.h"

#define SLEEP_TIME 1

// #define USE_MEMSET

int main(int argc, char * argv[])
{
    const int sizes[] = {64, 256, 1024, 4096, 16384, 65536, 262144, 1048576, 4194304, 16777216};
    const int sizes_num = sizeof(sizes) / sizeof(sizes[0]);
#define MAX_SIZE sizes[sizes_num-1]
    pid_t pid = getpid();
    pid_t pid_child;
    int i;

    int pipefd[2];
    if (pipe(pipefd) == -1) {
        ERROR("pipe", errno);
    }

    int ret = pid_child = fork();

    if (-1 == ret) {
        ERROR("fork", errno);
    }

    if (0 == ret)  {
        /* CHILD */
        char *buffer;
        buffer = malloc(MAX_SIZE);
        if (NULL == buffer) {
            ERROR("malloc", ENOMEM);
        }
        memset(buffer, 0, MAX_SIZE);
        pid_child = getpid();

        close(pipefd[0]);  // Close the read end of the pipe
        write(pipefd[1], buffer, MAX_SIZE);
        close(pipefd[1]);  // Close the write end of the pipe after writing
        sleep(SLEEP_TIME);
        printf("PID %d (CHILD): COPY DONE\n", pid_child);
        free(buffer);
        return (EXIT_SUCCESS);
    }
    
    int *ticks;
    ticks = malloc(MEASUREMENTS * sizeof(int));
    if (NULL == ticks) {
        ERROR("malloc", ENOMEM);
    }
    memset(ticks, 0, MEASUREMENTS * sizeof(int));    

    /* PARENT: measure the writing into the buffer */
    for (i = 0; i < sizes_num; i++) {
        int current_size = sizes[i];
        int j;
        int min_ticks;
        int max_ticks;
        long long ticks_all;
        struct timeval tv_start;
        struct timeval tv_stop;
        double time_delta_sec;

        sleep(SLEEP_TIME);
        
        gettimeofday(&tv_start, NULL);
        for (j = 0; j < MEASUREMENTS; j++) {
            unsigned long long start;
            unsigned long long stop;

            char *anon = malloc(current_size);
            if (anon == NULL) {
                ERROR("malloc", ENOMEM);
            }

            start = getrdtsc();
#ifdef USE_MEMSET
            memset(anon, 'a', current_size);
#else
            {
                int k;
                for (k = 0; k < current_size; k++) {
                    anon[k] = 'a';
                }
            }
#endif
            stop = getrdtsc();
            ticks[j] = stop - start;

            close(pipefd[1]);  // Close the write end of the pipe
            read(pipefd[0], anon, current_size);
            close(pipefd[0]);  // Close the read end of the pipe after reading

            free(anon);
        }
        gettimeofday(&tv_stop, NULL);
            
        min_ticks = INT_MAX;
        max_ticks = INT_MIN;
        ticks_all = 0;
        for (j = 0; j < MEASUREMENTS; j++) {
            if (min_ticks > ticks[j]) min_ticks = ticks[j];
            if (max_ticks < ticks[j]) max_ticks = ticks[j];
            ticks_all += ticks[j];
        }
        ticks_all -= min_ticks;
        ticks_all -= max_ticks;

        time_delta_sec = ((tv_stop.tv_sec - tv_start.tv_sec) + ((tv_stop.tv_usec - tv_start.tv_usec) / (1000.0 * 1000.0)));
	

        printf("duration: %lf PID: %d time: min:%d max:%d Ticks Avg without min/max:%f Ticks (for %d measurements) for %d Bytes (%.2f MB/s)\n;",
               time_delta_sec, pid, min_ticks, max_ticks,
               (double) ticks_all / (MEASUREMENTS - 2.0), MEASUREMENTS, current_size,
               ((double) current_size * MEASUREMENTS) / (1024.0 * 1024.0 * time_delta_sec));
    }

    free(ticks);
    return (EXIT_SUCCESS);
}
