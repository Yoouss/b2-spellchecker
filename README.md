# <p align="center"> LEPL1503 - Projet 3</p>
## <p align="center">Spellchecker</p>
### <p align="center">Matthieu Baerts — Olivier Bonaventure — Benoît Legat <p>
### <p align="center">Alexandre Orékhoff</p>
#### <p align="center"> Année académique 2025 - 2026</p>


<div align="center">
  <div style="display:grid; grid-template-columns: repeat(2, auto); gap: 50px">
    <a href="https://uclouvain.be/en-cours-2025-lepl1503">
      <img alt="Fiche du cours" src="https://upload.wikimedia.org/wikipedia/commons/7/72/UCLouvain_logo.svg" height="32"/>
    </a>
    <a href="https://sites.uclouvain.be/SystInfo/notes/Theorie/html/index.html">
      <img alt="Syllabus" src="https://upload.wikimedia.org/wikipedia/commons/f/f0/Paperback-stack.png" height="48">
    </a>
  </div>
  <img src="https://forge.uclouvain.be/lepl15031/students/project-matrix/groupe-a/projet-matrix/badges/main/pipeline.svg"/>
</div>

---

Un correcteur orthographique est probablement un des programmes informatiques que vous utilisez le plus au quotidien. Que ce soit pour envoyer un message à vos ami(e)s, un mail au professeur pour demander une correction de projet, ou encore pour rédiger votre rapport en $\LaTeX$, le correcteur orthographique est omniprésent dans nos vies. 

Au travers de ce projet, vous appliquerez les concepts du langage de programmation C dont l'apprentissage a déjà commencé durant les premières semaines du quadrimestre. Vous serez libre d'explorer les différentes techniques qui existent afin de mettre en place un correcteur orthographique performant et efficace. Enfin, un classement sera mis-à-jour chaque semaine avec les performances de chaque groupe dans l'identification des mots incorrects afin de vous permettre de vous mesurer aux autres !

Le projet visera à corriger les fautes d'ortographes uniquement. Les fautes de grammaire, de conjugaison, de syntaxe ou de sémantique ne feront pas partie du sujet étudié.

Le projet se déroulera en 2 phase. La première partie durera jusqu'en **S8** et consistera en une implémentation séquentielle à soumettre en ligne. Vous analyserez ensuite les programmes de vos camarades et leur soumettrez des commentaires sur les points à améliorer. Ce processus, incontournable dans l'industrie, est nommé *peer-review*.

La seconde phase sera la parallélisation de votre programme. Celle-ci se déroulera de la **S8** à la **date de soumission finale** de votre programme.

# A. La Mission

Au fil des semaines, vous implémenterez les différents aspects du correcteur orthographique. Voici, dans l'ordre, les tâches qui vous seront demandées: 
- En S5, basé sur un petit dictionnaire donné, vous détecterez les mots incorrects
- En S6, vous continuerez sur votre lancée et proposerez une correction pour les mots que vous avez détectés comme erronés
- En S7, vous étendrez le travail de la semaine précédente pour être capable de détecter le meilleur dictionnaire parmi un jeu de dictionnaires donné
- En S8, vous implémenterez vous-même la lecture des dictionnaires ainsi que la lecture/écriture des fichiers d'entrée et de sortie, et vous réfléchirez, en groupe, à une structure multithreadée que vous proposerez à votre tuteur
- En S9 et en S10, vous validerez et implémenterez la structure multithreadée de votre projet
- Les semaines suivantes vous serviront à peaufiner et optimiser votre programme.

