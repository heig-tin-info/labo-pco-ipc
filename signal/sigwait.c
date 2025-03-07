#define _XOPEN_SOURCE 700

#include <stdio.h>
#include <signal.h>
#include <sys/wait.h>

int main() {
    sigset_t mask;
    sigemptyset(&mask);
    sigaddset(&mask, SIGUSR1);
    sigaddset(&mask, SIGUSR2);

    sigprocmask(SIG_BLOCK, &mask, NULL); // Block SIGUSR1

    int sig;  // Store the signal number received
    sigdelset(&mask, SIGUSR2);
    sigwait(&mask, &sig);

    printf("Signal %d received!\n", sig);
}
