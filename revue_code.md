# Guide de revue de code en C

Ce document propose un ensemble de lignes directrices pour effectuer une revue de code d'un programme écrit en C. Chaque point est accompagné d'un exemple de mauvaise pratique et d'une suggestion d'amélioration.

Notez qu'il n'est pas nécessaire de fermer l'issue sur gitlab. Mais si elle a été fermée, ce n'est pas un problème non plus.

---

## 1. Architecture globale du programme

L'architecture du programme est-elle claire sur base de la description (fichier README), de la découpe en fichiers et des commentaires fournis ?

Un programme bien structuré permet à un nouveau développeur de comprendre rapidement l'organisation du code, le rôle de chaque fichier et les relations entre les différents modules.

### Exemple de mauvaise pratique

```c
// main.c - un seul fichier contenant tout le programme
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

// 2000 lignes de code mélangées : parsing, calculs, affichage, gestion réseau...
void parse() { /* ... */ }
void compute() { /* ... */ }
void display() { /* ... */ }
void send_network() { /* ... */ }
int main() { /* ... */ }
```

**Problème** : tout le code est regroupé dans un seul fichier sans organisation logique. Il n'y a pas de README décrivant le projet, et les commentaires sont absents.

**Bonne pratique** : découper le programme en modules cohérents (`parser.c`, `compute.c`, `display.c`, `network.c`) avec leurs fichiers d'en-tête respectifs (`.h`), et fournir un README décrivant l'architecture, la compilation et l'utilisation du programme.

---

## 2. Structures de données principales

Les structures de données sont-elles correctement documentées ? Les noms des structures et des variables sont-ils parlants pour le programmeur ?

Des noms explicites et une documentation claire des structures facilitent la compréhension et la maintenance du code.

### Exemple de mauvaise pratique

```c
struct s {
    int x;
    char *p;
    int n;
    struct s *q;
};
```

**Problème** : le nom de la structure (`s`) et de ses champs (`x`, `p`, `n`, `q`) ne donnent aucune indication sur leur rôle. Un autre programmeur ne peut pas deviner à quoi ils servent.

**Bonne pratique** :

```c
/**
 * Structure représentant un étudiant dans la liste chaînée.
 *
 * @name    : nom de l'étudiant (chaîne allouée dynamiquement)
 * @age     : âge de l'étudiant en années
 * @noma    : numéro de matricule
 * @next    : pointeur vers l'étudiant suivant dans la liste
 */
struct student {
    char *name;
    int age;
    int noma;
    struct student *next;
};
```

---

## 3. Fonctions

Les noms, arguments et spécifications des fonctions sont-ils clairs et non ambigus ? Sur base de cette description, est-il facile pour un programmeur de réutiliser les fonctions proposées ? Lorsque les fonctions sont dans un module, les fonctions exposées et les fonctions privées sont-elles bien distinguées ?

En C, le mot-clé `static` appliqué à une fonction limite sa visibilité au fichier dans lequel elle est définie. C'est le mécanisme qui permet de distinguer les fonctions **privées** (internes au module, déclarées `static`) des fonctions **publiques** (exposées via le fichier `.h`). Une bonne conception de module expose le minimum nécessaire dans le `.h` et déclare toutes les fonctions auxiliaires comme `static` dans le `.c`.

### Exemple de mauvaise pratique

```c
// list.h - toutes les fonctions exposées sans distinction
int f(struct node *l, int v);
int g(struct node *l);
struct node *h(struct node *a, struct node *b);
```

```c
// list.c
// fonction auxiliaire utilisée uniquement en interne, mais exposée dans le .h
struct node *h(struct node *a, struct node *b) {
    /* fusionne deux sous-listes */
}

int f(struct node *l, int v) {
    /* recherche une valeur dans la liste */
}

int g(struct node *l) {
    /* calcule la longueur de la liste */
}
```

**Problème** : les noms de fonctions (`f`, `g`, `h`) sont incompréhensibles. Les arguments ne sont pas documentés. La fonction auxiliaire `h` est exposée dans le `.h` alors qu'elle n'est utilisée qu'en interne : n'importe quel autre fichier peut l'appeler, ce qui crée un couplage inutile et complique la maintenance.

