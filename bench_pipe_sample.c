#define _GNU_SOURCE
#include <unistd.h>
#include <stdio.h>
#include <stdlib.h>
#include <stdint.h>
#include <string.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <sys/time.h>
#include <fcntl.h>
#include <limits.h>
#include <errno.h>
#include <assert.h>

#include "bench_utils.h"



int main(int argc, char * argv[]) {
    const int sizes[] = {64, 256, 1024, 4096, 16384, 65536, 262144, 1048576, 4194304, 16777216};
    const int sizes_num = sizeof(sizes)/sizeof(sizes[0]);
#define MAX_SIZE sizes[sizes_num-1]
    int pipe_child_to_parent[2];
    int pipe_parent_to_child[2];
    char * buffer;
    pid_t pid;
    pid_t pid_child;
    int ret;

    // Open pipes to communicate the pointer to buffer
    ret = pipe(pipe_child_to_parent);
    if (-1 == ret)
        ERROR("pipe child_to_parent", errno);
    ret = pipe(pipe_parent_to_child);
    if (-1 == ret)
        ERROR("pipe parent_to_child", errno);

    pid = getpid();
    ret = pid_child = fork();
    if (-1 == ret)
        ERROR("fork", errno);

    if (0 == ret) {
        /* CHILD Process */
        int num_read;
        int to_read;
        pid = getpid();
        DEBUG(printf("PID:%d (CHILD) starts\n", (int) pid));

        // Close the reading end in child_to_parent and writing end in parent_to_child
        ret = close(pipe_child_to_parent[0]);
        if (ret == -1) ERROR("close", errno);
        ret = close(pipe_parent_to_child[1]);
        if (ret == -1) ERROR("close", errno);

        buffer = malloc(MAX_SIZE);
        if (NULL == buffer)
            ERROR("malloc", ENOMEM);
        memset(buffer, 0, MAX_SIZE);

        for (int i = 0; i < sizes_num; i++) {
            int current_size = sizes[i];
            to_read = current_size;
            for (num_read = 0; num_read < current_size; num_read += ret, to_read -= ret) {
                ret = read(pipe_parent_to_child[0], buffer + num_read, to_read);
                if (ret == -1) ERROR("read", errno);
            }
        }

        DEBUG(printf("PID:%d (CHILD) exits\n", (int) pid));
        free(buffer);
        exit(EXIT_SUCCESS);
    }

    int num_written;
    int to_write;
    int * ticks;
    int i;

    DEBUG(printf("PID:%d (PARENT) starts child_pid:%d\n", (int) pid, (int)pid_child));

    // Close the reading end in parent_to_child and writing end in child_to_parent
    ret = close(pipe_parent_to_child[0]);
    if (ret == -1) ERROR("close", errno);
    ret = close(pipe_child_to_parent[1]);
    if (ret == -1) ERROR("close", errno);

    buffer = malloc(MAX_SIZE);
    if (NULL == buffer)
        ERROR("malloc", ENOMEM);
    memset(buffer, 'a', MAX_SIZE);  // Fill the buffer with 'a's

    ticks = malloc(MEASUREMENTS * sizeof(int));
    if (NULL == ticks)
        ERROR("malloc", ENOMEM);
    memset(ticks, 0, MEASUREMENTS * sizeof(int));

    for (i = 0; i < sizes_num; i++) {
        int current_size = sizes[i];
        int j;
        int min_ticks;
        int max_ticks;
        long long ticks_all;
        struct timeval tv_start;
        struct timeval tv_stop;
        double time_delta_sec;

        assert(current_size <= MAX_SIZE);

        gettimeofday(&tv_start, NULL);
        for (j = 0; j < MEASUREMENTS; j++) {
            unsigned long long start;
            unsigned long long stop;
            start = getrdtsc();
            to_write = current_size;
            for (num_written = 0; num_written < current_size; num_written += ret, to_write -= ret) {
                ret = write(pipe_parent_to_child[1], buffer + num_written, to_write);
                if (ret == -1) ERROR("write", errno);
            }
            stop = getrdtsc();
            ticks[j] = stop - start;
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

        printf("PID:%d, Size:%d, Avg. Ticks: %f (for %d measurements), Time: %.3fs, Speed: (%.2f MB/s)\n",
               pid, current_size,
               (double) ticks_all / (MEASUREMENTS - 2.0), MEASUREMENTS, time_delta_sec,
               ((double) current_size * MEASUREMENTS) / (1024.0 * 1024.0 * time_delta_sec));
    }

    // Tell Child to exit:
    write(pipe_parent_to_child[1], &pid, 1);

    free(buffer);
    free(ticks);

    return EXIT_SUCCESS;
}
