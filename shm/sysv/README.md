# Mémoire partagée sous Unix System V

Historiquement la mémoire partagée était implémentée avec les appels système `shmget`, `shmat`, `shmdt`, `shmctl` et `shmop` (pour les opérations sur la mémoire partagée). Ces appels système sont toujours disponibles sous Linux, mais ils sont dépréciés et remplacés par `mmap` et `shm_open`.

## 1. Compilation des exemples

Commencez par compiler les exemples donnés dans ce répertoire. Pour cela, exécutez la commande suivante :

```bash
make
```

> Pourquoi est-ce que dans le Makefile il n'y a aucune règle de compilation mais que les programmes sont tout de même compilés ?

## 2. Exécution

Tentez d'exécuter les programmes `write` puis `read`, et interceptez les appels système avec `strace` :

```bash
strace ./write 2>&1 | grep shm
strace ./read 2>&1 | grep shm
```

Pour comprendre les appels système, vous pouvez consulter la page de manuel de chaque appel système p.ex. `shmget(2)`.

> Voyez-vous une correspondance entre les appels système et les actions effectuées par les programmes `write` et `read` ?

## 3. Consultation du kernel

Pour consulter les segments de mémoire partagée actuellement utilisés, vous pouvez utiliser la commande `ipcs` :

```bash
ipcs -m
```

Si vous observez les appels systèmes utilisés par `ipcs` vous verrez qu'il utilise en réalité `/proc/sysvipc/shm` pour obtenir les informations sur les segments de mémoire partagée.

> Quelle est la taille du segment de mémoire associé à la clé 1234 ?
> Que signifie les permissions 666 ?

## 4. Condition de course

Le programme `race` simule une condition de course. Une **condition de course** est une situation où le comportement du programme dépend de l'ordre d'exécution des threads ou des processus.

1. Prenez connaissance de `race.c` et `worker.c`
2. Exécutez le programme `race` plusieurs fois
3. Observer le comportement du programme

L'écriture dans la mémoire partagée est appelée une **section critique**. Pour éviter les conditions de course, il est nécessaire de synchroniser les accès à la mémoire partagée. Pour cela, il est possible d'utiliser des **sémaphores** ou des **verrous**, ce que nous allons aborder dans les prochains chapitres.

> Pourquoi est-ce le contenu final du segment de mémoire partagée est différent à chaque exécution ?
