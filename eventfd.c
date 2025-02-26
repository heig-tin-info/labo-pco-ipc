#include <stdio.h>
#include <stdlib.h>
#include <sys/eventfd.h>
#include <unistd.h>

int main() {
    int efd = eventfd(0, 0);  // Crée un eventfd initialisé à 0
    if (efd == -1) {
        perror("eventfd");
        exit(1);
    }

    pid_t pid = fork();
    if (pid == 0) {
        // Processus fils : signale un événement
        printf("Fils : Envoi d'un événement...\n");
        uint64_t value = 1;
        write(efd, &value, sizeof(value));
        exit(0);
    } else {
        // Processus père : attend l'événement
        uint64_t value;
        read(efd, &value, sizeof(value));
        printf("Père : Événement reçu !\n");
    }

    close(efd);
}
