# Labo PCO - IPC

## Objectif

Comprendre les différents mécanismes de communication inter-processus (IPC) POSIX.

## Exemples

| IPC              | Chemin             |
| ---------------- | ------------------ |
| Messages         | [msg/](msg/)       |
| Sémaphores       | [sem/](sem/)       |
| Mémoire partagée | [shm/](shm/)       |
| Tuyaux           | [pipe/](pipe/)     |
| Sockets          | [socket/](socket/) |
| Signaux          | [signal/](signal/) |


## IPC : Rappel de la définition

Les mécanismes IPC (Inter-Process Communication) permettent à des processus de communiquer entre eux. Ils ont été normalisés par POSIX.1 en 1988.

Il existe plusieurs mécanismes IPC :

- Vérouillage de fichiers
- Les files de messages
- Les sémaphores
- Les mémoires partagées
- Les tubes nommés
- Les signaux
- Les sockets

Notre objectif est de passer en revue ces différents mécanismes et de démystifier leur utilisation.

## Note sur la conformité POSIX

La norme POSIX s'active en ajoutant la définition `_XOPEN_SOURCE` avant d'inclure les en-têtes. Par exemple :

```c
gcc -std=c2x -D_XOPEN_SOURCE=700 program.c
```

Certains mécanismes sont spécifiques à Linux et ne sont pas POSIX. Attention donc à la portabilité des exemples.

Les systèmes BSD (FreeBSD, OpenBSD, NetBSD) sont conformes à POSIX de même que macOS. Certains anciens systèmes Unix (Solaris, AIX, HP-UX) l'étaient également. En outre Linux est un cas particulier car il n'hérite pas directement du code source d'Unix. Il a été réécrit de zéro en 1991 par Linus Torvalds. Face à la popularité de Linux certaines libertés ont été prises et Linux est dit "Unix-like" et non "Unix", il a une très grande compatibilité POSIX mais n'est pas 100% conforme et ajoute des fonctionnalités non-POSIX qui lui sont propres.

### Signaux

Les signaux sont la forme la plus simple de communication inter-processus. Ils permettent à un processus d'envoyer une notification à un autre processus. Les signaux sont utilisés pour gérer les interruptions, les erreurs, les événements, etc. Ils ne transportent pas de données.

Les fonctions et appels systèmes associés aux signaux sont les suivantes. La documentation est accessible avec `man 2 <fonction>` :

- `kill` : Envoie un signal à un processus.
- `signal` : Associe une fonction de traitement à un signal *(obsolète)*
- `sigaction` : Modifie l'action associée à un signal.
- `sigprocmask` : Modifie le masque de signaux du processus.
- `sigpending` : Récupère la liste des signaux en attente.
- `sigwait` : Attend la réception d'un signal.

Les signaux les plus connus sont (accessible depuis `man 7 signal`) :

- `SIGINT` : Interruption depuis le clavier (Ctrl+C)
- `SIGKILL` : Tue un processus (Pas interceptable)
- `SIGSTOP` : Met en pause un processus immédiatement (Pas interceptable)
- `SIGSTP` : Met en pause un processus (Ctrl+Z)
- `SIGCONT` : Relance un processus arrêté
- `SIGCHLD` : Un processus fils s'est terminé

Il est courant de redéfinir le comportement d'un signal en utilisant la fonction `signal` ou `sigaction`. Par exemple, pour redéfinir le comportement du signal `SIGINT` et dire aurevoir avant de quitter :

```c
#include <stdio.h>
#include <stdlib.h>
#include <signal.h>
#include <unistd.h>

void handler(int signum) {
    printf("Goodbye!\n");
    exit(EXIT_SUCCESS);
}

int main() {
    struct sigaction sa = { .sa_handler = handler, .sa_flags = 0 };
    sigemptyset(&sa.sa_mask); // Bloque tous les signaux pendant le traitement

    if (sigaction(SIGINT, &sa, NULL) == -1) {
        perror("sigaction");
        exit(EXIT_FAILURE);
    }

    for (;;) pause();
}
```

Notez qu'il est également possible d'accéder aux signaux en attente depuis `/proc`:

```text
cat /proc/3660502/status | grep Sig
SigQ:   2/256994          # Signaux temps réel en attente
SigPnd: 0000000000000000  # Nombre de signaux en attente
SigBlk: 0000000000000000  # Signaux bloqués (masqués)
SigIgn: 0000000000000000  # Signaux ignorés par le processus
SigCgt: 0000000000010002  # Signaux capturés par le processus
```

Le kernel permet de modifier la limite des signaux en attentes avec `ulimit -i`.

```text
ulimit -q # Limite par défaut
ulimit -q 200000 # Limite à 200000 signaux pour le shelle courant
```

