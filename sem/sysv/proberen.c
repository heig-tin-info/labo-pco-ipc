#include <stdio.h> // printf
#include <stdlib.h> // exit
#include <sys/sem.h> // semget, semop, semctl

#include "key.h"

int main() {
    int semid = semget(KEY, 1, IPC_CREAT | 0666);
    if (semid == -1) {
        perror("semget");
        exit(1);
    }

    printf("Wait for semaphore...\n");
    semop(semid, &(struct sembuf){.sem_op=-1}, 1);
    printf("Semaphore acquired! (%d)\n", semctl(semid, 0, GETVAL));
}
