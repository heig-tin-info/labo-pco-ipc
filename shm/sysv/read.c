#include <stdio.h>
#include <stdlib.h>
#include <sys/ipc.h>
#include <sys/shm.h>

#define SHM_KEY 1234
#define SHM_SIZE 1024

int main() {
    // Récupération du segment mémoire
    int shmid = shmget(SHM_KEY, SHM_SIZE, 0666);
    if (shmid == -1) {
        perror("shmget");
        exit(EXIT_FAILURE);
    }

    // Attachement au segment
    char *shared_memory = (char *)shmat(shmid, NULL, 0);
    if (shared_memory == (char *)-1) {
        perror("shmat");
        exit(EXIT_FAILURE);
    }

    // Lecture depuis la mémoire partagée
    printf("Message lu en mémoire partagée : %s\n", shared_memory);

    // Détachement du segment
    shmdt(shared_memory);

    // Suppression du segment mémoire (facultatif, fait souvent par le dernier processus)
    shmctl(shmid, IPC_RMID, NULL);
}
