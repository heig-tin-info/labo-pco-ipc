#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <signal.h>
#include <sys/wait.h>

void signal_handler(int sig) {
    printf("%d: signal %s (%d) received!\n", getpid(), strsignal(sig), sig);
}

int main() {
    pid_t pid = fork();
    if (pid < 0) {
        perror("fork");
        exit(EXIT_FAILURE);
    }

    if (pid == 0) { // Child
        signal(SIGUSR1, signal_handler);
        printf("%d: Waiting for a signal...\n", getpid());
        pause(); // Wait for a signal
    } else { // Parent
        sleep(1); // Give some time
        printf("%d: kill(SIGUSR1) -> %d\n", getpid(), pid);
        kill(pid, SIGUSR1);
        wait(NULL); // Wait for the child to terminate
    }
}