#### Exercice

Créer un programme en C qui capture `SIGINT`, `SIGUSR1` et `SIGUSR2` et qui ignore `SIGALRM`. Le programme doit afficher un message lors de la réception de chaque signal capturés.

1. Essayez d'envoyer des signaux au processus
2. Vérifier que l'état de `SigCgt` est correct avec votre configuration (utiliser `/proc` et l'aide de `man 7 signal`).
3. Vérifiez que l'état de `SigIgn` est correct avec votre configuration.

#### Quiz

1. Quel est le signal envoyé par défaut avec `Ctrl+C` ?
2. Quel est le signal envoyé par défaut avec `Ctrl+Z` ?
3. Quel est le signal envoyé par défaut avec `kill` ?
4. De combien de signaux utilisateurs un processus peut-il disposer ?

### Sémaphores

Les sémaphores sont des objets de synchronisation qui permettent de contrôler l'accès à une ressource partagée. Ils sont utilisés pour résoudre les problèmes de concurrence entre processus et comme les signaux, ils ne transportent pas de données.

Un sémaphore est basiquement un *compteur* qui s'incrément ou se *décrémente*  pour contrôler l'accès à une ressource partagée. Il démarre à une donnée et deux opérations sont possibles :

- `P` (Proberen) : Attente. Si le sémaphore est positif, il est décrémenté. Sinon, le processus est mis en attente.
- `V` (Verhogen) : Libération. Incrémente le sémaphore.

Il faut voir le sémaphore comme un mécanisme de jetons pour accéder à des outils partagés. Si aucun jetons n'est disponible, vous attendez que quelqu'un ramène l'outil, et dépose le jeton qu'il a pris. Certains outils peuvent nécessiter plusieurs jetons pour être utilisés alors vous devez attendre que le nombre de jetons nécessaires soient disponibles.

#### Sémaphores System V

Historiquement sous Unix (System V), les sémaphores étaient implémentés avec les appels systèmes `semget`, `semop` et `semctl`. C'est un mécanisme simple, toujours supporté par POSIX mais qui n'est plus utilisé réelle.
Aujourd'hui, on utilise les sémaphores dit POSIX qui sont une amélioration des sémaphores System V.

Les fonctions associées aux sémaphores de System V sont :

- `semget` : Crée ou accède à un ensemble de sémaphores
- `semop` : Modifie un ensemble de sémaphores
- `semctl` : Contrôle un ensemble de sémaphores

Notez que le kernel met à disposition l'état de ces sémaphores dans `/proc/sysvipc/sem` (*System V IPC / SEMaphores*).

Pour tester cela écrire deux programmes :

1. `verhogen.c` qui incrémente le sémaphore à la clé 1234.
2. `proberen.c` qui attend qu'une ressource soit disponible sur le sémaphore 1234, puis décrémente le sémaphore et affiche un message.

La première étape est de créer un ensemble de sémaphores avec `semget` :

```c
int semid = semget(1234, 1 /* Nombre de sémaphores */,
    IPC_CREAT /* Créer si n'existe pas */
    | 0666 /* Droits d'accès */);
```

Il faut s'assurer que `semid` est différent de `-1` pour vérifier que l'ensemble de sémaphores a bien été créé.

Puis on peut incrémenter le sémaphore avec `semop` :

```c
semop(semid, &(struct sembuf){.sem_op = 1}, 1);
```

On selectionne le sémaphore 0, sans falgs et on incrémente le sémaphore de 1.

Pour vérifier la valeur du sémaphore, on peut utiliser `semctl` :

```c
int value = semctl(semid, 0, GETVAL);
```

Si vous êtes bloqués, rendez-vous dans `sem/` pour voir la solution et la tester.

#### Sémaphores POSIX

Rendez-vous dans `man -L fr 7 sem_overview` pour voir la documentation des sémaphores POSIX. Profitez pour installer les manuels en français avec `sudo apt install manpages-fr`. Cela vous renseigne sur les différentes fonctions associées aux sémaphores POSIX :

- `sem_open` : Crée ou ouvre un sémaphore
- `sem_close` : Ferme un sémaphore
- `sem_unlink` : Supprime un sémaphore
- `sem_init` : Initialise un sémaphore
- `sem_post` : Incrémente un sémaphore
- `sem_wait` : Décrémente un sémaphore
- `sem_getvalue` : Récupère la valeur d'un sémaphore

Les sémaphores POSIX sont plus simples à utiliser que les sémaphores System V. Contrairement aux sémaphores System V qui possède une clé et qui sont partagés entre les processus, les sémaphores POSIX sont stockés dans le système de fichier dans `/dev/shm` et sont accessibles par tous les processus.

