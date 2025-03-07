#include <stdio.h>
#include <stdlib.h>
#include <sys/ipc.h>
#include <sys/shm.h>
#include <unistd.h>
#include <string.h>

#include "key.h"

#define SIZE 64       // Taille du segment mémoire

int main(int argc, char *argv[]) {
    if (argc != 2) {
        fprintf(stderr, "Usage: %s <char>\n", argv[0]);
        exit(EXIT_FAILURE);
    }

    char c = argv[1][0];  // Caractère à écrire (A ou B)

    printf("Processus %c va écrire dans la mémoire partagée.\n", c);
    int shmid = shmget(KEY, SIZE, IPC_CREAT | 0666);
    if (shmid == -1) {
        perror("shmget");
        exit(EXIT_FAILURE);
    }

    printf("Processus %c attache le segment de mémoire partagée.\n", c);
    char *shmaddr = (char *)shmat(shmid, NULL, 0);
    if (shmaddr == (char *)(-1)) {
        perror("shmat");
        exit(EXIT_FAILURE);
    }

    // Écriture concurrente
    for (int i = 0; i < SIZE - 1; i++) {
        printf("Processus %c écrit à l'index %i.\n", c, i);
        shmaddr[i] = c;
        usleep(rand() % 10000);
    }
    shmaddr[SIZE - 1] = '\0';

    printf("Processus %c a écrit dans la mémoire partagée.\n", c);

    shmdt(shmaddr);
}
