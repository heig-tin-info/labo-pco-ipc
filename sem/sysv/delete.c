
#include <stdio.h>
#include <stdlib.h>
#include <sys/sem.h>

#include "key.h"

int main() {
    int semid = semget(KEY, 1, IPC_CREAT | 0666);
    if (semid == -1) {
        perror("semget");
        exit(1);
    }
    semctl(semid, 0, IPC_RMID);
    printf("Semaphore removed!\n");
}
