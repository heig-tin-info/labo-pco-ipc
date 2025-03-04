#include <stdio.h>
#include <stdlib.h>
#include <signal.h>
#include <unistd.h>

// Gestionnaire de signaux
void signal_handler(int signum) {
    switch(signum) {
        case SIGINT:
            printf("Signal SIGINT reçu (Ctrl+C)\n");
            break;
        case SIGUSR1:
            printf("Signal SIGUSR1 reçu\n");
            break;
        case SIGUSR2:
            printf("Signal SIGUSR2 reçu\n");
            break;
        default:
            break;
    }
}

int main() {
    struct sigaction sa;

    // Initialisation de la structure
    sa.sa_handler = signal_handler;
    sigemptyset(&sa.sa_mask);
    sa.sa_flags = 0;

    // Capture des signaux
    sigaction(SIGINT, &sa, NULL);
    sigaction(SIGUSR1, &sa, NULL);
    sigaction(SIGUSR2, &sa, NULL);

    // Ignorer SIGALRM
    signal(SIGALRM, SIG_IGN); //

    printf("Processus en attente de signaux... (PID: %d)\n", getpid());

    while (1) {
        pause();
        sigset_t pending;
        sigpending(&pending);
        if (sigismember(&pending, SIGUSR1)) {
            printf("SIGUSR1 est en attente !\n");
        }
        if (sigismember(&pending, SIGUSR2)) {
            printf("SIGUSR2 est en attente !\n");
        }
        if (sigismember(&pending, SIGINT)) {
            printf("SIGINT est en attente !\n");
        }
        if (sigismember(&pending, SIGALRM)) {
            printf("SIGALRM est en attente !\n");
        }

    }

}
