# Mémoire partagée POSIX

La mémoire partagée POSIX est un mécanisme de partage de données entre processus. Elle est plus flexible que la mémoire partagée System V et plus simple à utiliser. Elle est basée sur des fichiers et utilise des noms pour identifier les segments de mémoire partagée.

Dans ce mécanisme seul deux appels système sont utilisés :

- `shm_open` : Crée ou ouvre un segment de mémoire partagée
- `mmap` : Associe le segment de mémoire partagée à l'espace d'adressage du processus

## 1. Compilation

Commencez par compiler les exemples avec

```bash
make
```

## 2. Lecteur écrivain

Les programmes `read.c` et `write.c` représentent un exemple de communication entre deux processus à l'aide de la mémoire partagée POSIX. Le programme `write` écrit un message dans la mémoire partagée et le programme `read` lit le message.

Commencez par lancer le programme `write` :

```bash
./write
```

Notez que dans `/dev/shm` vous avez un fichier qui correspond au nom du segment de mémoire partagée que vous avez ouvert. Vous pouvez le lire avec `cat`.

> Que lisez-vous ?
> Quel est la taille de ce segment de mémoire partagée ?

Ensuite, lancez le programme `read` :

```bash
./read
```

> Que lisez-vous ?

## 3. Hack

On va s'amuser a compromettre le segment de mémoire partagée en écrivant directement sur le fichier avec `dd` et insérant `"HACKED"` au début du message.

```bash
echo -n "HACKED" | dd of=/dev/shm/posix_shm bs=1 seek=0 conv=notrunc
```

Consultez le contenu du segment de mémoire partagée avec `cat` :

```bash
cat /dev/shm/posix_shm
```

Puis exécutez le programme `read` :

```bash
./read
```

> Que constatez-vous ?

## 4. Segment de mémoire partagée anonyme

Il est également possible de n'utiliser que l'appel système `mmap` pour créer des segments de mémoire partagée anonymes. Cela permet de partager des données entre processus sans avoir à créer de fichier. Néanmoins, ce mécanisme n'est pas persistant et ne peut être utilisé que pour des processus ayant un lien de parenté.

Prenez connaissances du programme `read-write.c` et exécutez-le.

> C'est quoi pthread?

## Mmap

Dans la variante moderne, l'appel système `mmap` est utilisé. Il permet de *mapper* (associer) un fichier dans la mémoire du processus.

```c
void *mmap(
    void *addr, // Adresse de base suggérée (NULL pour laisser le kernel choisir)
    size_t length, // Taille de la mémoire à mapper
    int prot, // Protection (PROT_READ, PROT_WRITE, PROT_EXEC, PROT_NONE)
    int flags, // Flags (MAP_SHARED, MAP_PRIVATE, MAP_ANONYMOUS, MAP_FIXED)
    int fd, // Descripteur de fichier (-1 si MAP_ANONYMOUS)
    off_t offset // Décalage dans le fichier multiplié par sysconf(_SC_PAGE_SIZE)
); // Retourne MAP_FAILED en cas d'erreur
```

Une utilisation classique est de gagner du temps à la lecture d'un fichier en le mappant directement en mémoire :

```c
#include <stdio.h>
#include <stdlib.h>
#include <sys/mman.h>
#include <fcntl.h>
#include <sys/stat.h>
#include <unistd.h>

int main() {
    int fd = open("test.txt", O_RDWR);
    struct stat sb;
    fstat(fd, &sb); // Get file size
    char *data = mmap(NULL, sb.st_size, PROT_READ | PROT_WRITE, MAP_PRIVATE, fd, 0);
    data[42] = 'A'; // Modify the file in memory
    write(STDOUT_FILENO, data, sb.st_size);
    munmap(data, sb.st_size);
    close(fd);
}
```

Allocation de mémoire anonyme. C'est exactement ce que fait `malloc` mais la libc gère également un pool de mémoire pour éviter d'appeler `mmap` à chaque fois.

```c
#include <stdio.h>
#include <stdlib.h>
#include <sys/mman.h>

#define SIZE 4096

int main() {
    char *buffer = mmap(NULL, SIZE, PROT_READ | PROT_WRITE,
        MAP_ANONYMOUS | MAP_PRIVATE, -1, 0);
    if (buffer == MAP_FAILED) {
        perror("mmap");
        exit(EXIT_FAILURE);
    }

    sprintf(buffer, "Mémoire allouée avec mmap!");
    printf("%s\n", buffer);

    munmap(buffer, SIZE);
}
```
