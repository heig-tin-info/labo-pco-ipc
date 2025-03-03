#include <stdio.h>
#include <stdlib.h>
#include <semaphore.h>
#include <fcntl.h>  // Pour O_CREAT

const char *SEM_NAME = "/sem";
int main() {
    // Visible dans /dev/shm (mémoire partagée)
    sem_t *sem = sem_open(SEM_NAME, O_CREAT, 0666, 0);
    if (sem == SEM_FAILED) {
        perror("sem_open");
        exit(1);
    }

    printf("Attente du sémaphore...\n");
    sem_wait(sem);
    printf("Sémaphore obtenu !\n");

    sem_post(sem);
    sem_close(sem);
    sem_unlink(SEM_NAME); // Supprimer le sémaphore
}
