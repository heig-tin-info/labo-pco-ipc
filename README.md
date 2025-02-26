# Labo PCO - IPC

## Objectif

Comprendre les différents mécanismes de communication inter-processus (IPC) POSIX.

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

## Note sur la conformité POSIX

La norme POSIX s'active en ajoutant la définition `_XOPEN_SOURCE` avant d'inclure les en-têtes. Par exemple :

```c
gcc -std=c2x -D_XOPEN_SOURCE=700 program.c
```

Certains mécanismes sont spécifiques à Linux et ne sont pas POSIX. Attention donc à la portabilité des exemples.

### Signaux

Les signaux sont la forme la plus simple de communication inter-processus. Ils permettent à un processus d'envoyer une notification à un autre processus. Les signaux sont utilisés pour gérer les interruptions, les erreurs, les événements, etc. Ils ne transportent pas de données.

Les fonctions et appels systèmes associés aux signaux sont :

- `kill` : Envoie un signal à un processus.
- `signal` : Associe une fonction de traitement à un signal.
- `sigaction` : Modifie l'action associée à un signal.
- `sigprocmask` : Modifie le masque de signaux du processus.
- `sigpending` : Récupère la liste des signaux en attente.
- `sigwait` : Attend la réception d'un signal.

Les signaux les plus connus sont :

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
```

Note: l'utilisation de `signal`, plus simple, est déconseillée car elle n'est pas portable.

### Sémaphores

Les sémaphores sont des objets de synchronisation qui permettent de contrôler l'accès à une ressource partagée. Ils sont utilisés pour résoudre les problèmes de concurrence entre processus. Ils ne transportent pas de données.

Un sémaphore est un *compteur* qui est utilisé pour contrôler l'accès à une ressource partagée. Il est initialisé à une valeur donnée. Deux opérations sont possibles :

- `P` (Proberen) : Attente. Si le sémaphore est positif, il est décrémenté. Sinon, le processus est mis en attente.
- `V` (Verhogen) : Libération. Incrémente le sémaphore.

Historiquement sous Unix (System V), les sémaphores étaient implémentés avec les appels systèmes `semget`, `semop` et `semctl`. Aujourd'hui, on utilise les sémaphores POSIX.

### Eventfd

`eventfd` est un mécanisme de communication inter-processus qui permet à un processus de signaler un événement à un autre processus. Il est utilisé pour la communication entre processus sur une même machine.

### Signalfd

`signalfd` remplace `sigwait` et `sigaction`. Il permet de lire les signaux comme si c'était des fichiers. Cela permet de les traiter de manière asynchrone.


### Vérouillage de fichiers

Deux processus peuvent exploiter un fichier en même temps leur permettant d'échanger de l'information. Le problème est que si un processus écrit dans le fichier, l'autre processus ne pourra pas lire l'information tant que le premier n'aura pas terminé d'écrire. Il y a donc un problème de synchronisation, et donc de concurrence.

L'astuce est d'utiliser des fonctions de vérouillage de fichiers. Ces fonctions permettent de vérouiller un fichier pour qu'un seul processus puisse y accéder à la fois. Cela permet de synchroniser les accès au fichier.

Imaginons deux processus :

- Processus 1 : producteur de donnée, il écrit dans le fichier
- Processus 2 : consommateur de donnée, il lit dans le fichier

Un exemple est donné [ici](file-locking.c).

### Tuyau anonyme (*anonymous pipe*)

Un tuyau anonyme est un mécanisme **uni-directionnel** de communication entre deux processus. Il est créé par un processus et partagé avec un autre processus. Il est utilisé pour la communication entre un processus père et un processus fils.

Quand vous écrivez dans Bash :

```bash
$ fortune | cowsay -d
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

Les processus sont étanches les uns des autres. Ils ne peuvent pas accéder à la mémoire des autres processus. Même un processus qui utilise `fork` ne peut pas accéder à la mémoire de son père. Cependant, il est possible de partager de la mémoire entre deux processus en utilisant la mémoire partagée. Les fonctions associées sont :

Comment est-ce que cela fonctionne ?

1. Un processus crée un segment mémoire
2. Il attache ce segment à son espace mémoire
3. Un autre processus peut se connecter à ce segment
4. Les deux processus peuvent alors écrire et lire dans ce segment
5. Le segment est détaché de l'espace mémoire des processus
6. Le segment est détruit

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
