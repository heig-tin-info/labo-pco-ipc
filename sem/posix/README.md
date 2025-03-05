# Semaphores POSIX

Maintenant que vous avez exploré les [sémaphores System V](../sysv/README.md), ainsi que la mémoire partagée, intéressons-nous aux sémaphores POSIX.

Ils ont été introduits avec la norme **POSIX.1b-1993** (anciennement **POSIX.4**) dans le but de simplifier l'API. Les sémaphores de *System V* étaient jugés trop complexes et risqué, notamment à cause des identifiants systèmes et de la persistance après les processus.


## Travail pratique

### 1. Compilation et prise de connaissance du code

Pour compiler les exemples, utilisez simplement `make`. Avant l'exécution, prenez connaissance du code et tentez de le comprendre.

> Que fait `sem_post` et `sem_wait`, et que notez vous de différent par rapport aux sémaphores *System V* ?

### 2. Tester les programmes

Essayez les programmes `post` et `wait` et `delete`.

> Que se passe-t-il si `delete` est appelé pendant que `wait` est en attente ?

### 3. Visualiser l'état des sémaphores

Dans cette implémentation, les sémaphores sont implémentés sous forme d'un fichier **binaire**. Il est créé dans le répertoire `/dev/shm/` et dans votre cas il sera nommé `sem.sem`.

```sh
cat /dev/shm/sem.sem | hexdump
```

Tentez d'incrémenter et de décrémenter le sémaphore pour voir comment le fichier est modifié. Vous pouvez influencer deux valeurs 32-bit écrites en *little-endian*.

Observez également les permissions du fichier :

```sh
ls -l /dev/shm/sem.sem
```

> 1. Que représente les premiers 4 octets ?
> 2. Que représente les 4 octets suivants ?
> 3. Comment sont gérées les permissions d'accès au sémaphore ?
> 4. Que signifie `shm` ?

### 4. Observer les appels systèmes

Nous savons que les sémaphores System V utilisent des appels système spécifiques (`semget`, `semop`, `semctl`) directement implémentés dans le noyau. Voyons voir ce qu'il en est pour les sémaphores POSIX.

```sh
strace ./post
strace ./wait
```

On observe notament les appels système suivants :

```c
openat(AT_FDCWD, "/dev/shm/sem.sem", O_RDWR|O_NOFOLLOW|O_CLOEXEC) = 3
mmap(NULL, 32, PROT_READ|PROT_WRITE, MAP_SHARED, 3, 0) = 0x714e5e911000
futex(0x714e5e911000, FUTEX_WAKE, 1) = 0q
```

Les deux premiers devraient vous être familiers, il s'agit du mécanisme POSIX de mémoire partagée. Le dernier est nouveau, vous le trouverez dans `futex(2)` et dans `futex(7)`, allez donc lire les deux manuels.

> D'après le [manuel](https://fr.manpages.org/futex/7) que fait `futex` ?
> À quoi sert `FUTEX_WAIT` et `FUTEX_WAKE` ?

### 5. Hack

Vous avez découvert qu'en réalité l'implémentation des sémaphores POSIX appartiennent à l'implémentation NPTL (Native POSIX Thread Library) de la glibc (dont le code source est disponible dans le répertoire `nptl/` de la glibc), et que contrairement aux sémaphores de *System V*, ils n'utilisent pas une implémentation spécifique dans le noyau mais sont en grande partie résolus en espace utilisateur -- ils sont donc plus rapides.

Puisqu'ils sont implémentés en utilisant de la mémoire partagée, pourriez-vous créer un programme qui :

1. Map la mémoire partagée du sémaphore.
2. Incrémente l'entier à l'adresse de la mémoire partagée.
3. Exécute l'appel système `futex(FUTEX_WAKE)` pour réveiller un processus en attente.

On vous propose de tester le programme `hack.c`:

```sh
./hack
```

> Parvenez-vous à incrémenter le sémaphore ?
> Est-ce que le programme `wait` se débloque ?

## Discussion

Contrairement aux sémaphores *System V*, les sémaphores POSIX ne permettent pas l'incrémentation ou la décrémentation de plusieurs unités à la fois. Cela peut être vu comme une limitation, mais cela simplifie également l'API et résout certains problèmes de concurrence.

En effet, un processus qui attenderait 3 ressources pour continuer serait nécessairement toujours moins prioritaires qu'un processus qui n'en attendrait qu'une seule. Ce processus pourrait être bloqué indéfiniment ce qui crée une situation de **famine**.

D'autre part, vous avez relevé que le mécanisme utilise la mémoire partagée pour stocker les informations des sémaphores au lieu de structures de données complexes et spécifiques hébergées dans le noyau directement.

L'implémentation est donc beaucoup plus simple et les sémaphores POSIX reposent en réalité sur deux concepts :

- `futex` (propre à linux)
- Mémoire partagée POSIX (*shm*)