**Bonne pratique** :

```c
// list.h - interface publique du module
/**
 * Recherche la valeur `value` dans la liste chaînée `list`.
 * Retourne 1 si la valeur est trouvée, 0 sinon.
 */
int list_contains(struct node *list, int value);

/**
 * Retourne le nombre d'éléments dans la liste chaînée `list`.
 */
int list_length(struct node *list);
```

```c
// list.c
#include "list.h"

// Fonction privée : static limite sa visibilité à ce fichier.
// Elle n'apparaît pas dans le .h et ne peut pas être appelée
// depuis un autre module.
static struct node *merge(struct node *left, struct node *right) {
    /* fusionne deux sous-listes */
}

int list_contains(struct node *list, int value) {
    /* ... */
}

int list_length(struct node *list) {
    /* ... */
}
```

---

## 4. Algorithmes

Les algorithmes utilisés dans le programme sont-ils clairs pour le lecteur et semblent-ils corrects ?

Un algorithme complexe doit être accompagné d'un commentaire décrivant son principe, sa complexité et éventuellement une référence.

### Exemple de mauvaise pratique

```c
void s(int *t, int n) {
    for (int i = 0; i < n - 1; i++)
        for (int j = 0; j < n - i - 1; j++)
            if (t[j] > t[j+1]) {
                int tmp = t[j]; t[j] = t[j+1]; t[j+1] = tmp;
            }
}
```

**Problème** : aucun commentaire n'explique qu'il s'agit d'un tri à bulles. Le nom de la fonction (`s`) et des paramètres (`t`, `n`) ne sont pas parlants. Un lecteur ne peut pas vérifier la correction de l'algorithme sans effort.

**Bonne pratique** :

```c
/**
 * Trie le tableau `array` de `length` entiers par ordre croissant
 * en utilisant l'algorithme du tri à bulles (bubble sort).
 * Complexité : O(n^2) en temps, O(1) en espace.
 *
 * @array  : tableau d'entiers à trier (modifié en place)
 * @length : nombre d'éléments dans le tableau
 */
void bubble_sort(int *array, int length) {
    for (int i = 0; i < length - 1; i++) {
        for (int j = 0; j < length - i - 1; j++) {
            if (array[j] > array[j + 1]) {
                // Échange des deux éléments adjacents
                int tmp = array[j];
                array[j] = array[j + 1];
                array[j + 1] = tmp;
            }
        }
    }
}
```

---

## 5. Gestion de la mémoire

L'utilisation de `malloc`/`calloc` est-elle claire ? `free` est-il utilisé correctement ? Y a-t-il des fuites de mémoire ? Le programme fait-il une distinction claire entre les données sur le heap et celles sur la stack ?

Il est fortement recommandé d'utiliser **Valgrind** ([documentation](https://valgrind.org/docs/manual/mc-manual.html)) pour vérifier l'absence de fuites de mémoire et de **documenter chaque fuite identifiée** dans le rapport de revue.

```bash
valgrind --leak-check=full --show-leak-kinds=all ./mon_programme
```

Valgrind signale chaque bloc de mémoire alloué mais non libéré, en indiquant la pile d'appels de l'allocation. Pour chaque fuite détectée, il convient de :
- identifier la fonction responsable de l'allocation,
- déterminer à quel moment la mémoire aurait dû être libérée,
- documenter le correctif à apporter.

### Exemple de mauvaise pratique

```c
char *concatenate(char *s1, char *s2) {
    char *result = malloc(strlen(s1) + strlen(s2) + 1);
    strcpy(result, s1);
    strcat(result, s2);
    return result;
}

void process() {
    char *msg = concatenate("hello", " world");
    printf("%s\n", msg);
    // pas de free(msg) -> fuite de mémoire !
}
```

