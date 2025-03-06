# Files de messages POSIX

Vous avez déjà vu les files de messages System V, mais il existe aussi des files de messages POSIX. Elles sont plus récentes et plus simples à utiliser que les files de messages System V.

L'avantage principal est de pouvoir supporter des files prioritaires, ce qui n'est pas possible avec les files de messages System V. C'est à dire que l'envoi peut être associé à une priorité.

## 1. Exemple

Prenez connaissance de l'exemple `send` et `receive` pour voir comment ils fonctionnent.

> Quels sont les appels systèmes utilisés ?
> Ou est stockée la file de message `/dev?` et quel est son format ?

## 2. Gestion des priorités

Modifiez le programme pour gérer les priorités et démontrez que plusieurs messages envoyés avec des priorités différentes sont reçus dans l'ordre.
