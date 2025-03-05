#include <stdio.h>
#include <stdlib.h>
#include <signal.h>
#include <unistd.h>

void handler(int signum) {
    printf("\nGoodbye!\n");
    exit(0);
}

int main() {
    struct sigaction sa = { .sa_handler = handler };

    // Intercept SIGINT
    sigemptyset(&sa.sa_mask);
    if (sigaction(SIGINT, &sa, NULL) == -1) {
        perror("sigaction");
        exit(EXIT_FAILURE);
    }

    // Ignore SIGUSR1
    signal(SIGUSR1, SIG_IGN);

    // Block SIGUSR2
    sigset_t mask;
    sigemptyset(&mask);
    sigaddset(&mask, SIGUSR2);
    sigprocmask(SIG_BLOCK, &mask, NULL);

    // Simulate work for 10 seconds
    for (int i = 0; i < 10; i++) {
        printf("Working... %d\n", i);
        sleep(1);
    }

    sigprocmask(SIG_UNBLOCK, &mask, NULL);
}
