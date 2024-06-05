#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <sys/types.h>
#include <sys/wait.h>
#include <sys/time.h>

#define MESSAGE_SIZE 10
#define NUM_MESSAGES 10000

void child_process(int read_pipe, int write_pipe) {
    char message[MESSAGE_SIZE];
    for (int i = 0; i < NUM_MESSAGES; ++i) {
        // Read message from parent
        read(read_pipe, message, MESSAGE_SIZE);
        
        // Write response to parent
        write(write_pipe, message, MESSAGE_SIZE);
    }
    exit(0);
}

int main() {
    int parent_to_child[2];
    int child_to_parent[2];
    
    // Create pipes
    if (pipe(parent_to_child) < 0 || pipe(child_to_parent) < 0) {
        perror("pipe");
        exit(EXIT_FAILURE);
    }

    // Fork child process
    pid_t pid = fork();
    if (pid == -1) {
        perror("fork");
        exit(EXIT_FAILURE);
    } else if (pid == 0) {
        // Child process
        close(parent_to_child[1]);
        close(child_to_parent[0]);
        child_process(parent_to_child[0], child_to_parent[1]);
    } else {
        // Parent process
        close(parent_to_child[0]);
        close(child_to_parent[1]);
        
        struct timeval start, end;
        gettimeofday(&start, NULL);
        
        char message[MESSAGE_SIZE];
        for (int i = 0; i < NUM_MESSAGES; ++i) {
            // Send message to child
            write(parent_to_child[1], "ping", MESSAGE_SIZE);
            
            // Receive response from child
            read(child_to_parent[0], message, MESSAGE_SIZE);
        }
        
        gettimeofday(&end, NULL);
        double time_taken = (end.tv_sec - start.tv_sec) * 1e6 + (end.tv_usec - start.tv_usec);
        printf("Total time for %d messages: %lf microseconds\n", NUM_MESSAGES, time_taken);
        printf("Average time per message: %lf microseconds\n", time_taken / NUM_MESSAGES);
        
        // Wait for child to finish
        wait(NULL);
    }

    return 0;
}
