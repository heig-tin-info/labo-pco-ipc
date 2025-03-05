#include <stdio.h>
#include <stdlib.h>
#include <semaphore.h>
#include <fcntl.h>  // O_CREAT

#include "name.h"

int main() {
    sem_t *sem = sem_open(SEM_NAME, O_CREAT, 0666, 0);
    if (sem == SEM_FAILED) {
        perror("sem_open");
        exit(1);
    }
    puts("sem_wait...");
    sem_wait(sem);
    puts("acquired!");

    sem_close(sem);
}
