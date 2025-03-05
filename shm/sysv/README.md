# Mémoire partagée sous Unix System V

Historiquement la mémoire partagée était implémentée avec les appels système `shmget`, `shmat`, `shmdt`, `shmctl` et `shmop` (pour les opérations sur la mémoire partagée). Ces appels système sont toujours disponibles sous Linux, mais ils sont dépréciés et remplacés par `mmap` et `shm_open`.

## 1. Compilation des exemples

Commencez par compiler les exemples donnés dans ce répertoire. Pour cela, exécutez la commande suivante :

```bash
make
```

## 2. Exécution

Tentez d'exécuter les programmes `write` puis `read`, et interceptez les appels système avec `strace` :

```bash
strace ./write 2>&1 | grep shm
strace ./read 2>&1 | grep shm
```

Pour comprendre les appels système, vous pouvez consulter la page de manuel de chaque appel système (par exemple `man 2 shmget`). Utilisez les versions françaises des pages de manuel si vous préférez (`man -L fr 2 shmget`).

## 3. Consultation du kernel

Pour consulter les segments de mémoire partagée actuellement utilisés, vous pouvez utiliser la commande `ipcs` :

```bash
ipcs -m
```

Si vous observez les appels systèmes utilisés par `ipcs` vous verrez qu'il utilise en réalité `/proc/sysvipc/shm` pour obtenir les informations sur les segments de mémoire partagée.

> Quelle est la taille du segment de mémoire associé à la clé 1234 ?
> Que signifie les permissions 666 ?
