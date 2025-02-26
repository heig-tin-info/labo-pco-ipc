#include <stdio.h>
#include <stdlib.h>
#include <fcntl.h>
#include <unistd.h>

int main() {
    int pipefd[2];

    if (pipe(pipefd) == -1) {
        perror("pipe");
        exit(1);
    }

    fcntl(pipefd[0], F_SETFL, O_NONBLOCK);
    fcntl(pipefd[1], F_SETFL, O_NONBLOCK);

    // S'enquérir de la taille du buffer (Linux uniquement, pas POSIX)
    #ifndef F_GETPIPE_SZ
    #    define F_GETPIPE_SZ 1032
    #endif
    int size = fcntl(pipefd[0], F_GETPIPE_SZ);
    if (size == -1)
        perror("fcntl");
    else
        printf("Buffer size : %d octets\n", size);


    // Rempli le buffer
    for (int i = 0; write(pipefd[1], &("ATGC"[rand() % 4]), 1) == 1; i++);

    // Lire jusqu'à ce que le buffer soit vide
    char buffer[16];
    int bytesRead;
    while ((bytesRead = read(pipefd[0], buffer, sizeof(buffer))) > 0) {
        buffer[bytesRead] = '\0';
        printf("%s", buffer);
    }

    close(pipefd[0]);
    close(pipefd[1]);
}
