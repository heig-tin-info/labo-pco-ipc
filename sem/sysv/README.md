# Sémaphores System V

On vous donne trois programmes qui dépendent d'un secret commun, la clé `1234` contenu dans `key.h`.

1. `verhogen.c` qui incrémente le sémaphore.
2. `proberen.c` qui attend qu'une ressource soit disponible sur le sémaphore 1234, puis décrémente le sémaphore et affiche un message.
3. `delete.c` qui supprime le sémaphore.

Ces mots sont empruntés au néerlandais, et signifient respectivement *augmenter* et *essayer*.

## Travail pratique

### 1. Compilation

Pour compiler les exemples, utilisez simplement `make`.

- `make clean` nettoie les fichiers générés.
- `make all` compile les programmes.

### 2. Visualiser l'état des sémaphores

Commencez par voir qu'aucun sémaphore n'existe dans le système. D'abord avec l'outil `ipcs` (*InterProcess Communication Status*) :

```bash
ipcs -s
```

Puis, à la source, directement du *kernel* :

```bash
cat /proc/sysvipc/sem
```

Notez que `ipcs` utilise les mêmes informations que `/proc/sysvipc/sem`, la preuve avec `strace`:

```bash
$ strace ipcs -s 2>&1 | grep proc
openat(AT_FDCWD, "/proc/sysvipc/sem", O_RDONLY) = 3
```

### 3. Incrémenter et décrémenter

Commencez par incrémenter le sémaphore avec `verhogen`. À chaque exécution vous verrez entre parenthèses la valeur du compteur qui s'incrémente.

```bash
./verhogen
```

Testez de décrémenter le sémaphore avec `proberen` jusqu'à ce que le programme se bloque.

```bash
./proberen
```

Pour plus d'interactivité, ouvrez deux terminaux en tête à tête par exemple avec `tmux` (qui peut être installé avec `sudo apt install tmux`).

```bash
tmux -f /dev/null new-session \; split-window -h
```

Pour naviguer des fenêtres (gauche ou droite) utilisez le raccourci CTRL+b puis flèche gauche ou droite.

### 4. Vérifier le statut

Maintenant qu'il est utilisé, vérifiez que le sémaphore a bien été créé avec `ipcs` et `cat /proc/sysvipc/sem`.

> À quoi servent les permissions et que veut dire 666 ?

### 5. Gestion de lots

