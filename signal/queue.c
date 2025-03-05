#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <signal.h>

// Gestionnaire de signal
void signal_handler(int sig, siginfo_t *info, void *context) {
    printf("Enfant : Signal %d reçu avec valeur %d\n", sig, info->si_value.sival_int);
}

int main() {
    pid_t pid = fork();

    if (pid < 0) {
        perror("fork");
        exit(EXIT_FAILURE);
    }

    if (pid == 0) { // Processus enfant
        struct sigaction sa;
        sa.sa_flags = SA_SIGINFO;
        sa.sa_sigaction = signal_handler;
        sigaction(SIGUSR1, &sa, NULL);

        printf("Enfant : En attente de SIGUSR1...\n");
        pause();
        exit(0);
    } else { // Processus parent
        sleep(1); // Attendre que l'enfant soit prêt
        union sigval value;
        value.sival_int = 42; // Valeur transmise

        printf("Parent : Envoi du signal SIGUSR1 avec valeur %d à %d\n", value.sival_int, pid);
        sigqueue(pid, SIGUSR1, value);
        wait(NULL);
    }

    return 0;
}
