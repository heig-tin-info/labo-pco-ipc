# Socket TCP

Connaissez-vous le fameux **TCP/IP** ? C'est le protocole de communication le plus utilisé sur Internet. Il est basé sur deux protocoles : **TCP** (Transmission Control Protocol) et **IP** (Internet Protocol).

Il est défini par la [RFC 793](https://tools.ietf.org/html/rfc793) établie en septembre 1981. C'est un protocole fiable, c'est-à-dire que les données envoyées sont garanties d'arriver à destination. Cela est possible grâce à un système de **retransmission** des données en cas de perte.

## 1. Compilation et test

Compilez le programme avec `make`. Lancez le serveur avec `./server` et le client avec `./client`. Essayez de lancer plusieurs fois le client.

> Que constatez-vous ?

Dans le code du serveur vous pouvez noter les étapes suivantes:

1. Création du *socket* en mode TCP (*SOCK_STREAM*).
2. Liaison du *socket* à une adresse et un port (*bind*) avec `INADDR_ANY` pour accepter les connexions de n'importe quelle adresse (*0.0.0.0*).
3. Mise en écoute (*listen*) du *socket* pour accepter les connexions entrantes.
4. Attente d'une connexion (*accept*).
5. Création d'un nouveau *socket* pour communiquer avec le client.
6. Réception et envoi de données.
7. Fermeture des *sockets*.

Contrairement à UDP, le protocol TCP est dit **connecté**. Une fois l'acceptation d'une connexion, un autre *socket* est créé pour communiquer avec le client. Cela permet de gérer plusieurs connexions simultanément.

> Comprenez-vous pourquoi les numéros de ports changent à chaque fois ?

## 2. D'autre clients ?

Pendant que vous avez un client connecté, tentez de lancer un autre client.

> Que se passe-t-il ?
> Comprenez-vous pourquoi ?

## 3. Multi clients

Pour gérer plusieurs clients, une approche courante et de créer un nouveau processus pour chaque client. Cela permet de gérer les clients de manière indépendante.

Prenez connaissance de `server2.c`, compiler le programme et testez 3 clients simultanément.

> Cela fonctionne-t-il ?
> Voyez-vous les différents processus dans `htop` ?
