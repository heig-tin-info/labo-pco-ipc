
#include <stdio.h>
#include <stdlib.h>
#include <sys/ipc.h>
#include <sys/sem.h>
#include <unistd.h>

int main() {
    int semid = semget(1234, 1, IPC_CREAT | 0666);
    if (semid == -1) {
        perror("semget");
        exit(1);
    }
    semop(semid, &(struct sembuf){.sem_op=1}, 1);
    printf("Sémaphore déclenché (%d) !\n", semctl(semid, 0, GETVAL));
}
