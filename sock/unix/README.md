# Socket UNIX

La dernière famille de sockets que nous allons voir est la famille `AF_UNIX` ou `AF_LOCAL`. Ces sockets sont utilisés pour la communication entre processus sur la même machine. Ils sont très utiles pour la communication entre processus parent et enfant ou entre processus de même utilisateur.

On les utilises très fréquemment pour :

1. Communication avec une base de données (MySQL utilise `/var/run/mysqld/mysqld.sock`).
2. Communication avec Docker (Docker utilise `/var/run/docker.sock`).
3. Communication avec Syslog (`/dev/log`).
4. Communication avec X11 (`/tmp/.X11-unix/X0`) comme xeyes ou xclock.

Au lieu de se reposer sur une adresse IP et un port, les sockets UNIX utilisent un fichier pour communiquer.

## 1. Compilation et test

Compilez le programme avec `make`. Lancez le serveur avec `./server` et le client avec `./client`.

> Parvenez-vous à envoyer un message du client au serveur ?
> Comprenez-vous comment fonctionne le code ?
