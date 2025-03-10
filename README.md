# Labo PCO - IPC

## Objectif

Comprendre les différents mécanismes de communication inter-processus (IPC) POSIX.

## Prérequis

Pour réaliser ce travail, vous devez disposer de :

- Linux (Ubuntu, Debian, Fedora, Arch Linux, etc.)
- GCC (GNU Compiler Collection)
- Make (GNU Make)

## Documentation (manpages)

Pour ce travail pratique nous allons utiliser la documentation Linux et principalement les `manpages`. Les `man` pour manual pages sont des pages de manuel qui décrivent les fonctions du système, les fichiers spéciaux, les appels systèmes, etc.

Par défaut la documentation n'est pas nécessairement installée et n'est pas forcément disponible en français.

```bash
sudo apt install -y manpages-fr manpages-posix manpages-posix-dev
wget http://manpagesfr.free.fr/download/man-pages-fr-3.03.0.tar.bz2
tar xvjf man-pages-fr-3.03.0.tar.bz2
sudo cp -r man-pages-fr-3.03.0/man* /usr/share/man/fr
rm man-pages-fr-3.03.0.tar.bz2
```

Pour spécifier la langue par défaut des manpages, vous pouvez ajouter la ligne suivante dans votre fichier `~/.bashrc` :

```bash
alias man='man -L fr'
```

Puis recharger votre fichier de configuration :

```bash
source ~/.bashrc
```

### 1. Les manpages ?

Les manpages sont des pages de manuel qui décrivent les fonctions du système, les fichiers spéciaux, les appels systèmes, etc. Il s'agit d'un format imaginé en 1971 par Dennis Ritchie et Ken Thompson pour documenter les fonctions du système Unix lorsqu'il étaient chez Bell Labs.

Ces fichiers textes sont stockés dans `/usr/share/man` et sont organisés en sections :

- Section 1 : Commandes utilisateur
- Section 2 : Appels système
- Section 3 : Fonctions de bibliothèque
- Section 4 : Fichiers spéciaux
- Section 5 : Formats de fichiers
- Section 6 : Jeux
- Section 7 : Conventions, protocoles, etc.
- Section 8 : Commandes administratives
- Section 9 : Fonctions du noyau

Le format `troff` (anciennement `roff`) est interpreté par `man` pour afficher les manpages. Il est possible de convertir ces fichiers en `HTML` ou `PDF` avec `man -Thtml` ou `man -Tpdf`.

1. Regardez le contenu brute de nice avec `zless /usr/share/man/man2/nice.2.gz`.
2. Maintenant regardez la version formatée avec `man 2 nice`.

> Pourquoi utiliser `zless` au lieu de `less` ?

### 2. Invoquer `man`

Tout d'abord, pour afficher une entrée man le plus simple c'est d'utiliser `man` suivi du nom de la commande ou de la fonction.

```bash
man ls
```

Lorsque plusieurs entrées sont en conflit comme `printf` qui peut faire référence soit à la commande `printf` soit à la fonction `printf`, vous pouvez spécifier la section:

```bash
man 1 printf  # Pour afficher la commande
man 3 printf  # Pour afficher la fonction C standard
```

Parfois vous ne connaissez pas le nom exact de la commande ou de la fonction, vous pouvez utiliser `man -k` pour chercher une commande.

```bash
man -k print
man -k print | grep -P '\bformat'
```

Lorsque vous ouvrez un manpage, `man` ne s'occupe pas de l'affichage. Sa responsabilité c'est de :

1. Chercher la manpage dans `/usr/share/man`
2. Vérifier la section demandée
3. Décompresser le fichier
4. Applique la mise en page avec l'outil `troff`/`groff`
5. Envoi le résultat à `less` pour l'affichage selon l'état de la variable d'environnement `PAGER`.

### 3. `less`

Il s'agit d'une évolution de `more` qui permet de lire des fichiers texte. `less` permet de naviguer dans le fichier avec des raccourcis clavier empruntés à `vi`. Voici l'essentiel à retenir :

| Raccourci     | Action                                               |
| ------------- | ---------------------------------------------------- |
| `j`, ↓, Enter | Défilement vers le bas de 1 ligne                    |
| `k` ou ↑      | Défilement vers le haut de 1 ligne                   |
| espace        | Défilement vers le bas d'un certain nombre de lignes |
| `g`           | Aller au début du fichier                            |
| `G`           | Aller à la fin du fichier                            |
| `/`           | Recherche de texte                                   |
| `n`           | Aller à l'occurrence suivante                        |
| `N`           | Aller à l'occurrence précédente                      |
| `q`           | Quitter `less`                                       |

