/*
 * Small timing test of Unix Pipes 
 * 
 * Author: Rainer Keller, HFT Stuttgart
 */


#include <unistd.h>
#include <stdio.h>
#include <stdlib.h>
#include <errno.h>
#include <string.h>

#define SIZE 4096

#define ERROR(s,e) do {                                     \
        fprintf (stderr, "ERROR: %s (strerror of %d:%s)\n", \
                 (s), (e), strerror((e)));                  \
        exit (e);                                           \
    } while(0)


char buffer[SIZE];
const char string[] = "Hello56789World56789";

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
    int fd[2];
    int ret;

    ret = pipe (fd);
    if (0 != ret)
        ERROR ("pipe()", errno);

    pid = getpid();

    ret = pid_child = fork ();
    if (ret == -1)
        ERROR ("fork", errno);

    if (0 == ret) {
        int nread = 1;
        /* CHILD Process: READS from pipe */
        pid = getpid();
        printf ("PID:%d (CHILD)\n",
                (int)pid);
        close (fd[1]);
        while (0 != nread) {
            nread = read (fd[0], buffer, SIZE);
            printf ("PID:%d (CHILD) just read:%d Bytes:%s\n",
                    pid, nread, buffer);
        }
    } else if (0 < ret) {
        unsigned long long start;
        unsigned long long stop;
        int ticks;
        int nwrite;

        /* PARENT Process: WRITES to pipe */
        close (fd[0]);
        strcpy (buffer, string);
        start = getrdtsc();
        nwrite = write(fd[1], buffer, strlen (string));
        stop = getrdtsc();

        ticks = stop - start;
        printf ("PID:%d time: %d Ticks for %d Bytes\n",
                pid, ticks, nwrite);
    }

    return 0;
}
