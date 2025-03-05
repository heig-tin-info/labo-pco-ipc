#include <stdio.h>
#include <stdlib.h>
#include <signal.h>
#include <unistd.h>

void handler(int signum) {
    printf("\nGoodbye!\n");
    exit(0);
}

int main() {
    // Quel handler appeler lors de la réception du signal
    struct sigaction sa = { .sa_handler = handler };

    // On ne bloque aucun signal durant l'execution du handler
    sigemptyset(&sa.sa_mask);

    // Association du handler au signal SIGINT
    if (sigaction(SIGINT, &sa, NULL) == -1) {
        perror("sigaction");
        exit(EXIT_FAILURE);
    }

    // Ignore SIGUSR1
    signal(SIGUSR1, SIG_IGN);

    // Blocage temporaire
    printf("Blocking SIGUSR1 and SIGUSR2... hit enter to unblock\n");
    sigset_t mask;
    sigemptyset(&mask);
    sigaddset(&mask, SIGUSR1);
    sigaddset(&mask, SIGUSR2);
    sigprocmask(SIG_BLOCK, &mask, NULL);
    getchar();
    sigprocmask(SIG_UNBLOCK, &mask, NULL);

    while (1) pause();
}