**Problème** : la mémoire allouée par `malloc` dans `concatenate` n'est jamais libérée par l'appelant. De plus, le retour de `malloc` n'est pas vérifié (il peut retourner `NULL` si l'allocation échoue).

**Bonne pratique** :

```c
char *concatenate(char *s1, char *s2) {
    char *result = malloc(strlen(s1) + strlen(s2) + 1);
    if (result == NULL) {
        return NULL;  // signaler l'échec de l'allocation
    }
    strcpy(result, s1);
    strcat(result, s2);
    return result;
}

void process() {
    char *msg = concatenate("hello", " world");
    if (msg == NULL) {
        fprintf(stderr, "Erreur d'allocation mémoire\n");
        return;
    }
    printf("%s\n", msg);
    free(msg);  // libération de la mémoire allouée
    msg = NULL; // éviter un dangling pointer
}
```

---

## 6. Fonctions de la librairie standard et appels système

Les valeurs de retour des fonctions de la librairie standard et des appels système sont-elles systématiquement testées ? Les erreurs possibles sont-elles traitées correctement de façon à ce que le programme soit robuste ?

### Exemple de mauvaise pratique

```c
void write_to_file(char *filename, char *data) {
    int fd = open(filename, O_WRONLY | O_CREAT | O_TRUNC, 0644);
    write(fd, data, strlen(data));
    close(fd);
}
```

**Problème** : le retour de `open` n'est pas vérifié. Si le fichier ne peut pas être ouvert (permissions insuffisantes, disque plein, chemin invalide), `fd` vaut `-1` et les appels suivants à `write` et `close` provoquent un comportement indéfini. Les retours de `write` et `close` ne sont pas non plus vérifiés — `write` peut écrire moins d'octets que demandé, et `close` peut échouer (par exemple en cas d'erreur d'écriture différée sur un système de fichiers réseau).

**Bonne pratique** :

```c
int write_to_file(char *filename, char *data) {
    int fd = open(filename, O_WRONLY | O_CREAT | O_TRUNC, 0644);
    if (fd == -1) {
        perror("open");
        return -1;
    }

    ssize_t len = strlen(data);
    ssize_t written = write(fd, data, len);
    if (written == -1) {
        perror("write");
        close(fd);
        return -1;
    }
    if (written < len) {
        fprintf(stderr, "write: écriture partielle (%zd/%zd octets)\n",
                written, len);
        close(fd);
        return -1;
    }

    if (close(fd) == -1) {
        perror("close");
        return -1;
    }
    return 0;
}
```

---

## 7. Formatage du code

Le code est-il lisible et uniformément formaté ? Un style cohérent facilite la lecture, réduit les erreurs et rend les revues de code plus efficaces.

Il est recommandé d'utiliser **clang-format** ([documentation](https://clang.llvm.org/docs/ClangFormat.html)) pour formater automatiquement le code C selon un style prédéfini. Le style **WebKit** est un bon choix : il est lisible, largement utilisé et bien adapté au C.

```bash
# Formater un fichier en place
clang-format -i --style=WebKit mon_fichier.c

# Vérifier sans modifier (utile en CI)
clang-format --style=WebKit --dry-run --Werror mon_fichier.c
```

### Exemple de mauvaise pratique

```c
int bubble_sort(int*array,int length){
for(int i=0;i<length-1;i++){
for(int j=0;j<length-i-1;j++){
if(array[j]>array[j+1]){int tmp=array[j];array[j]=array[j+1];array[j+1]=tmp;}}}
return 0;}
```

**Problème** : le code est illisible. L'indentation est absente, les espaces manquent autour des opérateurs, plusieurs instructions sont regroupées sur une même ligne. Une revue de code sur ce style est très difficile.

Après `clang-format -i --style=WebKit bubble_sort.c` :

```c
int bubble_sort(int* array, int length)
{
    for (int i = 0; i < length - 1; i++) {
        for (int j = 0; j < length - i - 1; j++) {
            if (array[j] > array[j + 1]) {
                int tmp = array[j];
                array[j] = array[j + 1];
                array[j + 1] = tmp;
            }
        }
    }
    return 0;
}
```

