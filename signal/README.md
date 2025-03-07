# Signaux

Les signaux sont des interruptions *asynchrones* envoyées à un processus pour l'informer d'un événement. Les signaux peuvent être envoyés par le noyau, par un autre processus ou par le processus lui-même. L'API est standardisée d'une part par l'ISO/IEC 9899 pour le standard C, puis par POSIX.

Dans le standard C, l'implémentation est minimum et ne comporte que deux fonctions :

- `signal` : Associe une fonction de traitement à un signal
- `raise` : Emets un signal au processus courant

L'API POSIX est plus bien plus complète et met à disposition un ensemble de fonctions décrites ci-dessous. Cette API peut sembler lourde et un peu datée par rapport à des mécanismes plus modernes. Cette complexité vient principalement de l'héritage historique de UNIX à une époque ou les *thread* n'existaient pas.

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

## 2. Programmation d'une alarme (`alarm`)

La fonction `alarm` permet de programmer une alarme qui enverra un signal `SIGALRM` au processus courant après un certain temps.

Prenez connaissance du programme [signal/alarm.c](signal/alarm.c) et testez le programme.

> Si j'envoi un signal au programme pendant l'attente, que ce passe-t-il ?
> Comment puis-je annuler l'alarme ?

## 3. Interception d'un signal (`signal`)

La fonction `signal` permet de définir une fonction de traitement pour un signal donné. Cette fonction sera appelée lorsque le signal est reçu.

Reprenons notre programme [signal/alarm.c](signal/alarm.c) et ajoutons un traitement pour le signal `SIGALRM` pour afficher le message `Alarm went off!`.

Voir l'aide de `signal(2)` ou `signal(3)` pour plus d'informations.

## 4. Levée d'un signal au processus courant (`raise`)

Jetez un oeil à l'exemple [signal/raise.c](signal/raise.c) et testez le programme.

> Est-ce que cela fonctionne de la même manière si vous remplacez `SIGTERM` par `SIGKILL` ?
> Observez les appels systèmes générés (`strace`) pour les deux cas. Qu'observez-vous ?

Notez que `raise` ne lève un signal que pour le processus courant, les enfants ne sont pas affectés.

## 5. Communication avec son enfant

Un cas de figure très élémentaire est l'envoi d'un signal à son enfant. Observez le code de [signal/signal-child.c](signal/signal-child.c) et testez le programme.

> Parvenez-vous à expliquer le comportement du programme ?
> Que se passe-t-il si vous remplacez `SIGUSR1` par `SIGKILL` ?

## 6. Ne se réveille qu'à la réception d'un signal spécifique (`sigwait`)

La fonction `sigwait` permet de mettre en pause le processus courant jusqu'à la réception d'un signal spécifique. Cette fonction est très utile pour synchroniser des processus.

En revanche, par défaut, les signaux envoyés à un processus provoquent l'arrêt de ce dernier. Essayez par exemple de lancer une attente puis l'envoi d'un signal au processus :

```bash
sleep 60 &
kill -SIGUSR1 $(pgrep sleep)
```

Le processus `sleep` est arrêté. Pour éviter cela, il est nécessaire de masquer le signal pour désactiver son comportement par défaut. Pour cela, il est possible d'utiliser `sigprocmask`.

Prenez connaissance du programme [signal/sigwait.c](signal/sigwait.c) et testez le programme.

Pour envoyer un signal au programme (que vous avez compilé avec `gcc -osigwait sigwait.c`) utilisez la commande suivante :

```bash
kill -s <signal> $(pgrep sigwait)
```

> Pourquoi débloquer le signal `SIGUSR1` avant l'appel de `sigwait` ?
> Que ce passe-t-il si un autre signal non bloqué comme `SIGALRM` est envoyé au processus ?

## 7. Sigaction

L'appel système `sigaction` a été introduit pour remplacer `signal` qui pouvait avoir des comportements indéterminés. De nos jours, il n'y a plus aucune raison d'utiliser `signal` pour les raisons suivantes :

1. La fonction `signal()` ne bloque pas nécessairement les autres signaux lors de l'exécution du gestionnaire de signal.
2. La fonction `signal()` réinitialise le gestionnaire de signal à la valeur par défaut après l'exécution du gestionnaire de signal.
3. Le fonctionnement de `signal()` est dépendant de l'implémentation.

Par exemple, selon le cas, un second signal qui arriverait peut interrompre l'exécution du premier gestionnaire de signal et compromettre la cohérence de l'application. Cela causerait un empilement des signaux.

Même si `signal` fonctionne très bien sous Linux parce qu'il est basé sur `sigaction`, ce n'est pas une manière portable de gérer les signaux car sur un autre système POSIX, le comportement peut être différent.

Prenez connaissance du programme [signal/sigaction.c](signal/sigaction.c) et testez le programme. Vous pouvez tenter d'interrompre le programme avec `Ctrl+C` pour observer le comportement.

## 8. Status du processus

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

> À quoi servent les champs `ShdPnd`, `SigBlk`, `SigIgn` et `SigCgt` ?

1. Prenez connaissance du programme [test.c](test.c) et testez le programme.
2. Durant l'exécution du programme, nous souhaitons envoyer un signal `SIGUSR2` et avant la fin de l'exécution
3. Le status (`/proc`) doit être affiché après que le signal ait été envoyé.

> Parvenez-vous à expliquer les valeurs affichées et la correspondance avec les numéros de signaux ?
