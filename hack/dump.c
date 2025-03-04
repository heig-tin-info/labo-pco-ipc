#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <time.h>
#include <stdio.h>
#include <stdlib.h>
#include <sys/mman.h>

#define SECRET_SIZE 32

void generate_passphrase(char *buffer, size_t size) {
    const char charset[] = "abcdefghijklmnopqrstuvwxyzABCDEFGHIJKLMNOPQRSTUVWXYZ0123456789";
    for (size_t i = 0; i < size - 1; i++) {
        buffer[i] = charset[rand() % (sizeof(charset) - 1)];
    }
    buffer[size - 1] = '\0';
}

int main() {
    char *secret = mmap(NULL, SECRET_SIZE, PROT_READ | PROT_WRITE,
        MAP_ANONYMOUS | MAP_PRIVATE, -1, 0);
    if (secret == MAP_FAILED) {
        perror("mmap");
        exit(EXIT_FAILURE);
    }

    generate_passphrase(secret, SECRET_SIZE);

    printf("Guess the passphrase: ");
    char input[SECRET_SIZE];
    scanf("%31s", input);

    if (strcmp(input, secret) == 0) {
        printf("Congratulations! You found the passphrase!\n");
    } else {
        printf("Sorry, the passphrase is incorrect.\n");
    }

    munmap(secret, SECRET_SIZE);
}