#### 1. Détection
Le sujet des correcteurs orthographique a été très actif  dès les premières années de l'informatique moderne. Aujourd'hui englobé dans le traitement du langage naturel ([Natural Language Processing (NLP)](https://en.wikipedia.org/wiki/Natural_language_processing)), il reste encore un sujet majeur.
De nombreux algorithmes ont été développés afin de réaliser cette tâche. Les implémentations les plus judicieuses combinent plusieurs de ces techniques afin d'obtenir le résultat voulu en un temps optimal. Vous pouvez commencer par vous renseigner sur la notion de [distance entre 2 mots](https://en.wikipedia.org/wiki/String_metric)...

Durant cette semaine, les arguments `--dicts <dict_path>` et `--input <input_path>` n'ont pas besoin d'être utilisés. Par défaut, les fonctions `load_dictionaries` et `read_input_file` du fichier io.h chargent des fichiers prédéfinis.

Consacrez-vous à la détection des mots erronés dans chaque ligne. Les positions des mots à détecter pour chaque ligne se trouvent dans le fichier `input.pos`.

<!-- Les fonctions de lecture et d'écriture (I/O) ne vous étant pas encore connues, vous pouvez utiliser la librairie `io.o` fournie. Celle-ci vous donne toutes les fonctions nécessaires à la réalisation de votre tâche.-->

Lisez le fichier en-tête `io.h` pour comprendre le fonctionnement et l'utilisation de la librairie 😉.  

> [!TIP]
> **Problème d'affichage dans le terminal ?**
> Vos terminaux modernes utilisent par défaut l'encodage UTF-8. Puisque le projet (et donc la sortie de votre programme) utilise le Latin-1, les accents s'afficheront sous forme de symboles étranges `?`. 
> Pour traduire la sortie de votre programme en UTF-8 à la volée et retrouver un affichage lisible, vous pouvez utiliser un "pipe" vers l'outil `iconv` :
> ```sh
> ./spellchecker --dicts <dict_path> --input <input_path> | iconv -f ISO-8859-1
> ```

<!-- > [!TIP]
> Si vous lisez attentivement ce fameux fichier en-tête `io.h`, vous remarquerez la présence des fonctions `pretty_print_detection` et `pretty_print_correction`. Ces fonctions, déjà implémentées, vous permettent de visualiser le résultat de votre programme dans le terminal avec de jolies couleurs. -->

#### 2. Correction

Maintenant que vous êtes capable d'identifier les mots incorrects, proposez une correction pour ceux-ci. N'hésitez pas à vous renseigner sur les algorithmes existants ainsi qu'à vous inspirer de ceux-ci.

#### 3. Meilleur dictionnaire
Chaque dictionnaire correspond à une langue unique. Une ligne du fichier d’entrée est écrite entièrement dans une seule langue, mais la langue peut changer d’une ligne à l’autre.

Le meilleur dictionnaire pour une ligne donnée est donc le dictionnaire correspondant à la langue dans laquelle cette ligne précise est écrite, c’est-à-dire celui qui minimise le nombre de mots incorrects pour cette ligne.

En utilisant votre implémentation du calcul du nombre de mots incorrects, vous calculerez le dictionnaire qui correspond le mieux à l'entrée saisie parmi une sélection de dictionnaires donnée. 

L'argument `--dicts <dicts_path>` permet de spécifier le dossier contenant tous les dictionnaires. Pour vous simplifier la tâche la semaine passée, si cet argument n'était pas spécifié, le programme allait chercher le dictionnaire wallon par défaut. Cette semaine vous allez devoir implémenter cet argument et permettre de lire tous les dictionnaires du dossier spécifié afin de trouver le meilleur. 

Le chemin des dictionnaires vous étant donnés par argument, il ne vous reste plus qu'à lire les dictionnaires en utilisant les fonctions fournies dans la librairie `io.o`.

#### 4. Lecture/Écriture (I/O)
Jusqu'en S8, un fichier en-tête (`.h`) vous est fournis afin de vous permettre de lire les dictionnaires sans devoir implémenter le code vous-même. A partir de la S8, les connaissances vous étant acquises, vous devrez vous passer de ce fichier et implémenter vous-même le code de lecture/écriture. Attention, vous n'avez pas le droit d'utiliser les fonctions faisant appel aux *streams* (utilisant `FILE`). Vous avez donc accès aux fonctions classiques vues en TP comme `open`, `close`, `mmap`, `write`, ... Les fonctions comme `fopen`, `fclose` ou encore `fwrite` sont interdites.

#### 5. Threads
La première partie du projet se déroule jusqu'en S8 et requiert une implémentation séquentielle fonctionnelle à l'issue de celle-ci. En S8, vous aurez vu les bénéfices importants que peut apporter la parallélisation du travail. Il existe plusieurs structures et organisations possibles, la plus connue étant le duo [Producteur/Consommateur](https://sites.uclouvain.be/SystInfo/notes/Theorie/html/Threads/coordination.html#probleme-des-producteurs-consommateurs). Nous vous demandons de proposer une implémentation parallélisée du code que vous avez soumis en fin de S8. 

Pensez à réfléchir aux principaux [bottlenecks](https://en.wikipedia.org/wiki/Bottleneck_(software)) de votre programme avant de vous lancer.

> [!TIP]
> Les formats des fichiers de sortie décrits dans la section [Fichiers de sortie](#fichiers-de-sortie) sont adaptés à une exécution parallèle. L'ordre des lignes dans la sortie peut ainsi différer de celui de l'entrée. 

> [!Warning]
> L'implémentation parallèle devra être effectuée à l'aide de la librairie [`pthread`](https://man7.org/linux/man-pages/man7/pthreads.7.html) impérativement. Toute autre librairie est formellement interdite.

# B. Organisation du projet
### README
Un fichier `README.md` est un fichier au format *Markdown*, comme celui-ci, qui explique l'utilisation et le fonctionnement d'un programme. Comme nous ne travaillerons pas sur votre projet, nous vous demandons de nous expliquer en quelques lignes comme celui-ci fonctionne et quelles sont les fonctionnalités que vous avez développées. Ces explications figureront dans un fichier `REPORT.md`.

### Makefile
Un `Makefile` est un fichier permettant de simplifier la compilation de programme. Un point de départ vous est fourni, à vous de le compléter. Le code doit être au minimum compilé avec les flags `-Wall` et `-Werror` de `clang`. Le fichier `Makefile` doit permettre au moins les 3 commandes suivantes:
- `make`: compile le programme et produit l'exécutable nommé `spellchecker` à la racine du répertoire
- `make tests`: compile le programme et lance les tests que vous avez écrits
- `make clean`: nettoie le projet en supprimant l'exécutable et les fichiers objets produits lors de la compilation

### Test Unitaire
Lors de l'écriture d'un programme informatique, il est nécessaire de s'assurer de la validité de celui-ci. A cet effet, les développeurs ont imaginés le concept de **Test Unitaire**. Un test unitaire permet de vérifier individuellement une petite partie du programme afin de s'assurer qu'elle réalise correctement la tâche qui lui est assignée. En vérifiant petit morceau par petit morceau, on peut rapidement détecter la partie fautive du programme lorsque tout le système plante. 

### CUnit
Afin d'être le plus efficace possible, ces tests doivent être écrit au plus vite, idéalement dès l'écriture du code à tester. C'est pourquoi nous vous demandons d'utiliser le framework [`CUnit`](https://sites.uclouvain.be/SyllabusC/notes/Outils/cunit.html) pour tester votre programme au fur et à mesure de votre avancement (chaque semaine). L'intérêt majeur est de vous permettre de faire des modifications dans votre code tout en vous assurant que vous ne cassez rien de ce qui a déjà été fait et ne produisez pas de bug supplémentaire. 

# C. Spécifications techniques

### Arborescence du projet
Afin d'aider à la correction, vous devez impérativement suivre l'arborescence de fichier donnée dans le squelette du projet:

- `dicts` : dossier où sont stockés l'ensemble des dictionnaires,
- `hearders` : dossier où sont stockés l'ensemble des headers (fichiers finissant par `.h`) utilisés par votre code,
<!-- - `libs` : dossier où est stockés le fichier `io.o`, ce fichier vous permets de lire les fichiers avant le tp où vous apprendrez comment coder le lecteur de fichier, -->
- `src` : dossier où sont stockés l'ensemble de vos fichiers de code en C (fichiers finissant par `.c`),
- `build` : dossier où sont stockés l'ensemble des fichiers de compilation (exécutable et fichiers finissant par `.o`)
- `tests` : dossier où est stocké l'ensemble de vos codes de tests.



### Spécification du programme
Le `Makefile` génère un exécutable `spellchecker`:
```sh
./spellchecker --dicts <dictionary_path> --input <input_path> [--output <output_path>] [--mode <mode>] [--threads <number>] [--verbose]
```
- `--dicts <dictionary_path>` spécifie le dossier dans lequel sont rangés les dictionnaires
- `--input <input_path>` spécifie le fichier contenant les lignes à corriger. - `--output_path <output_path>` spécifie le nom des fichiers de sortie (sans extension !)[^1]. S'il n'est pas spécifié, le terminal sera utilisé par défaut
- `--mode <mode>` spécifie le mode de fonctionnement du programme. S'il n'est pas spécifié, le mode `detection` sera utilisé par défaut
- `--threads <number>` spécifie le nombre de threads à utiliser
- `--verbose` permet d'afficher plus de détails sur ce que fait le programme en temps réel (mode verbeux)

Lorsque des crochets sont présents dans une ligne de commande, comme `[--mode <mode>]` par exemple, le contenu entre ceux-ci est optionnel. Le programme fonctionnera sans problème s'ils ne sont pas spécifiés. Les chevrons, comme `<input_path>` indiquent l'endroit où écrire la valeur que prend l'argument. Les crochets, comme les chevrons, ne doivent pas être inclus dans la ligne de commande.

[^1]: La sortie de la détection sera écrite dans `<output_path>.err` tandis que la sortie de la correction sera écrite dans `<output_path>.fix`. Voir le [format](#fichiers-de-sortie) des fichiers de sortie pour plus de détails.

### Formats de fichiers
##### Dictionnaires
Un fichier dictionnaire portera pour nom `<lang>.dict` où `<lang>` sera la langue dans laquelle il est écrit. Son contenu suivra l'encodage [`latin-1`](https://fr.wikipedia.org/wiki/ISO/CEI_8859-1), et chaque ligne comportera un et un seul mot. Chaque ligne sera séparée par un caractère de saut de ligne `\n` (aussi appelé Line Feed: `LF`). La première ligne contiendra l'identifiant du dictionnaire sous forme d'un `uint32`.

#### Lignes à corriger
Un fichier de lignes à corriger sera au format binaire, et respectera les spécifications suivantes :
- Les lignes à corriger sont écrites les unes à la suite des autres sans séparateur
- Une ligne commence par 4 octets codant un `uint32` en Big-Endian[^2] indiquant la taille de la ligne à venir en *bytes*
- Le texte d'une ligne est écrit suivant l'encodage [`latin-1`](https://fr.wikipedia.org/wiki/ISO/CEI_8859-1).

Afin de vous permettre d'identifier efficacement le dictionnaire, vous pouvez considérer que plus de 90% de la ligne à corriger est déjà écrite correctement.

[^2]: Big-Endian est une des 2 configurations principales d'[*endianness*](https://en.wikipedia.org/wiki/Endianness) qui existent. Pour ce projet, nous vous obligeons à utiliser la notation **Big-Endian**. Les fonctions `htobe32` et `be32toh` présentes dans le fichier en-tête `portable_endian.h` devraient vous aider. 

> [!WARNING] 
> Il est **nécessaire** d'utiliser le fichier en-tête `portable_endian.h` fourni dans vos fichiers. L'inclusion du fichier `endian.h` présent nativement sur la plupart des distributions Linux ne sera pas autorisée.

#### Fichiers de sortie
L'argument `--mode` permet de spécifier le mode de fonctionnement du programme et sa sortie. Ce paramètre peut prendre 2 valeurs (ou plus si vous décidez d'en ajouter). 

##### `--mode detection` : sortie dans `<output_path>.err`
Dans ce mode, le programme doit écrire un fichier binaire listant les mots mal orthographiés.
Pour chaque ligne de l’entrée, le programme écrit un bloc structuré comme suit :

- `uint32`: **(4 octets)**:  numéro de la ligne `n` (0 pour la première, 1 pour la deuxième, ...)
- `uint32`: **(4 octets)**: indice du dictionnaire utilisé `d`
- `uint32`: **(4 octets)**: nombre de mots incorrects `nb`
- `nb`$\times$`uint32`: **(4 octets chacun)**: positions (`offsets`) des mots incorrects dans la ligne, commençant à 0. 

> [!Note]
L'`offset` des mots à écrire correspond bien à leur place dans la phrase, voir exemples ci-dessous.


Si l'entrée à corriger comporte plusieurs lignes, les blocs sont écrits à la suite dans le fichier.

Dans les exemples ci-dessous, le format hexadécimal est utilisé pour représenter les données binaires. 
#### Exemple 1
Entrée:
> Je suis étudiant à l'Ékole Polytechnique de Louvain

- Ligne: `n=0`
- Dictionnaire choisi: `d=0`
- Mots erronés: `nb=1`: `"ékole"` (offset `4`)

Le bloc attendu dans `<output_path>.err` est 
```hex
n=0|d=0|nb=1|[4]
```
En vue hexadécimale (big endian):
```hex
00 00 00 00   00 00 00 00 
00 00 00 01   00 00 00 04
```

#### Exemple 2
Entrée:
> Le mardi z'est friite au restaurant universitaire 
>
> Le bâtimant du Cyclotron est zuper louin

Supposons que les meilleurs dictionnaires soient le `9` pour la première ligne et le `12` pour la seconde. 

On aura ici 2 blocs: 
- Ligne `n=0`: dictionnaire `d=9`, erreurs `"z'est"` (offset `2`) et `"friite"` (offset `3`) → `nb=2`
- Ligne `n=1`: dictionnaire `d=12`, erreurs `"bâtimant"` (offset `1`), `"zuper"` (offset `5`) et `"louin"` (offset `6`) → `nb=3`

Les blocs binaires attendus dans `<output_path>.err` sont:
```hex
n=0|d=9|nb=2|[2,3]
n=1|d=12|nb=3|[1,5,6]
```
En hexadécimal:
```hex
00 00 00 00    00 00 00 09
00 00 00 02    00 00 00 02
00 00 00 03   *00 00 00 01
00 00 00 0C    00 00 00 03
00 00 00 01    00 00 00 05
00 00 00 06 
```

> [!TIP]
> L'astérique `*` marque le début du second bloc. Il n'est présent que vous aider à visualiser l'exemple, il ne doit pas être inclus dans la vraie sortie.

##### `--mode correction` : sorties dans `<output_path>.err` et `<output_path>.fix`

En mode correction, deux fichiers sont générés:

1. `<output_path>.err`: identique au mode `detection` (cf. ci-dessus)
2. `<output_path>.fix`: contient les corrections des mots fautifs, dans le même ordre que les offsets décrits dans `.err`. 

Le fichier `<output_path>.fix` est une suite de blocs:
- `uint32`: **(4 octets, big endian)**: longueur du mot corrigé `N+1` en octets (+1 pour le `NULL` char !) ([ISO8859-1](https://fr.wikipedia.org/wiki/ISO/CEI_8859-1))
- `(N+1) × char`: les caractères du mot corrigé (y compris le `NULL` char !)

#### Exemple 
> Le mardi z'est friite au restaurant universitaire
>
> Le bâtimant du Cyclotron est zuper louin

Sortie `.err` (vu ci-dessus) :
```hex
n=0|d=9|nb=2|[2,3]
n=1|d=12|nb=3|[1,5,6]
```

Sortie `.fix`: 
corrections écrites dans l'ordre des offsets:
- Ligne `0`, offset `2`: `"c'est"`
- Ligne `0`, offset `3`: `"frite"`
- Ligne `1`, offset `1`: `"bâtiment"`
- Ligne `1`, offset `5`: `"super"`
- Ligne `1`, offset `6`: `"loin"`

\
Bloc binaire attendu (`<output_path>.fix`):
```hex
00 00 00 05    63 27 65 73    # 5 c'es
74 00 00 00    05 66 72 69    # t 5 fri
74 65 00 00    00 08 62 4C    # te 8 bâ
74 69 6D 65    6E 74 00 00    # timent
00 05 73 75    70 65 72 00    # 5 super
00 00 04 6C    6F 69 6E       # 4 loin
```

> [!WARNING] 
> S'il n'y a aucune correction à faire, le fichier `<output_path>.err` indiquera les dictionnaires détectés pour chaque ligne et le fichier `<output_path>.fix` sera vide (mais existant !) 

### Contraintes de programmation

Lorsqu'on développe un programme, on ne peut jamais être certain que, même si le code est dépourvu de bug, le programme ne s'arrête pas de manière prématurée. Que ce soit parce que l'utilisateur ait entré `CTRL+C`, que la prise de la machine ait été arrachée, ou qu'une comète ait détruit la civilisation, il ne faut jamais remettre à plus tard ce qu'on peut faire tout de suite.

A cet effet, nous vous demandons d'**écrire** vos résultats dans les fichiers de sortie **au fur et à mesure**. De cette manière, si votre programme s'arrête de manière inattendue, le travail déjà réalisé sera sauvegardé.
A vous de vous arranger pour que cela ne perturbe pas le fonctionnement normal de la lecture/écriture (I/O) ni l'aspect multithreading dans la suite du projet.

# D. Le Championnat

Nouveauté cette année, vos codes seront lus et exécutés chaque dimanche à **23h59** afin d'établir un classement des codes selon différentes métriques. Nous mesurerons la rapidité et la précision de votre code à renvoyer les mots incorrectements orthographiés ainsi que leur correction. Le classement sera ensuite actualisé automatiquement et vous pourrez le consulter à l'adresse [waf-test1.uclouvain.be](waf-test1.uclouvain.be) 🎉

Nous exécuterons votre code avec la commande suivante: 
```sh
./spellchecker --dicts <dicts_path> --input <input_path> --output <output_path> --mode detection|correction --threads <number>
```

# E. Outils
Durant ce projet vous apprendrez également à utiliser [<img height="16" src="https://upload.wikimedia.org/wikipedia/commons/thumb/e/e0/Git-logo.svg/250px-Git-logo.svg.png">](https://git-scm.com/book/fr/v2https://valgrind.org/docs/manual/QuickStart.html) ainsi que [Valgrind](https://valgrind.org/docs/manual/QuickStart.html) et [CppCheck](https://cppcheck.sourceforge.io/manual.pdf). 
- [*Git*](https://forge.uclouvain.be/lepl15031/students/practical-sessions/2025-2026/-/blob/main/Git.md?ref_type=heads) est un système de gestion de versions de code. Il vous permet entre autres de garder l'historique de vos modifications et d'y revenir si besoin. Vous l'utiliserez activement tout au long de votre projet pour gérer votre dépôt de code. 
- *Valgrind* vous permet de débuguer l'exécution de votre programme. Lorsque vous découvrirez les (si connus) `segfault`'s, ce programme vous permettra de détecter les potentielles sources du problème (pensez à compiler avec le flag `-g` 😉). Il vous permettra également d'analyser la consommation mémoire et de vérifier que vous libérez bien toute la mémoire que vous allouez. Pour l'utiliser, tapez `valgrind <command_line>`, par exemple `valgrind ./spellchecker --input path/to/input`. Lorsque vous utilisez *Valgrind* ou tout autre debugger dynamique, pensez à compiler avec l'option `-g` afin d'inclure des informations supplémentaires de déboggage dans la compilation.
- *Cppcheck* vous permet de vérifier statiquement que votre code C ne présente pas de problème *a priori*. Pour l'utiliser, il suffit d'utiliser `cppcheck path/to/file` ou `cppcheck path/to/dir/` pour analyser un fichier ou tous les fichiers d'un dossier. Le programme vous renverra les erreurs potentielles ainsi que le fichier et la ligne problématique.
- Utilisation du flag `-fsanitize` pour aider à détecter les accès hors d'un tableau


# F. Evaluations & Livrables
- Vendredi 20/03 à 18:00 : Finir la partie séquentielle du projet qui sera évaluée dans les peer reviews.
- Vendredi 27/03 à 18:00 : Remettre les peer reviews et remplir l'évaluation de groupe sur Dynamo
- Vendredi 08/05 à 18:00 : Le projet doit être finalisé dans le git de chaque groupe avec un README qui explique de façon détaillé comment compiler, executer le code et reproduire les résultats de benchmarks
- **[SINF]** Mardi 12/05 à 18:00 : Remettre les slides de la présentation au format pdf
- **[SINF]** Mercredi 13/05 : Présentations orales
- **[FSA]** Jeudi 14/05 à 18:00 : Remettre les slides de la présentation au format pdf
- **[FSA]** Vendredi 15/05 : Présentations orales

Lors de l’évaluation, nous vérifierons la participation de chaque membre du groupe en consultant les *merge requests* fusionnées sur GitLab.
Ainsi, si un membre contribue sans passer par des *merge requests* pour soumettre ses modifications, nous supposerons qu’il n’a pas participé.

## Présentation orale

Chaque groupe disposera de 8 minutes maximum pour présenter son travail. Ce temps est court : la présentation doit être structurée, synthétique et technique. Il ne s’agit pas de refaire une démonstration complète du projet, mais de mettre en avant vos choix d’implémentation, vos analyses et votre esprit critique. Vous êtes libres de choisir quoi présenter dans le temps imparti.
Exemple de contenu attendu :
- Principaux choix d’architecture réalisés au cours du projet
- Algorithme de correction
- Stratégie de thread safety : Explication claire des mécanismes utilisés et justification
- Deux implémentations différentes de détection (Ajoutez alors une option dans la suite de tests permettant de choisir la version du code de détection à tester) :
    - Comparaison via un benchmark rigoureux
    - Situation avec une réelle incertitude avant benchmark
    - Analyse et explication des observations (Temps d’exécution, Consommation mémoire, Consommation énergétique)

Nous poserons des questions à l’issue de la présentation et choisirons aléatoirement le membre du groupe qui devra répondre.


# G. Informations relatives au cours

Toutes les informations relatives à l'organisation, à l'évaluation du cours et à la politique concernant l'utilisation des intelligences artificielles sont disponibles sur le [Moodle du cours](https://moodle.uclouvain.be/course/view.php?id=3842) et à l'adresse [https://uclouvain.be/cours-2025-lepl1503](https://uclouvain.be/cours-2025-lepl1503). Seules les informations reprises sur ce site font foi.
