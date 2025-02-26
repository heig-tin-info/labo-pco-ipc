# Tuyaux (Pipes)

## FILE * vs fd (int)

Dans les exemples précédents, nous avons utilisé des `FILE *` pour lire et écrire dans des fichiers. Cependant, les appels systèmes utilisent des descripteurs de fichiers (`int`).

En réalité le `FILE *` est une abstraction proposée par la `libc`. Elle est utilisée pour simplifier la manipulation des fichiers. En interne, la `libc` utilise des descripteurs de fichiers pour effectuer les opérations de lecture et d'écriture. D'ailleurs dans le type abstrait `FILE` se cache une structure qui contient un descripteur de fichier. Voici à quoi elle pourrait ressembler sous Linux :

```c
typedef struct {
    int fd; // Le fameux descripteur de fichier
    char *buffer;
    size_t buffer_size;
    size_t buffer_pos;
    int flags;
    // ...
} FILE;
```

## Descripteurs de fichiers

Un descripteur de fichier est simplement un entier qui identifie un fichier ou un périphérique. Un descripteur de fichier possède l'interface suivante :

- `int open(const char *pathname, int flags, mode_t mode)` : Ouvre un fichier.

## Bufferisation

Un descripteur de fichier ou un pipe peut disposer d'une mémoire tampon d'environ 64 Ko. Cela signifie que les données écrites dans un fichier ou un pipe ne sont pas immédiatement écrites sur le disque ou lus par le processus de lecture. Elles sont d'abord stockées dans une mémoire tampon. Cela permet d'améliorer les performances en réduisant le nombre d'appels systèmes.

On peut s'amuser par exemple à créer un pipe et utiliser sa mémoire tampon pour stocker des données :

```c
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
```