**Bonne pratique** : appliquer `clang-format` sur l'ensemble des fichiers sources dès le début du projet et s'y tenir tout au long du développement. On peut créer un fichier `.clang-format` à la racine du projet pour fixer le style une fois pour toutes :

```bash
# Générer un fichier de configuration basé sur le style WebKit
clang-format --style=WebKit --dump-config > .clang-format
```

Tous les membres de l'équipe utilisent alors le même fichier et le style est appliqué de façon identique sur toutes les machines.

---

## 8. Tests

Les fonctions principales sont-elles couvertes par une suite de tests ? Celle-ci couvre-t-elle tous les cas possibles (cas normaux, cas limites, cas d'erreur) ?

### Mesurer la couverture avec gcov

**gcov** ([documentation](https://gcc.gnu.org/onlinedocs/gcc/Gcov.html)) est un outil de couverture de code. Il permet de déterminer, pour chaque ligne du code source, combien de fois elle a été exécutée lors des tests. Son objectif est d'identifier les portions du code qui ne sont pas couvertes par les tests.

Le principe est le suivant :

1. **Compilation instrumentée** : on compile avec `-fprofile-arcs -ftest-coverage`. Le compilateur insère du code d'instrumentation qui compte les exécutions de chaque ligne et branche.

2. **Exécution** : on lance les tests. Les compteurs sont écrits dans des fichiers `.gcda`.

3. **Analyse** : on lance `llvm-cov gcov` sur le fichier source. Il combine les informations de compilation (`.gcno`) et d'exécution (`.gcda`) pour produire un rapport ligne par ligne.

Supposons le fichier d'implémentation `list.c` (fonctions `list_insert`, `list_contains`, `list_free`) et le fichier de tests `test_list.c` utilisant CUnit :

```bash
# 1. Compilation avec les options de couverture
clang -std=gnu99 -fprofile-arcs -ftest-coverage -o test_list test_list.c list.c -lcunit

# 2. Exécution des tests
./test_list

# 3. Génération du rapport de couverture
#    (llvm-cov gcov est la version de gcov compatible avec clang)
llvm-cov gcov list.c -gcno=test_list-list.gcno -gcda=test_list-list.gcda
```

Le fichier `list.c.gcov` produit indique pour chaque ligne le nombre d'exécutions :

```
        1:    4:struct node *list_insert(struct node *head, int value)
        -:    5:{
        1:    6:    struct node *new = malloc(sizeof(struct node));
        1:    7:    if (new == NULL)
    #####:    8:        return NULL;
        1:    9:    new->value = value;
        1:   10:    new->next = head;
        1:   11:    return new;
        1:   12:}
        -:   13:
        1:   14:int list_contains(struct node *list, int value)
        -:   15:{
        1:   16:    while (list != NULL) {
        1:   17:        if (list->value == value)
        1:   18:            return 1;
    #####:   19:        list = list->next;
        -:   20:    }
    #####:   21:    return 0;
        1:   22:}
```

- Un nombre (ex. `1:`) indique combien de fois la ligne a été exécutée.
- `#####` signifie que la ligne n'a **jamais** été exécutée.
- `-` indique une ligne non exécutable (commentaire, accolade, ligne vide).

Ici, trois lignes sont marquées `#####` :
- ligne 8 : le cas `malloc` retournant `NULL` n'est jamais testé,
- ligne 19 : la boucle ne parcourt jamais plus d'un élément,
- ligne 21 : `list_contains` n'est jamais appelé avec un élément absent.

Chaque ligne `#####` doit conduire à l'écriture d'un test supplémentaire. Le résumé de couverture est affiché à la fin :

```
Lines executed:85.71% of 21
```

L'objectif est d'atteindre 100 % de couverture.

### Exemple de mauvaise pratique

```c
// Un seul test qui ne vérifie qu'un cas simple
void test_list_contains() {
    struct node *list = list_insert(NULL, 42);
    CU_ASSERT_EQUAL(list_contains(list, 42), 1);
    free(list);
}
```

**Problème** : un seul cas est testé (élément présent). Les cas limites ne sont pas couverts : élément absent, liste vide, liste à plusieurs éléments, recherche du premier ou du dernier élément.

**Bonne pratique** :

```c
void test_list_contains() {
    // Cas limite : liste vide
    CU_ASSERT_EQUAL(list_contains(NULL, 42), 0);

    // Construction d'une liste : 3 -> 7 -> 15 -> NULL
    struct node *list = NULL;
    list = list_insert(list, 15);
    list = list_insert(list, 7);
    list = list_insert(list, 3);

    // Cas normal : élément présent
    CU_ASSERT_EQUAL(list_contains(list, 7), 1);

    // Cas limites : premier et dernier élément
    CU_ASSERT_EQUAL(list_contains(list, 3), 1);
    CU_ASSERT_EQUAL(list_contains(list, 15), 1);

    // Cas d'erreur : élément absent
    CU_ASSERT_EQUAL(list_contains(list, 99), 0);
    CU_ASSERT_EQUAL(list_contains(list, 0), 0);

    // Libération de la mémoire
    // ...
}
```

---

## 9. Threads et synchronisation *(optionnel)*

> Cette section est optionnelle : elle s'applique uniquement si le programme à revoir utilise des threads POSIX. Si ce n'est pas le cas, vous pouvez l'ignorer.

L'architecture multi-threadée est-elle claire ? Y a-t-il un risque de violation de section critique, des variables non protégées ou trop protégées, ou des erreurs dans l'utilisation des mutex et sémaphores ?

Il est recommandé d'utiliser **Helgrind** ([documentation](https://valgrind.org/docs/manual/hg-manual.html)), un outil de Valgrind, pour détecter les problèmes de synchronisation tels que les *data races*, les deadlocks et les utilisations incorrectes de l'API POSIX threads ([documentation](https://man7.org/linux/man-pages/man7/pthreads.7.html)).

```bash
valgrind --tool=helgrind ./mon_programme
```

### Exemple 1 : data race

```c
int counter = 0;

void *increment(void *arg) {
    for (int i = 0; i < 1000000; i++) {
        counter++;  // accès concurrent non protégé !
    }
    return NULL;
}

int main() {
    pthread_t t1, t2;
    pthread_create(&t1, NULL, increment, NULL);
    pthread_create(&t2, NULL, increment, NULL);
    pthread_join(t1, NULL);
    pthread_join(t2, NULL);
    printf("counter = %d\n", counter);  // résultat imprévisible
    return 0;
}
```

**Problème** : la variable `counter` est modifiée simultanément par deux threads sans aucune protection. C'est une violation de section critique classique (*data race*). Le résultat final est imprévisible et dépend de l'ordonnancement des threads. De plus, les valeurs de retour de `pthread_create` et `pthread_join` ne sont pas vérifiées.

Helgrind détecte immédiatement le problème et identifie la ligne exacte :

```
==1816025== Possible data race during read of size 4 at 0x10C02C by thread #3
==1816025== Locks held: none
==1816025==    at 0x109178: increment (data_race.c:9)
==1816025==
==1816025== This conflicts with a previous write of size 4 by thread #2
==1816025== Locks held: none
==1816025==    at 0x109181: increment (data_race.c:9)
==1816025==  Address 0x10c02c is 0 bytes inside data symbol "counter"
```

**Bonne pratique** :

```c
int counter = 0;
pthread_mutex_t mutex = PTHREAD_MUTEX_INITIALIZER;

void *increment(void *arg) {
    for (int i = 0; i < 1000000; i++) {
        int err = pthread_mutex_lock(&mutex);
        if (err != 0) {
            fprintf(stderr, "pthread_mutex_lock: %s\n", strerror(err));
            return NULL;
        }
        counter++;
        err = pthread_mutex_unlock(&mutex);
        if (err != 0) {
            fprintf(stderr, "pthread_mutex_unlock: %s\n", strerror(err));
            return NULL;
        }
    }
    return NULL;
}

int main() {
    pthread_t t1, t2;
    int err;

    err = pthread_create(&t1, NULL, increment, NULL);
    if (err != 0) {
        fprintf(stderr, "pthread_create: %s\n", strerror(err));
        return EXIT_FAILURE;
    }
    err = pthread_create(&t2, NULL, increment, NULL);
    if (err != 0) {
        fprintf(stderr, "pthread_create: %s\n", strerror(err));
        return EXIT_FAILURE;
    }

    err = pthread_join(t1, NULL);
    if (err != 0) {
        fprintf(stderr, "pthread_join: %s\n", strerror(err));
        return EXIT_FAILURE;
    }
    err = pthread_join(t2, NULL);
    if (err != 0) {
        fprintf(stderr, "pthread_join: %s\n", strerror(err));
        return EXIT_FAILURE;
    }

    pthread_mutex_destroy(&mutex);
    printf("counter = %d\n", counter);  // toujours 2000000
    return EXIT_SUCCESS;
}
```

### Exemple 2 : deadlock

Un *deadlock* (interblocage) survient lorsque deux threads s'attendent mutuellement, chacun détenant un verrou dont l'autre a besoin.

```c
pthread_mutex_t mutex_a = PTHREAD_MUTEX_INITIALIZER;
pthread_mutex_t mutex_b = PTHREAD_MUTEX_INITIALIZER;

void *thread1(void *arg) {
    int err = pthread_mutex_lock(&mutex_a);
    if (err != 0) { fprintf(stderr, "pthread_mutex_lock: %s\n", strerror(err)); return NULL; }
    err = pthread_mutex_lock(&mutex_b);  // attend que thread2 libère mutex_b
    if (err != 0) { fprintf(stderr, "pthread_mutex_lock: %s\n", strerror(err)); return NULL; }
    pthread_mutex_unlock(&mutex_b);
    pthread_mutex_unlock(&mutex_a);
    return NULL;
}

void *thread2(void *arg) {
    int err = pthread_mutex_lock(&mutex_b);
    if (err != 0) { fprintf(stderr, "pthread_mutex_lock: %s\n", strerror(err)); return NULL; }
    err = pthread_mutex_lock(&mutex_a);  // attend que thread1 libère mutex_a
    if (err != 0) { fprintf(stderr, "pthread_mutex_lock: %s\n", strerror(err)); return NULL; }
    pthread_mutex_unlock(&mutex_a);
    pthread_mutex_unlock(&mutex_b);
    return NULL;
}
```

**Problème** : `thread1` acquiert `mutex_a` puis tente d'acquérir `mutex_b`, tandis que `thread2` fait l'inverse. Si les deux threads progressent simultanément, chacun détient le verrou que l'autre attend : le programme se bloque indéfiniment. Helgrind détecte la violation d'ordre d'acquisition avant même que le blocage ne se produise :

```
==1816066== Thread #3: lock order "0x10C048 before 0x10C070" violated
==1816066==
==1816066== Observed (incorrect) order is: acquisition of lock at 0x10C070
==1816066==    at 0x484FBEC: mutex_lock_WRK (hg_intercepts.c:944)
==1816066==    by 0x109277: thread2 (deadlock.c:23)
==1816066==
==1816066==  followed by a later acquisition of lock at 0x10C048
==1816066==    at 0x484FBEC: mutex_lock_WRK (hg_intercepts.c:944)
==1816066==    by 0x1092C1: thread2 (deadlock.c:26)
==1816066==
==1816066== Required order was established by acquisition of lock at 0x10C048
==1816066==    at 0x484FBEC: mutex_lock_WRK (hg_intercepts.c:944)
==1816066==    by 0x1091A7: thread1 (deadlock.c:10)
==1816066==
==1816066==  followed by a later acquisition of lock at 0x10C070
==1816066==    at 0x484FBEC: mutex_lock_WRK (hg_intercepts.c:944)
==1816066==    by 0x1091F1: thread1 (deadlock.c:13)
==1816066==
==1816066==  Lock at 0x10C048 is data symbol "mutex_a"
==1816066==  Lock at 0x10C070 is data symbol "mutex_b"
==1816066==
==1816066== ERROR SUMMARY: 1 errors from 1 contexts (suppressed: 12 from 12)
```

Helgrind indique que `thread1` a établi l'ordre `mutex_a → mutex_b` (lignes 10 et 13), mais que `thread2` les acquiert dans l'ordre inverse `mutex_b → mutex_a` (lignes 23 et 26).

**Bonne pratique** : imposer un ordre global d'acquisition des mutex. Tous les threads doivent toujours acquérir `mutex_a` avant `mutex_b` :

```c
void *thread1(void *arg) {
    int err = pthread_mutex_lock(&mutex_a);
    if (err != 0) { fprintf(stderr, "pthread_mutex_lock: %s\n", strerror(err)); return NULL; }
    err = pthread_mutex_lock(&mutex_b);  // ordre : a puis b
    if (err != 0) { fprintf(stderr, "pthread_mutex_lock: %s\n", strerror(err)); return NULL; }
    pthread_mutex_unlock(&mutex_b);
    pthread_mutex_unlock(&mutex_a);
    return NULL;
}

void *thread2(void *arg) {
    int err = pthread_mutex_lock(&mutex_a);  // même ordre que thread1 : a puis b
    if (err != 0) { fprintf(stderr, "pthread_mutex_lock: %s\n", strerror(err)); return NULL; }
    err = pthread_mutex_lock(&mutex_b);
    if (err != 0) { fprintf(stderr, "pthread_mutex_lock: %s\n", strerror(err)); return NULL; }
    pthread_mutex_unlock(&mutex_b);
    pthread_mutex_unlock(&mutex_a);
    return NULL;
}
```

---

## Résumé

| # | Point de vérification | Outil recommandé |
|---|----------------------|------------------|
| 1 | Architecture globale | — |
| 2 | Structures de données | — |
| 3 | Fonctions (noms, visibilité, `static`) | — |
| 4 | Algorithmes | — |
| 5 | Gestion de la mémoire | [Valgrind Memcheck](https://valgrind.org/docs/manual/mc-manual.html) |
| 6 | Librairie standard et appels système | — |
| 7 | Formatage du code | [clang-format](https://clang.llvm.org/docs/ClangFormat.html) |
| 8 | Tests et couverture | [gcov](https://gcc.gnu.org/onlinedocs/gcc/Gcov.html) |
| 9 | Threads et synchronisation *(optionnel)* | [Helgrind](https://valgrind.org/docs/manual/hg-manual.html) |

---

## Reproduire les exemples

Tous les fichiers sources utilisés dans ce document sont disponibles dans le répertoire [`exemples_revue/`](exemples_revue/).

### Section 8 — Couverture de code

| Fichier | Rôle |
|---------|------|
| [`exemples_revue/list.h`](exemples_revue/list.h) | Interface du module (déclarations) |
| [`exemples_revue/list.c`](exemples_revue/list.c) | Implémentation (`list_insert`, `list_contains`, `list_free`) |
| [`exemples_revue/test_list.c`](exemples_revue/test_list.c) | Suite de tests CUnit incomplète (illustre la couverture partielle) |

```bash
cd exemples_revue
make coverage      # compile, lance les tests et génère list.c.gcov
```

### Section 9 — Threads et synchronisation *(optionnel)*

| Fichier | Rôle |
|---------|------|
| [`exemples_revue/data_race.c`](exemples_revue/data_race.c) | Exemple de *data race* (compteur sans mutex) |
| [`exemples_revue/deadlock.c`](exemples_revue/deadlock.c) | Exemple de deadlock (ordre d'acquisition inversé) |

```bash
cd exemples_revue
make helgrind-race     # compile data_race.c et analyse avec Helgrind
make helgrind-deadlock # compile deadlock.c et analyse avec Helgrind
```

Le [`Makefile`](exemples_revue/Makefile) détecte automatiquement la version de `llvm-cov` compatible avec le compilateur clang installé.
