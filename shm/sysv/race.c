#include <stdio.h>
#include <stdlib.h>
#include <sys/ipc.h>
#include <sys/shm.h>
#include <unistd.h>
#include <sys/wait.h>

#define SHM_KEY 1234
#define SIZE 64

int main() {
    int shmid = shmget(SHM_KEY, SIZE, IPC_CREAT | 0666);
    if (shmid == -1) {
        perror("shmget");
        exit(EXIT_FAILURE);
    }

    pid_t pid1 = fork();
    if (pid1 == 0) {
        execl("./worker", "./worker1", "A", NULL);
        perror("execl");
        exit(EXIT_FAILURE);
    }

    pid_t pid2 = fork();
    if (pid2 == 0) {
        execl("./worker", "./worker2", "B", NULL);
        perror("execl");
        exit(EXIT_FAILURE);
    }

    wait(NULL);
    wait(NULL);

    // Attachement pour lecture
    char *shmaddr = (char *)shmat(shmid, NULL, 0);
    if (shmaddr == (char *)(-1)) {
        perror("shmat");
        exit(EXIT_FAILURE);
    }

    printf("Contenu final : %s\n", shmaddr);

    // Détachement et suppression
    shmdt(shmaddr);
    shmctl(shmid, IPC_RMID, NULL);

    return 0;
}
