# Les Tubes (Pipes)

## 1. Les tubes par défaut

Vous le savez déjà, mais chaque processus dispose de base de 3 flux de données :

- `stdin` : Entrée standard
- `stdout` : Sortie standard
- `stderr` : Sortie d'erreur

Ces flux sont unidirectionnels, c'est-à-dire que `stdin` ne peut être utilisé que pour lire, `stdout` et `stderr` ne peuvent être utilisés que pour écrire.

1. Réalisez un programme de base qui lit un caractère sur l'entrée standard et l'écrit sur la sortie standard.
2. Affichez les appels systèmes du programme.
3. Notez les descripteurs de fichiers ouverts dans `/proc/<pid>/fd/`.

> Que constatez-vous ?
> Identifiez-vous les numéros des descripteurs de fichiers ouverts ?

## 2. Les tubes nommés en ligne de commande

Les tubes nommés sont des fichiers spéciaux qui permettent de communiquer entre deux processus. Ils sont créés avec la commande `mkfifo`.

On commence par créer un tube nommé :

```bash
mkfifo /tmp/mytube
```

Notez le type du fichier créé :

```bash
ls -l /tmp/mytube
```

> Que signifie le `p` en début de ligne ?

Avec le caractère `>` on peut rediriger la sortie standard d'une commande vers un fichier. On peut aussi rediriger la sortie standard d'une commande vers un tube nommé. Essayez :

```bash
echo "Hello" > /tmp/file
cat file

echo "Hello" > /tmp/mytube
cat /tmp/mytube
```

On peut également lire depuis un tube nommé ou un fichier

```bash
cat < /tmp/file

cat < /tmp/mytube
```

> Que constatez-vous ?

Pour supprimer un tube, il suffit d'utiliser `rm` comme tout fichier:

```bash
rm /tmp/mytube
```

## 3. Tubes anonymes en ligne de commande

Un tube n'a pas à être nommé. Il ne peut exister que durant la vie des processus qui l'utilisent. Bash utilise la syntaxe `|` pour créer un tube anonyme.

```bash
echo "Hello" | cat
```

Pour mieux décortiquer ce qui se passe, on peut utiliser `strace` :

```bash
strace -f sh -c 'fortune | cowsay'
```

> Que se passe-t-il ?

## 4. Tubes anonymes en C

Prenez connaissance du programme `echo-cat.c`, exécutez-le et observez les appels systèmes.

> Que font les fonctions `pipe`, `dup2` et `close` ?
> Parvenez-vous à expliquer le comportement du programme ?

## 6. Tubes nommés en C

On s'intéresse maintenant aux programmes `read` et `write`.

Il peut être intéressant d'avoir plusieurs terminaux ouverts pour exécuter les programmes.

> Que pouvez-vous dire sur ces programmes ?
> Quel est le rôle de `mkfifo` ?
> Parvenez-vous à les faire fonctionner ?

## Repères théoriques

### `FILE *` vs `int` (fd)

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

### Descripteurs de fichiers

Un descripteur de fichier est simplement un entier qui identifie un fichier ou un périphérique. Un descripteur de fichier possède l'interface suivante :

- `int open(const char *pathname, int flags, mode_t mode)` : Ouvre un fichier.

### Bufferisation

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
