#include <stdio.h>
#include <stdlib.h>
#include <semaphore.h>
#include <fcntl.h>  // Pour O_CREAT
#include <unistd.h>

const char *SEM_NAME = "/sem";

int main() {
    // Ouvrir le sémaphore existant
    sem_t *sem = sem_open(SEM_NAME, 0);
    if (sem == SEM_FAILED) {
        perror("sem_open");
        exit(1);
    }

    printf("Déclencheur : Libération du sémaphore...\n");
    sem_post(sem);  // Libère le sémaphore pour l'autre programme
    printf("Déclencheur : Sémaphore libéré !\n");

    sem_close(sem);
}
