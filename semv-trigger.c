
#include <stdio.h>
#include <stdlib.h>
#include <sys/ipc.h>
#include <sys/sem.h>
#include <unistd.h>

#define SEM_KEY 1234

int main() {
    int semid = semget(SEM_KEY, 1, IPC_CREAT | 0666);
    if (semid == -1) {
        perror("semget");
        exit(1);
    }

    struct sembuf op = {0, 1, 0};  // Incrémenter le sémaphore
    printf("Déclenchement du sémaphore...\n");
    semop(semid, &op, 1);
    printf("Sémaphore déclenché (%d) !\n", semctl(semid, 0, GETVAL));
}
