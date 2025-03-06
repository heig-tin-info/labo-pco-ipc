# Les Tubes (Pipes)

## 1. Les tubes par défaut

Vous le savez déjà, mais chaque processus dispose de base de 3 flux de données :

- `stdin` : Entrée standard
- `stdout` : Sortie standard
- `stderr` : Sortie d'erreur

Ces flux sont unidirectionnels, c'est-à-dire que `stdin` ne peut être utilisé que pour lire, `stdout` et `stderr` ne peuvent être utilisés que pour écrire.

1. Réalisez un programme de base qui lit un caractère sur l'entrée standard et l'écrit sur la sortie standard.
2. Affichez les appels systèmes du programme.
3. Notez les descripteurs de fichiers ouverts dans `/proc/<pid>/fd/`.

> Que constatez-vous ?
> Identifiez-vous les numéros des descripteurs de fichiers ouverts ?

## 2. Les tubes nommés en ligne de commande

Les tubes nommés sont des fichiers spéciaux qui permettent de communiquer entre deux processus. Ils sont créés avec la commande `mkfifo`.

On commence par créer un tube nommé :

```bash
mkfifo /tmp/mytube
```

Notez le type du fichier créé :

```bash
ls -l /tmp/mytube
```

> Que signifie le `p` en début de ligne ?

Avec le caractère `>` on peut rediriger la sortie standard d'une commande vers un fichier. On peut aussi rediriger la sortie standard d'une commande vers un tube nommé. Essayez :

```bash
echo "Hello" > /tmp/file
cat file

echo "Hello" > /tmp/mytube
cat /tmp/mytube
```

On peut également lire depuis un tube nommé ou un fichier

```bash
cat < /tmp/file

cat < /tmp/mytube
```

> Que constatez-vous ?

Pour supprimer un tube, il suffit d'utiliser `rm` comme tout fichier:

```bash
rm /tmp/mytube
```

## 3. Les tubes anonymes en ligne de commande

Un tube n'a pas à être nommé. Il ne peut exister que durant la vie des processus qui l'utilisent. Bash utilise la syntaxe `|` pour créer un tube anonyme.

```bash
echo "Hello" | cat
```

Pour mieux décortiquer ce qui se passe, on peut utiliser `strace` :

```bash
strace sh -c 'echo "Hello" | cat'
```

> Que se passe-t-il ?