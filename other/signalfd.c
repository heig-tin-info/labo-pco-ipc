#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <signal.h>
#include <sys/signalfd.h>

int main() {
    sigset_t mask;
    sigemptyset(&mask);
    sigaddset(&mask, SIGINT);

    // Bloquer SIGINT pour éviter qu'il soit capté par défaut
    sigprocmask(SIG_BLOCK, &mask, NULL);

    // Création du signalfd
    int sfd = signalfd(-1, &mask, 0);
    if (sfd == -1) {
        perror("signalfd");
        exit(1);
    }

    printf("Attente de SIGINT...\n");

    // Lire les signaux dans la file
    struct signalfd_siginfo si;
    read(sfd, &si, sizeof(si));

    printf("SIGINT reçu !\n");

    close(sfd);
    return 0;
}
