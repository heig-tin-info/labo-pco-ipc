# Signaux

Les signaux sont des interruptions *asynchrones* envoyées à un processus pour l'informer d'un événement. Les signaux peuvent être envoyés par le noyau, par un autre processus ou par le processus lui-même. L'API est standardisée d'une part par l'ISO/IEC 9899 pour le standard C, puis par POSIX.

Dans le standard C, l'implémentation est minimum et ne comporte que deux fonctions :

- `signal` : Associe une fonction de traitement à un signal
- `raise` : Emets un signal au processus courant

L'API POSIX est plus complète et met à disposition un ensemble de fonctions décrites ci-dessous. Cette API peut sembler lourde et un peu datée par rapport à des mécanismes plus modernes. Cette complexité vient principalement de l'héritage historique de UNIX à une époque ou les *thread* n'existaient pas.

| Fonction       | Description                                                |
| -------------- | ---------------------------------------------------------- |
| `kill`         | Envoie un signal à un processus                            |
| `killpg`       | Envoie un signal à un groupe de processus                  |
| `raise`        | Envoie un signal au processus courant                      |
| `sigaction`    | Modifie l'action associée à un signal                      |
| `sigprocmask`  | Modifie le masque de signaux du processus                  |
| `sigpending`   | Récupère la liste des signaux en attente                   |
| `sigwait`      | Attend la réception d'un signal                            |
| `sigaddset`    | Ajoute un signal à un ensemble                             |
| `sigdelset`    | Supprime un signal d'un ensemble                           |
| `sigemptyset`  | Initialise un ensemble de signaux                          |
| `sigfillset`   | Remplit un ensemble de signaux                             |
| `sighold`      | Bloque un signal                                           |
| `sigignore`    | Ignore un signal                                           |
| `siginterrupt` | Modifie le comportement d'un signal                        |
| `sigismember`  | Teste si un signal est dans un ensemble                    |
| `sigpause`     | Met en pause le processus jusqu'à la réception d'un signal |
| `sigqueue`     | Envoie un signal avec une valeur                           |
| `sigrelse`     | Débloque un signal                                         |
| `sigset`       | Associe une fonction de traitement à un signal             |
| `sigstack`     | Modifie la pile de signaux                                 |
| `sigsuspend`   | Met en pause le processus jusqu'à la réception d'un signal |
| `sigtimedwait` | Attend la réception d'un signal avec un timeout            |
| `sigwaitinfo`  | Attend la réception d'un signal avec des informations      |

Les signaux en tant que tel sont représentés par des entiers positifs et ils  sont divisés en deux catégories :

1. Les signaux standards : Ils sont définis par le standard POSIX et sont compris entre 1 et 31, chacun est associé à un événement particulier.
2. Les signaux temps réel : Ils sont définis par le standard POSIX et sont compris entre 32 et 64.

## 1. Documentation

Les manpages sont une source d'information précieuse pour comprendre le fonctionnement des signaux. Les manpages les plus utiles sont :

- `man 7 signal` : Décrit les signaux standards
- `man 2 signal` : Décrit les fonctions de gestion des signaux


Les signaux les plus connus sont (accessible depuis `man 7 signal`).

> Quel est la valeur de `SIGUSR1` ?
> À quoi sert `SIGSTP` ?
> Quel est le signal envoyé par défaut avec `Ctrl+C` ?
> Quel est le signal envoyé par défaut avec `Ctrl+Z` ?
> Quel est le signal envoyé par défaut avec `kill` ?

## 2. `signal` et `raise`

Le standard C définit `signal` et `raise` pour gérer les signaux. La fonction `signal` permet d'associer une fonction de traitement à un signal. La fonction `raise` permet d'envoyer un signal au processus courant.

Ce mécanisme est très simple et ne permet pas de gérer les signaux en attente. Il est donc déconseillé d'utiliser `signal` et `raise` dans un programme moderne. Néanmoins une utilisation assez courante est d'arrêter proprement un programme en déclanchent un signal `SIGTERM` depuis le programme lui-même.

En effet, il est probable que le programme soit lancé par un autre programme ou un utilisateur et qu'il soit nécessaire de lui permettre de s'arrêter proprement. Par exemple, un serveur web peut être arrêté proprement en envoyant un signal `SIGTERM`. Le serveur web peut alors fermer les connexions en cours, sauvegarder les données et fermer proprement les fichiers.

Rien n'empêche que ce serveur décide de s'arrêter lui-même après un certain temps d'inactivité.

Jetez un oeil à l'exemple [signal/raise.c](signal/raise.c) et testez le programme.

> Est-ce que cela fonctionne de la même manière si vous remplacez `SIGTERM` par `SIGKILL` ?
> Observez les appels systèmes générés pour les deux cas. Qu'observez-vous ?

## 3. Communication avec son enfant

Un cas de figure très élémentaire est l'envoi d'un signal à son enfant. Observez le code de [signal/signal.c](signal/signal.c) et testez le programme.

Il est courant de redéfinir le comportement d'un signal en utilisant la fonction `signal` ou `sigaction`. Par exemple, pour redéfinir le comportement du signal `SIGINT` et dire aurevoir avant de quitter :

## 4. Sigaction

L'appel système `sigaction` a été introduit pour remplacer `signal` qui pouvait avoir des comportements indéterminés. `sigaction` permet de définir un comportement plus précis pour un signal donné. Aujourd'hui l'utilisation de `signal` est automatiquement substituée par `sigaction`.

L'avantage de `sigaction` est de permettre de masquer certains signaux.
Le fonctionnement est
Notez qu'il est également possible d'accéder aux signaux en attente depuis `/proc`:

## 5. Status du processus

Depuis `/proc` il est possible de consulter les signaux en attente pour un processus donné. Par exemple, pour le processus `test` :

```text
$ cat /proc/$(pgrep test)/status | grep -P 'S(ig|hd)\w{3}'
ShdPnd: 0000000000000080  # Signaux en attente dans son ensemble
SigPnd: 0000000000000000  # Signaux en attente du thread
SigBlk: 0000000000000000  # Signaux bloqués (masqués)
SigIgn: 0000000000000000  # Signaux ignorés par le processus
SigCgt: 0000000000010002  # Signaux capturés par le processus
```

Vous trouverez dans `man 5 proc` la description des champs de `/proc/[pid]/status`.

> À quoi servent les champs `SigPnd`, `SigBlk`, `SigIgn` et `SigCgt` ?

Prenez connaissance du programme [test.c](test.c) et testez le programme. Durant l'exécution du programme, nous souhaitons envoyer un signal `SIGUSR2` et avant la fin de l'exécution, afficher le status dans `/proc`.

> Parvenez-vous à expliquer les valeurs affichées ?
