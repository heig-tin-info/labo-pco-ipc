#include <stdio.h>
#include <stdlib.h>
#include <signal.h>
#include <unistd.h>

void handler(int signum) {
    printf("\nGoodbye!\n");
    exit(0);
}

int main() {
    struct sigaction sa = { .sa_handler = handler, .sa_flags = 0 };
    sigemptyset(&sa.sa_mask); // Bloque tous les signaux pendant le traitement

    if (sigaction(SIGINT, &sa, NULL) == -1) {
        perror("sigaction");
        exit(EXIT_FAILURE);
    }

    while (1) pause();
}
