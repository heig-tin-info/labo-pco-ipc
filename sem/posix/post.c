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
    sem_post(sem);
    puts("sem_post");
    sem_close(sem);
}