Notez que la plupart des commandes de saut fonctionnent avec un préfixe numérique. Par exemple, `5j` pour descendre de 5 lignes, ou `42g` pour aller à la ligne 42.

Cela peut vous paraître compliqué mais ce sont là des raccourcis historiques que tout informaticien travaillant sur des environnements POSIX devrait connaître.

1. Ouvrez `man 1 less`.
2. Allez à la fin du fichier pour connaître le nom de l'auteur.
3. Recherchez un mot dans le fichier et naviguer dans les occurences avec `n` et `N`.

Très bien, vous avez maintenant les bases pour naviguer dans les manpages. Nous allons maintenant voir comment utiliser les manpages pour comprendre les IPC.

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

> Rendez-vous à la section [signal](signal/README.md) pour la suite

### Mémoire partagée

Les processus sont *étanches* les uns des autres. Ils ne peuvent pas accéder à la mémoire des autres processus. Même un processus qui utilise `fork` ne peut pas accéder à la mémoire de son père. La preuve en code :

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

> Pourquoi la variable `x` n'est-elle pas modifiée dans le parent ?

Cependant, il est néanmoins possible de partager de la mémoire entre deux processus en utilisant de la **mémoire partagée**.

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

> Rendez-vous dans [shm/sysv](shm/sysv/README.md) pour la suite

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

Le noyau Linux implémente deux types de tuyaux :

- Les tuyaux anonymes (*anonymous pipes*)
- Les tuyaux nommés (*named pipes*)

> Rendez-vous dans [pipe](pipe/README.md)

### Files de messages (*message queue*)

Les files de messages permettent aux processus d'échanger des messages d'une manière structurée. Chaque message est placé dans une file d'attente et est lu par un autre processus. Les messages sont délivrés dans l'ordre dans lequel ils ont été envoyés.

De la même manière que pour les sémaphores et la mémoire partagée, les files de messages ont tout d'abord été implémentées dans **Unix** *System V*. Aujourd'hui, on utilise les files de messages **POSIX**.

Historiquement sous **Unix** (*System V*), les fonctions étaient :

- `msgget` : Crée ou accède à une file de messages
- `msgsnd` : Envoie un message dans une file de messages
- `msgrcv` : Reçoit un message dans une file de messages
- `msgctl` : Contrôle une file de messages

Avec **POSIX** on a :

- `mq_open` : Crée ou ouvre une file de messages
- `mq_send` : Envoie un message dans une file de messages
- `mq_receive` : Reçoit un message dans une file de messages
- `mq_close` : Ferme une file de messages
- `mq_unlink` : Supprime une file de messages
- `mq_getattr` : Récupère les attributs d'une file de messages

> Rendez-vous dans [msg/sysv](msg/sysv/README.md)
> Rendez-vous dans [msg/posix](msg/posix/README.md)

### Vérouillage de fichiers

Deux processus peuvent exploiter un fichier en même temps leur permettant d'échanger de l'information. Le problème est que si un processus écrit dans le fichier, l'autre processus ne pourra pas lire l'information tant que le premier n'aura pas terminé d'écrire. Il y a donc un problème de synchronisation, et donc de concurrence.

> Rendez-vous dans [lock](lock/README.md)

### Sockets

Contrairement aux mécanismes que nous avons vu précédemment, les sockets permettent la communication entre processus sur des machines distantes via le réseau (internet). Néanmoins, les sockets sont très fréquemment utilisé au sein d'une même machine pour la communication inter-processus, et dans une large mesure en remplacement des autres mécanismes IPC.

> Rendez-vous dans [sock/udp](sock/udp/README.md)
> Renvez-vous dans [sock/tcp](sock/tcp/README.md)
> Rendez-vous dans [sock/unix](sock/unix/README.md)

## Conclusion

Vous avez maintenant une vue d'ensemble des différents mécanismes IPC. Vous avez vu :

- les signaux,
- la mémoire partagée,
- les sémaphores,
- les tubes,
- les files de messages,
- le vérouillage de fichiers et
- les sockets.

L'objectif n'est pas que vous sachiez tout sur ces mécanismes mais que vous sachiez qu'ils existent et que vous puissiez les utiliser lorsque vous en aurez besoin et que vous puissiez vous appuyer sur des exemples pour les mettre en place.