### Vérouillage de fichiers

Deux processus peuvent exploiter un fichier en même temps leur permettant d'échanger de l'information. Le problème est que si un processus écrit dans le fichier, l'autre processus ne pourra pas lire l'information tant que le premier n'aura pas terminé d'écrire. Il y a donc un problème de synchronisation, et donc de concurrence.

L'astuce est d'utiliser des fonctions de vérouillage de fichiers. Ces fonctions permettent de vérouiller un fichier pour qu'un seul processus puisse y accéder à la fois. Cela permet de synchroniser les accès.

Ces opérations sont réalisées avec la fonction `fcntl` et les drapeaux `F_SETLKW`et `F_SETLK` :

Imaginons deux processus :

- Processus 1 : producteur de donnée, il écrit dans le fichier
- Processus 2 : consommateur de donnée, il lit dans le fichier

Testez l'exemple donné [ici](file-locking.c).

### Tube/Tuyaux (*pipes*)

Un tube est un mécanisme **uni-directionnel** de communication entre deux processus. Il est créé par un processus et partagé avec un autre processus. Il peut être notamment utilisé pour la communication entre un processus père et un processus fils.

Chaque processus dispose de base de 3 flux de données :

- `stdin` : Entrée standard
- `stdout` : Sortie standard
- `stderr` : Sortie d'erreur

Note que vous pouvez accéder aux descripteurs de fichiers ouverts dans `/proc/<pid>/fd/`.

#### Tuyau anonyme (*anonymous pipe*)

Un tuyau anonyme comme son nom l'indique ne possède pas de nom. Il est volatile et ne peut être utilisé que par des processus qui partagent un ancêtre commun.

Quand vous écrivez dans Bash :

```bash
fortune | cowsay -d
```

Ce qui se passe derrière c'est que `fortune` écrit dans un tuyau anonyme et `cowsay` lit dans ce tuyau.

```c
// pipe-fortune-cowsay.c
#include <fcntl.h>
#include <stdio.h>
#include <stdlib.h>
#include <sys/types.h>
#include <sys/wait.h>
#include <unistd.h>

int main() {
    int pipefd[2]; // A pipe has two ends: pipefd[0] (output) and pipefd[1] (input)
    if (pipe(pipefd) == -1) {
        perror("pipe");
        exit(1);
    }

    pid_t pid = fork();
    if (pid == -1) {
        perror("fork");
        exit(1);
    }

    if (pid == 0) {
        // Child process
        close(pipefd[0]);
        dup2(pipefd[1], STDOUT_FILENO);  // Redirect stdout to pipefd[1]
        close(pipefd[1]);
        execlp("fortune", "fortune", NULL);
    } else {
        // Parent process
        close(pipefd[1]);
        dup2(pipefd[0], STDIN_FILENO);  // Redirect stdin to pipefd[0]
        close(pipefd[0]);
        execlp("cowsay", "cowsay", "-d", NULL);
    }
}
```

L'appel système `dup2` prend un descripteur de fichier (numéro de fichier ouvert) et le remplace par un autre. Ici, on associe une extrémité du tube soit l'entrée standard (`STDIN_FILENO`) soit la sortie standard (`STDOUT_FILENO`) à l'identifiant de cette extrémité.

### Tuyau nommé (*named pipe*)

Un tuyau nommé est un mécanisme **bi-directionnel** de communication entre deux processus. Sa durée de vie n'est pas celle du processus comme le tuyau anonyme qui est détruit à la fin du processus. Il peut donc être partagé entre plusieurs processus.

### Files de messages (*message queue*)

Les files de messages permettent aux processus d'échanger des messages d'une manière structurée. Chaque message est placé dans une file d'attente et est lu par un autre processus. Les messages sont délivrés dans l'ordre dans lequel ils ont été envoyés.

De la même manière que pour les sémaphores, les files de messages ont tout d'abord été implémentées dans Unix System V. Aujourd'hui, on utilise les files de messages POSIX.

Historiquement sous Unix (System V), les fonctions étaient :

- `msgget` : Crée ou accède à une file de messages
- `msgsnd` : Envoie un message dans une file de messages
- `msgrcv` : Reçoit un message dans une file de messages
- `msgctl` : Contrôle une file de messages

Avec POSIX on a :

- `mq_open` : Crée ou ouvre une file de messages
- `mq_send` : Envoie un message dans une file de messages
- `mq_receive` : Reçoit un message dans une file de messages
- `mq_close` : Ferme une file de messages
- `mq_unlink` : Supprime une file de messages
- `mq_getattr` : Récupère les attributs d'une file de messages

## Mémoire partagée

