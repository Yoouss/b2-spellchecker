# Spellchecker - Projet de Langage C

## Informations du Groupe

| | |
|---|---|
| **Cours** | LEPL1503 - Projet 3 |
| **Groupe** | S1 |
| **Membres** | Younes, Sofia, Maryam, Adjovi, Maurice |

## Résumé du projet

Ce projet implémente un correcteur orthographique en langage C. Le programme charge plusieurs dictionnaires, analyse un fichier d'entrée texte et détecte les mots mal orthographiés. Il peut également proposer une correction pour chaque mot erroné en s'appuyant sur des critères de similarité et une distance de Levenshtein.

Le programme supporte deux modes :
- `detection` : identifie les mots incorrects et génère un fichier `.err`.
- `correction` : détecte les erreurs, propose des corrections et génère à la fois un fichier `.err` et un fichier `.fix`.

Il est possible de lancer le programme en mono-thread ou avec plusieurs threads grâce à l'option `--threads`.

## Architecture du code

Le code est organisé en modules distincts pour séparer la lecture/écriture, la détection, la correction et l'interface.

### Modules principaux

- `src/main.c`: Point d'entrée du programme. Gère le parsing des arguments, charge les dictionnaires et le fichier d'entrée, puis coordonne l'exécution selon le mode choisi.

- `src/io.c`: Gère l'écriture des résultats en format binaire (Big-Endian).

- `src/detector.c`: Implémente la détection des mots incorrects dans une ligne en utilisant une recherche binaire dans les dictionnaires. Sélectionne le meilleur dictionnaire pour chaque ligne.

- `src/corrector.c`: Propose des corrections pour les mots mal orthographiés en calculant la distance de Levenshtein et en trouvant les candidats les plus proches.

- `src/file_handler.c`: Charge les dictionnaires et le fichier d'entrée.

## Fonctionnalités implémentées

- Lecture d'un dossier de dictionnaires (`--dicts`).
- Lecture d'un fichier d'entrée texte (`--input`).
- Détection des mots incorrects ligne par ligne.
- Sélection automatique du meilleur dictionnaire pour chaque ligne.
- Écriture d'un fichier `.err` contenant : numéro de ligne, dictionnaire sélectionné, nombre d'erreurs, positions des mots.
- Écriture d'un fichier `.fix` contenant les corrections proposées dans le mode `correction`.
- Support du mode `verbose` pour un suivi détaillé de l'exécution.
- Option `--threads` pour paralléliser la recherche du meilleur dictionnaire et le calcul des corrections.

## Compilation et exécution

Le projet utilise un `Makefile` pour automatiser la construction des exécutables.

### Compilation

ette commande génère les fichiers binaires `spellchecker` et `test` :

```bash
make
```

Cette commande génère le fichier binaire `test` :

```bash
make test
```

### Exécution

```bash
./spellchecker --dicts <dictionary_path> --input <input_path> [--output <output_path>] [--mode <mode>] [--threads <number>] [--verbose]
```

- `--dicts <dictionary_path>` : spécifie le dossier dans lequel sont rangés les dictionnaires

L'utilisateur peut, si il a envie, spécifier qu'un seul dictionnaire du dossier (ex : `dicts/fr.dict)`) ou directement utiliser comme argument `all` pour spécifier tous les dictionnaires du dossier `dicts`

- `--input <input_path>` : spécifie le fichier d'entrée du programme (ex : `inputs/input_10l.txt`)

- `--output_path <output_path>` : spécifie le nom des fichiers de sortie sans extension (ex : `sortie`) 

S'il n'est pas spécifié, le terminal sera utilisé par défaut, l'utilisateur peut alors utiliser cette commande :

```bash
./spellchecker --dicts <dictionary_path> --input <input_path> [--mode <mode>] [--threads <number>] [--verbose] > <output_path>
```
pour avoir la sortie binaire sur le fichier `<output_path>` spécifié

ou

```bash
./spellchecker --dicts <dictionary_path> --input <input_path> [--mode <mode>] [--threads <number>] [--verbose] | hexdump
```

pour voir le résultat sur le terminal 

- `--mode <mode>` spécifie le mode de fonctionnement du programme

`--mode detection` : détecte les erreurs et les écrits sur `stdout` ou dans un nouveau fichier automatiquement créé `<output_path>.err` si `<output_path>` est spécifié

`--mode correction` : détecte les erreurs, les corrige et les écrits sur `stdout` ou dans de nouveaux fichiers automatiquement créés `<output_path>.err` pour la détection et `<output_path>.fix` pour la correction si `<output_path>` est spécifié

Si le mode n'est pas spécifié, le mode `detection` sera utilisé par défaut

- `--threads <number>` spécifie le nombre de threads à utiliser

Si `<number>` est inférieur ou égal à 1, le programme sera éxecuté de manière mono-threadé (séquentiel)

Si `<number>` est supérieur à 1, le programme sera éxecuté de manière multi-threadé avec `<number>` threads maximum par fonctions à parralèlisme configurable

- `--verbose` permet d'afficher plus de détails sur ce que fait le programme en temps réel (mode verbeux)

Lorsque des crochets sont présents dans une ligne de commande, comme `[--mode <mode>]` par exemple, le contenu entre ceux-ci est optionnel

Les chevrons, comme `<input_path>` indiquent l'endroit où écrire la valeur que prend l'argument. Les crochets, comme les chevrons, ne doivent pas être inclus dans la ligne de commande.

### Lancer Valgrind

Pour vérifier les fuites mémoire, utilisez les commandes suivantes :

- **Obverser les fuites mémoire du programme principal `spellchecker`**

```bash
make valgrind
```

- **Obverser les fuites mémoire du fichier binaire des tests `test`**

```bash
make valgrind-test
```

- **Obverser toutes les fuites mémoire (`spellchecker` et `test`)**

```bash
make valgrind-all
```

## Tests

Le projet inclut un dossier `tests/` pour les tests unitaires.

- `tests/detector.c` : vérifie la détection et la sélection du dictionnaire.
- `tests/corrector.c` : vérifie les algorithmes de correction et la distance de Levenshtein.
- `tests/` contient aussi des outils de génération et de validation de données.

### Commandes de test

```bash
./test
```

```bash
make tests
```

## Benchmarks

Le dossier `benchmark/` contient des dictionnaires et des entrées de test pour mesurer les performances.

### Commandes de benchmark

- **Obverser les benchmarks du mode detection**

```bash
make benchmark-detection
```

- **Obverser les benchmarks du mode correction**

```bash
make benchmark-correction
```

Les résultats seront écris dans le fichier `benchmark.csv`

Nous utilisons (dans le `dossier benchmark`) : 

- Un fichier de 20 000 lignes pour la détection
- Un fichier de 250 lignes pour la correction
- 6 dictionnaires

## Améliorations possibles

- Ajouter des tests unitaires supplémentaires pour tous les cas limites.
- On a remarqué que la correction ralentit fortement avec plusieurs dictionnaires ; il serait donc intéressant de paralléliser davantage le module `src/corrector.c`

## Conclusion
Ce projet a été une vraie prise de conscience pour nous. On utilise tous des correcteurs orthographiques tous les jours sur nos téléphones ou nos ordinateurs sans trop se poser de questions, mais passer "de l'autre côté" nous a forcés à voir la complexité que cela cache. Ce travail nous a surtout permis de mieux comprendre ce qui se passe réellement derrière la machine.