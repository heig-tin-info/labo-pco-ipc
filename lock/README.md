# Verouillage de fichiers

L'astuce est d'utiliser des fonctions de vérouillage de fichiers. Ces fonctions permettent de vérouiller un fichier pour qu'un seul processus puisse y accéder à la fois. Cela permet de synchroniser les accès.

Ces opérations sont réalisées avec la fonction `fcntl` et les drapeaux `F_SETLKW`et `F_SETLK` :

Imaginons deux processus :

- Processus 1 : producteur de donnée, il écrit dans le fichier.
- Processus 2 : consommateur de donnée, il lit dans le fichier.

## 1. Compiler et tester

Compiler et tester le programme lock.c

> Dans le code vous voyez des sections critiques, Qu'est-ce qu'une section critique ?
> Parvenez-vous a expliquer comment fonctionne le code ?

## 2. `fcntl`

L'appel système `fcntl` (File CoNTroL) permet de manipuler des descripteurs de fichiers. Il est utilisé pour modifier les propriétés d'un fichier.

Il peut également être utilisé pour vérouiller des fichiers (vérouillage exclusif ou partagé).

Lors de la pose d'un verrou il y a deux possibilités :

- `F_SETLK` : Verrou non bloquant : si le fichier est déjà verrouillé, la fonction renvoie une erreur.
- `F_SETLKW` : Verrou bloquant : si le fichier est déjà verrouillé, le processus est mis en attente.

Pendant que le programme tourne, utilisez la commande suivante :

```bash
lsof /tmp/shared
```

> Quelles sont les informations intéressante que vous pouvez tirer de cette commande ?

Également pendant que le programme tourne essayez cette commande. Il se peut que vous deviez la lancer plusieurs fois rapidement pour tomber sur le bon moment (lorsque le fichier est verrouillé) :

```bash
cat /proc/locks
```

Que voyez-vous ?
