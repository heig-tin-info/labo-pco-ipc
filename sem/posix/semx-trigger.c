#include <stdio.h>
#include <stdlib.h>
#include <semaphore.h>

int main() {
    sem_t *sem = sem_open("/sem", 0);
    if (sem == SEM_FAILED) {
        perror("sem_open");
        exit(1);
    }

    printf("Déclencheur : Libération du sémaphore...\n");
    sem_post(sem);  // Libère le sémaphore pour l'autre programme
    printf("Déclencheur : Sémaphore libéré !\n");

    sem_close(sem);
}
