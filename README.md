# Labo PCO - IPC

## Objectif

Comprendre les différents mécanismes de communication inter-processus (IPC) POSIX.

## Prérequis

Pour réaliser ce travail, vous devez disposer de :

- Linux (Ubuntu, Debian, Fedora, Arch Linux, etc.)
- GCC (GNU Compiler Collection)
- Make (GNU Make)
- La documentation si possible en français

### Documentation (manpages)

Pour ce travail pratique nous allons utiliser la documentation Linux et principalement les `manpages`. Les `man` pour manual pages sont des pages de manuel qui décrivent les fonctions du système, les fichiers spéciaux, les appels systèmes, etc.

Par défaut la documentation n'est pas nécessairement installée et n'est pas forcément disponible en français.

```bash
sudo apt install -y manpages-fr manpages-posix manpages-posix-dev
wget http://manpagesfr.free.fr/download/man-pages-fr-3.03.0.tar.bz2
tar xvjf man-pages-fr-3.03.0.tar.bz2
cp -r man-pages-fr-3.03.0/man* /usr/share/man/fr
rm man-pages-fr-3.03.0.tar.bz2
```

Pour spécifier la langue par défaut des manpages, vous pouvez ajouter la ligne suivante dans votre fichier `~/.bashrc` :

```bash
alias man='man -L fr'
```

## IPC : Rappel de la définition

Les mécanismes IPC (Inter-Process Communication) permettent à des processus de communiquer entre eux. Ils ont été normalisés par *POSIX.1* en 1988.

Il existe plusieurs mécanismes IPC :

- Les signaux
- Les mémoires partagées
- Les sémaphores
- Les tubes
- Les files de messages
- Les sockets
- Vérouillage de fichiers

Notre objectif est de passer en revue ces différents mécanismes et de démystifier leurs utilisations.

## IPC : Les mécanismes

### Signaux

Les signaux sont la forme la plus simple de communication inter-processus. Ils permettent à un processus d'envoyer une notification à un autre processus. Les signaux sont utilisés pour gérer les interruptions, les erreurs, les événements, etc. Ils ne transportent pas de données.

Rendez-vous à la section [signal](signal/README.md)

### Mémoire partagée

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

Comme pour certains autres IPC, il existe deux implémentations: une héritée de System V (ancienne) et une POSIX (moins ancienne). Les fonctions associées sont les suivantes :

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

> Rendez-vous dans [shm/sysv](shm/sysv/README.md)

### Sémaphores

Les sémaphores sont des objets de synchronisation qui permettent de contrôler l'accès à une ressource partagée. Ils sont utilisés pour résoudre les problèmes de concurrence entre processus et, comme les signaux, ils ne transportent pas de données.

Un sémaphore est basiquement un *compteur* qui s'incrément ou se *décrémente*  pour contrôler l'accès à une ressource partagée. Selon la publication initiale de Dijkstra, un sémaphore accepte deux opérations :

- `P` (Proberen) : Attente. Si le sémaphore est positif, il est décrémenté. Sinon, le processus est mis en attente.
- `V` (Verhogen) : Libération. Incrémente le sémaphore.

Il faut donc voir le sémaphore comme un mécanisme de jetons pour accéder à des outils partagés. Si aucun jetons n'est disponible, vous attendez que quelqu'un ramène l'outil, et dépose le jeton qu'il a pris. Certains outils peuvent nécessiter plusieurs jetons pour être utilisés alors vous devez attendre que le nombre de jetons nécessaires soient disponibles.

#### Sémaphores System V

Historiquement sous Unix (System V), les sémaphores étaient implémentés avec des appels systèmes `semget`, `semop` et `semctl`. C'est un mécanisme simple, toujours supporté par POSIX mais qui n'est plus vraimeent utilisé.

Dans le noyau Linux, ce type de sémaphores sont accessibles depuis `/proc/sysvipc/sem` (*System V IPC / SEMaphores*). Au niveau des sources, ils sont implémentés dans [ipc/sem.c](https://github.com/torvalds/linux/blob/master/ipc/sem.c).

Aujourd'hui, on utilise les sémaphores dit POSIX qui sont une amélioration des sémaphores System V.

Les fonctions associées aux sémaphores de System V sont :

- `semget` : Crée ou accède à un ensemble de sémaphores
- `semop` : Modifie un ensemble de sémaphores
- `semctl` : Contrôle un ensemble de sémaphores

> Rendez-vous dans [sem/sysv](sem/sysv/README.md)

#### Sémaphores POSIX

Commencez par accéder à `man -L fr 7 sem_overview` pour voir la documentation des sémaphores POSIX. Profitez pour installer les manuels en français avec `sudo apt install manpages-fr`.

> Rendez-vous dans [sem/posix](sem/posix/README.md)


### Tube/Tuyaux (*pipes*)

Un tube est un mécanisme **uni-directionnel** de communication entre deux processus. Il est créé par un processus et partagé avec un autre processus. Il peut être notamment utilisé pour la communication entre un processus père et un processus fils.



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

### Vérouillage de fichiers

Deux processus peuvent exploiter un fichier en même temps leur permettant d'échanger de l'information. Le problème est que si un processus écrit dans le fichier, l'autre processus ne pourra pas lire l'information tant que le premier n'aura pas terminé d'écrire. Il y a donc un problème de synchronisation, et donc de concurrence.

L'astuce est d'utiliser des fonctions de vérouillage de fichiers. Ces fonctions permettent de vérouiller un fichier pour qu'un seul processus puisse y accéder à la fois. Cela permet de synchroniser les accès.

Ces opérations sont réalisées avec la fonction `fcntl` et les drapeaux `F_SETLKW`et `F_SETLK` :

Imaginons deux processus :

- Processus 1 : producteur de donnée, il écrit dans le fichier
- Processus 2 : consommateur de donnée, il lit dans le fichier

Testez l'exemple donné [ici](file-locking.c).

### Sockets

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
