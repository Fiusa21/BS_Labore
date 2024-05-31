#include <sys/types.h>
#include <sys/mman.h>
#include <err.h>
#include <errno.h>
#include <fcntl.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>


#define ERROR(s,e) do {                                     \
        fprintf (stderr, "ERROR: %s (strerror of %d:%s)\n", \
                 (s), (e), strerror((e)));                  \
        exit (e);                                           \
    } while(0)

 
int main(int argc, char * argv[])
{
        const char str1[] = "string 1";
        const char str2[] = "string 2";
        int parpid = getpid(), childpid;
        int fd = -1;
        char *anon;
 
        anon = mmap(NULL, 4096, PROT_READ|PROT_WRITE, MAP_ANON|MAP_SHARED, -1, 0);
 
        strcpy(anon, str1);
        if (anon == MAP_FAILED)
                ERROR("mmap anon", errno);

        switch ((childpid = fork())) {
        case -1:
                ERROR("fork", errno);
                /* NOTREACHED */
 
        case 0:
                childpid = getpid();
                printf("PID %d (CHILD):\tanonymous %s\n", childpid, anon);
                sleep(3);
 
                printf("PID %d (CHILD):\tanonymous %s\n", childpid, anon);
                close(fd);
                return (EXIT_SUCCESS);
        }
 
        sleep(2);
        strcpy(anon, str2);
 
        printf("PID %d:\tanonymous %s\n", parpid, anon);
        return (EXIT_SUCCESS);
}