Dans un problème typique de [producteur-consommateur](https://fr.wikipedia.org/wiki/Probl%C3%A8me_des_producteurs_et_des_consommateurs), les producteurs de ressources peuvent produire des lots plus petits que n'en ont besoin les consommateurs, et réciproquement.

L'idée est donc de modifier les programmes `verhogen` et `proberen` pour qu'ils puissent incrémenter ou décrémenter de plusieurs unités à la fois le sémaphore. La valeur. On appellera donc ces programmes avec un argument qui sera la valeur à incrémenter ou décrémenter.

```bash
./verhogen 3 # Incrémente de 3
./proberen 2 # Décrémente de 2
```

1. Modifier les deux programmes
2. Tester les programmes avec des valeurs différentes pour observer qu'il faut appeler `verhogen` plusieurs fois pour que `proberen` se débloque selon la valeur passée en argument.

### 6. Concurrence simple

Que se passerait-il si nous avons plusieurs consommateurs ? Imaginons la stratégie suivante :

1. `./proberen 1` est exécuté 10 fois, mais en arrière-plan.
2. L'outil `htop` est utilisé pour observer ces processus.
3. En parallèle dans un autre terminal, produisez des ressources et observez l'ordre de déblocage des consommateurs.

Vous pouvez facilement instancier plusieurs consommateurs en utilisant un script bash :

```bash
for i in {1..10}; do ./verhogen & done
```

Dans `htop` vous pouvez facilement voir les processus en arrière-plan avec la touche `F4` pour filtrer les processus, désactivez le mode `tree` avec la touche `F5`.

Vous pouvez utiliser `tmux` pour partager l'écran et voir les deux terminaux en même temps.

> Notez l'ordre dans lequel les consommateurs sont débloqués. Est-ce que cela correspond à l'ordre dans lequel ils ont été lancés ?

### 7. Concurrence complexe

L'objectif est de lancer plusieurs consommateurs, mais qui n'ont pas besoin de la même quantité de ressources.

```bash
./proberen 3 &
./proberen 2 &
./proberen 1 &
```

Produisez ensuite des ressources avec `verhogen` et observez l'ordre de déblocage des consommateurs.

> Qu'observez-vous ?

### 8. Nettoyage

N'oubliez pas de nettoyer votre sémaphore à la fin du travail avec `delete`.

```bash
./delete
```

> Que se passe-t-il si vous essayez de supprimer un sémaphore qui est utilisé par un processus ?

## Explications

Comme expliqué, les sources de ce mécanisme de sémaphore sont disponibles dans le noyau Linux : [ipc/sem.c](https://github.com/torvalds/linux/blob/master/ipc/sem.c). Il est donc possible à n'importe qui de comprendre comment les sémaphores fonctionnent, néanmoins le code est complexe et il est difficile de le comprendre sans une bonne connaissance du noyau Linux. Voici donc quelques explications.

### Incrémentation (`sem_op = +1`)

Lorsqu’un processus exécute :

```c
semop(semid, &(struct sembuf){.sem_num = 0, .sem_op = 1}, 1);
```

le noyau suit les étapes suivantes :

1. Appel système `semop()` qui appelle `do_semtimedop()`.

   ```c
   SYSCALL_DEFINE3(semop, int, semid, struct sembuf __user *, tsops, unsigned,  nsops)
   ```

2. Les informations de l'espace utilisateurs sont copiées dans l'espace noyau. C'est le rôle de `do_semtimedop()`.

3. Validation et verrouillage : le noyau récupère ensuite la structure du sémaphore et il vérifique `semid` est valide, que le nombre d’opérations `nsops` est raisonnable, que `sem_num` est dans les limites et que les permissions sont correctes (`ipcperms()`).

4. Tentative d’incrémentation : la fonction `perform_atomic_semop()` est appelée pour essayer d’incrémenter immédiatement la valeur du sémaphore :

   ```c
   curr->semval += sem_op;
   ```

   Si aucun processus n’attend ce sémaphore, c’est terminé !

5. Réveil des processus bloqués : si un ou plusieurs processus étaient bloqués en attente (`sem_op = -1`), ils sont réveillés par :

   ```c
   do_smart_update(sma, sops, nsops, 1, &wake_q);
   wake_up_q(&wake_q);
   ```

   Le premier processus dans la file d’attente peut reprendre son exécution puis les autres restent bloqués si la valeur du sémaphore n'est pas suffisante.

### Décrémentation (`sem_op = -1`)

Quand un processus DÉCRÉMENTE un sémaphore (`sem_op = -1`) avec par exemple :

```c
semop(semid, &(struct sembuf){.sem_num = 0, .sem_op = -1}, 1);
```

les opérations suivantes sont effectuées :

1. L'appel système est déclenché et `do_semtimedop()` est appelé.

2. Tentative de décrémentation : la fonction `perform_atomic_semop()` tente de soustraire `1` à `semval` :

   ```c
   result += sem_op; // semval - 1
   ```

   Deux cas possibles. Soit la valeur du sémaphore est $>0$ alors la décrémentation est réussie. Soit la valeur est $0$ et le processus est bloqué.

3. Mise en sommeil si nécessaire : si `semval == 0`, le processus est placé dans la file d’attente avec `__set_current_state(TASK_INTERRUPTIBLE)`. Le processus est donc suspendu et ne consomme plus de CPU. Il attend qu’un autre processus incrémente le sémaphore (`sem_op = +1`).

4. Réveil quand un `sem_op = +1` est exécuté dans ce cas le processus peut alors reprendre son exécution après l'appel de :

   ```c
   wake_up_q(&wake_q);
   ```