Les processus sont étanches les uns des autres. Ils ne peuvent pas accéder à la mémoire des autres processus. Même un processus qui utilise `fork` ne peut pas accéder à la mémoire de son père. La preuve en code :

```c
#include <stdio.h>
#include <sys/wait.h>
#include <unistd.h>

int x = 42;

int main() {
    if (fork() == 0) {
        x = 0;
    } else {
        wait(NULL);
    }
    printf("x = %d\n", x); // Affiche 0 puis 42... pourquoi ?
}
```

Cependant, il est néanmoins possible de partager de la mémoire entre deux processus en utilisant la **mémoire partagée**.

Comme pour les autres IPC que nous avons vu, il existe deux implémentations toujours dans le kernel, une héritée de System V (ancienne) et une POSIX (moderne). Les fonctions associées sont les suivantes :

| Critère                 | System V               | POSIX                        |
| ----------------------- | ---------------------- | ---------------------------- |
| Création                | `shmget()`             | `shm_open()`                 |
| Attachement             | `shmat()`              | `mmap()`                     |
| Détachement             | `shmdt()`              | `munmap()`                   |
| Suppression             | `shmctl(IPC_RMID)`     | `shm_unlink()`               |
| Visibilité              | `ipcs -m`              | Fichiers dans `/dev/shm`     |
| Stockage                | Géré par le noyau      | Fichier en mémoire (`tmpfs`) |
| Simplicité              | Plus complexe          | Plus moderne et simple       |
| Utilisation recommandée | Applications anciennes | Applications modernes        |

### Mmap

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

## Sockets

Enfin, les sockets. Contrairement aux mécanismes que nous avons vu précédemment, les sockets permettent la communication entre processus sur des machines distantes via le réseau. Les sockets sont très fréquemment utilisé au sein d'une même machine pour la communication inter-processus, et dans une large mesure en remplacement des autres mécanismes IPC.

Néanmoins lorsque les performances sont critiques, il est préférable d'utiliser les sockets Unix qui ne passent pas par la couche réseau et qui utilsient un fichier pour la communication. Docker ou MySQL utilisent ce mécanisme pour communiquer entre les différents processus.

```c
int socket(int domain, int type, int protocol);  // Créer une socket
int bind(int sockfd, const struct sockaddr *addr, socklen_t addrlen);  // Associer une adresse
int listen(int sockfd, int backlog);  // Passer en mode écoute
int accept(int sockfd, struct sockaddr *addr, socklen_t *addrlen);  // Accepter une connexion
int connect(int sockfd, const struct sockaddr *addr, socklen_t addrlen);  // Se connecter à un serveur
ssize_t send(int sockfd, const void *buf, size_t len, int flags);  // Envoyer des données
ssize_t recv(int sockfd, void *buf, size_t len, int flags);  // Recevoir des données
int close(int sockfd);  // Fermer une socket
```

### Eventfd

`eventfd` est un mécanisme de communication inter-processus qui permet à un processus de signaler un événement à un autre processus. Il est utilisé pour la communication entre processus sur une même machine.

### Signalfd

`signalfd` remplace `sigwait` et `sigaction`. Il permet de lire les signaux comme si c'était des fichiers. Cela permet de les traiter de manière asynchrone.



## Appels systèmes

- `fork` : Crée un nouveau processus en dupliquant le processus appelant.
- `fcntl` : Manipule les descripteurs de fichiers.
- `open` : Ouvre un fichier.
- `close` : Ferme un fichier.
- `read` : Lit des données depuis un fichier.
- `write` : Ecrit des données dans un fichier.
- `lseek` : Déplace le curseur de lecture/écriture dans un fichier.
- `ftruncate` : Redimensionne un fichier.
- `sigaction` : Modifie l'action associée à un signal.
- `dup` : Duplique un descripteur de fichier.
- `pipe` : Crée un tuyau anonyme.
- `exec` : Remplace l'image mémoire du processus courant par un nouveau programme.

## Conclusion

Des mécanismes que nous avons vu, les mécanismes hérité de **System V** ne sont plus réellement utilisés (mémoire partagée, sémaphore, file de message). Les tuyaux nommés sont également peu utilisés et le système de vérrouillage de fichiers est peu performant et présente des risques de blocage.

Dans l'ordre les mécanismes les plus utilisés entre processus sont :

1. Les tuyaux anonymes (facile à mettre en place, utilisé partout)
2. Les signaux (pour les interruptions)
3. Les Sockets Unix. Ils sont ultra performants, sécurisés. Ils sont utilisés par Docker, MySQL, X11, Systemd, etc.
4. Les Sockets TCP/IP pour la communication entre machines distantes.
5. La mémoire partagée POSIX (`shm_open` et `mmap`).
6. Les sémaphores POSIX et les fils de messages POSIX.
