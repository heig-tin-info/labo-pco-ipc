# Files de messages System V

Les files de messages System V ont été introduites dans Unix System V, publié en 1983. Elles permettent la communication entre processus (IPC) via des files de messages persistantes en mémoire.

Il y a 4 fonctions de base :

1. `msgget` : Crée une nouvelle file de messages ou récupère une file de messages existante.
2. `msgsnd` : Envoie un message à une file de messages.
3. `msgrcv` : Récupère un message de la file de messages.
4. `msgctl` : Contrôle la file de messages.

Naturellement, comme tous les autres mécanismes IPC de System V vous pouvez utilisez l'outil `ipcs -q`.

## 1. Compiler les programmes

```bash
make
```

## 2. Envoi et réception

Jouez un peu avec les programmes `send` et `receive` pour voir comment ils fonctionnent.

> Que fait received si la file de messages est vide ?
> Dans quel ordre sont recu les messages si plusieurs sont envoyés avec lecture ?
> Est-ce que la file de message a une taille maximum ?

## 3. Nettoyer

N'oubliez pas de fermer les files de messages après utilisation.

```bash
./delete
```

Alternativement vous pouvez utiliser `ipcrm` pour supprimer les files de messages.

```bash
ipcrm -Q 1234
```
