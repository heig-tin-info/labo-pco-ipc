# Sockets UDP

Les sockets UDP sont des canaux de communication non fiables. Cela signifie que les données envoyées ne sont pas garanties d'arriver à destination. Cependant, elles sont envoyées rapidement et sans établir de connexion.

Elles sont beaucoup utilisées sur Internet pour les flux vidéos (streaming) ou les jeux en ligne. Parfois il n'est pas important si des des données sont perdues, l'important est qu'elles arrivent rapidement.

Ces sockets sont relativement simples à utiliser, c'est pourquoi c'est le premier exemple que nous allons voir.

## 1. RFC 768

Rendez-vous sur la page [RFC 768](https://tools.ietf.org/html/rfc768). Il s'agit de la documentation officielle du protocole UDP établi le 28 août 1980.

Au niveau du format on observe que :

1. Les données sont encapsulées dans un en-tête.
2. Cet en-tête comporte un port source et un port destination.
3. Il y a un champ checksum pour vérifier l'intégrité des données.
4. Il y a un champ longueur pour connaître la taille des données.

Le port est un numéro qui peut être compris entre 0 et 65535. Les ports de 0 à 1023 sont réservés pour les services connus (comme le port 80 pour le protocole HTTP). On peut imaginer un immeuble avec beaucoup de portes, chaque porte est un port. Lorsqu'un paquet arrive, il est dirigé vers la bonne porte.

Un port peut être ouvert (listening) ou fermé (closed). Lorsqu'un programme écoute sur un port, il est en attente de données. Lorsqu'un programme envoie des données, il envoie sur un port.

## 2. Client Serveur

Compilez les programmes avec `make`. Lancez le serveur avec `./server` et le client avec `./client`.

Observez ce qui se passe à chaque fois que vous lancez le client. Vous devriez voir des messages s'afficher sur le serveur.

## 3. État des sockets

Utilisez la commande `ss -lup` pour voir l'état des sockets UDP lorsque le serveur tourne ?

> Voyez-vous le socket en état `UNCONN` écoutant sur le port 3636 ?
> Voyez-vous à quel processus il appartient ?

Note que la commande `lsof -i UDP` permet aussi de voir les sockets UDP.

Essayez de voir les fichiers ouverts:

```bash
ls -l /proc/$(pgrep server)/fd
```

> Voyez-vous le fichier de socket (`socket:[<numero>]`) ?

Relevez le numéro de socket et cherchez le dans `cat /proc/net/udp`. Vous devriez voir une ligne correspondant à votre socket.

> À quoi correspond le `:0E34` ?

## 4. Sécurité

Lancez votre serveur et envoyez quelques messages pour vérifier que tout fonctionne.

On va configurer le firewall pour bloquer les paquets UDP sur le port 3636. Utilisez la commande suivante :

```bash
sudo iptables -A INPUT -s 127.0.0.1 -p udp --dport 3636 -j DROP
```

Vous pourriez avoir un mesasge d'erreur sur WSL mais cela ne devrait pas poser de problème. Vérifiez que vous avez bien bloqué les paquets UDP :

```bash
sudo iptables -L
```

> Maintenant ressayez d'envoyer des messages. Que se passe-t-il ?

Pour supprimer la règle de firewall utilisez la commande suivante :

```bash
sudo iptables -D INPUT -s 127.0.0.1 -p udp --dport 3636 -j DROP
```

## 5. `nc`

Depuis la ligne de commande vous pouvez également utiliser `nc` pour envoyer des messages. Essayez la commande suivante :

```bash
echo "Hello from nc" | nc -u -w2 127.0.0.1 3636
```

> Que se passe-t-il sur le serveur ?
