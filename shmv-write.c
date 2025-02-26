#include <stdio.h>
#include <stdlib.h>
#include <sys/ipc.h>
#include <sys/shm.h>
#include <string.h>

#define SHM_KEY 1234
#define SHM_SIZE 1024

int main() {
    // Création d'un segment mémoire partagé
    int shmid = shmget(SHM_KEY, SHM_SIZE, IPC_CREAT | 0666);
    if (shmid == -1) {
        perror("shmget");
        exit(1);
    }

    // Attachement du segment
    char *shared_memory = (char *)shmat(shmid, NULL, 0);
    if (shared_memory == (char *)-1) {
        perror("shmat");
        exit(1);
    }

    // Écriture dans la mémoire partagée
    strcpy(shared_memory, "Hello from System V SHM!");
    printf("Message écrit en mémoire partagée : %s\n", shared_memory);

    shmdt(shared_memory); // Detach
}
