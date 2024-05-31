/*
 * Small timing test of Linux' process_vm_readv / process_vm_writev calls
 * This new function calls were added in Linux 3.2 (released January 2012)
 * 
 * Author: Rainer Keller, HFT Stuttgart
 */

#define _GNU_SOURCE

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <errno.h>
#include <sys/uio.h>


#define SIZE 4096
#define PROCS 2

#define ERROR(s,e) do {                                     \
        fprintf (stderr, "ERROR: %s (strerror of %d:%s)\n", \
                 (s), (e), strerror((e)));                  \
        exit (e);                                           \
    } while(0)

#define SLEEP_MS   (1000*1000)
#define MEMSET
#define MEMSET_CHAR '\0'

/* Local variable definitions */
static char buffer[PROCS * SIZE];

/* Local function definitions */
static unsigned long long int getrdtsc(void)
{
        unsigned int hi, lo;
        __asm__ __volatile__("rdtsc" : "=a"(lo), "=d"(hi));
        return (((unsigned long long int)hi << 32) |
                ((unsigned long long int)lo));
}





int main (int argc, char * argv[])
{
    pid_t pid;
    pid_t pid_child;
    unsigned long long start;
    unsigned long long stop;
    int ret;


#if defined(MEMSET)
    memset (buffer, MEMSET_CHAR, SIZE);
#endif

    pid = getpid();

    ret = pid_child = fork ();
    if (ret == -1)
        ERROR ("fork", errno);

    if (0 == ret) {
        /* CHILD Process */
        pid = getpid();
        printf ("PID:%d (CHILD) buffer:%s\n",
                (int)pid, buffer);
        sleep (2);

        printf ("PID:%d (CHILD) NOW buffer:%s\n",
                pid, buffer);
        
    } else if (0 < ret) {
        struct iovec local[2];
        struct iovec remote[1];
        char buf1[10] = "Hello56789";
        char buf2[10] = "World56789";
        ssize_t nwrite;
        int ticks;


        local[0].iov_base = buf1;
        local[0].iov_len = 10;
        local[1].iov_base = buf2;
        local[1].iov_len = 10;
        remote[0].iov_base = (void *) buffer;
        remote[0].iov_len = 20;

        sleep (1);

        start = getrdtsc();
        nwrite = process_vm_writev(pid_child, local, 2, remote, 1, 0);
        stop = getrdtsc();

        ticks = stop - start;
        printf ("PID:%d time: %d Ticks for %ld Bytes\n",
                pid, ticks, nwrite);
        if (nwrite != 20) {
            char error[32];
            snprintf (error, sizeof(error), "nwrite:%ld != 20\n", nwrite);
            ERROR(error, EINVAL);
        }
        sleep (1);
    }

    return 0;
}
