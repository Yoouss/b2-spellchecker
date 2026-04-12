#include "common.h"
#include <stdint.h>
#include <stdio.h>
#include "dict.h"
#include "input.h"

#ifndef CORRECTOR_H
#define CORRECTOR_H

/**
 * structure représentant la distance de Levenshtein obtenue entre un mot et le mot mal orthographié
 *
 * @index : l'indexe du mot
 * @value : la valeur de la distance de Levenshtein calculée par compute_levenshtein_distance
 */
typedef struct Distance {
    int index;
    int value;
} Distance_t;

/**
 * @brief  calcule le nombre de trigrammes (= mot de trois lettres) de word1 présents dans word2
 * *       fonction utilisée par get_candidate_words pour réduire le nombre de mots candidats du dictionnaire
 * 
 * @param  word1 le premier mot
 * @param  word2 le deuxième mot
 * @return le nombre de trigrammes présents dans les deux mots ou 0 si un des deux mots est trop petit
 *
 * @note   complexité temporelle : O(n * m)
 *         complexité spaciale : O(1)
 * 
 *         n = la taille de word1
 *         m = la taille de word2
 */
int count_number_of_shared_trigrammes(char* word1, char* word2);

/**
 * @brief  retourne un tableau de mots candidats qui ont au moins 2 trigrammes en commun (ou 1 si le mot mal ortographié est court) avec le mot mal ortographié
 * 
 * @param  wrong_word le mot mal orthographié
 * @param  dict le dictionnaire utilisé
 * @param  result_count un pointeur vers un entier qui sera le nombre de mots candidats après l'appel à get_candidate_words
 * @return un tableau avec les mots candidats ou NULL en cas d'erreur
 * 
 * @note   complexité temporelle : O(d * (n * m))
 *         complexité spaciale : O(d)
 * 
 *         d = la taille du dictionnaire
 *         n = la taille du mot mal orthographié
 *         m = la taille du mot actuel dans le dictionnaire
 */
char** get_candidate_words(char* wrong_word, Dictionary_t* dict, int* result_count);

/**
 * @brief  calcule le minimum entre trois entiers
 * *       utilisée pour déterminer l'opération la moins coûteuse (insertion, suppression ou substitution) dans compute_levenshtein_distance
 * 
 * @param  a premier entier (coût de suppression)
 * @param  b deuxième entier (coût d'insertion)
 * @param  c troisième entier (coût de substitution)
 * @return la plus petite valeur entre a, b et c
 *  
 * @note   complexité temporelle : O(1)
 *         complexité spaciale : O(1)
 */
int get_min3(int a, int b, int c);

/**
 * @brief  alloue dynamiquement une matrice d'entiers en mémoire de dimensions n x m
 * 
 * @param  n le nombre de lignes
 * @param  m le nombre de colonnes
 * @return un double pointeur qui est une matrice n x m ou NULL en cas de problème d'allocation de mémoire
 * 
 * @note   complexité temporelle : O(n)
 *         complexité spaciale : O(m * n) 
 *  
 *         n = le nombre de lignes de la matrice
 *         m = le nombre de colonnes de la matrice
 */
int** initialize_matrix(int n, int m);

/**
 * @brief  calcule la distance d'édition de Levenshtein entre deux mots
 * *       mesure le nombre minimal d'opérations (insertion, suppression, substitution) pour passer de word1 à word2
 * 
 * @param  word1 le premier mot
 * @param  word2 le second mot
 * @return la distance entre les 2 mots, 0 si les mots sont identiques, ou -1 en cas d'erreur
 * 
 * @note   complexité temporelle : O(n * m)
 *         complexité spaciale : O(1) 
 *  
 *         n = le nombre de lignes de la matrice
 *         m = le nombre de colonnes de la matrice
 */
int compute_levenshtein_distance(char* word1, char* word2);

/**
 * @brief  retourne l'index du mot dans candidates qui a la plus petite distance de Levenshtein avec le mot erroné
 * 
 * @param  wrong_word le mot erroné de base
 * @param  candidates la liste des mots candidats (char**)
 * @param  nb_candidates le nombre de candidats dans la liste
 * @return un entier qui est l'index du mot pour lequel la distance est la plus petite ou -1 en cas d'erreur
 * 
 * @note   complexité temporelle : O(c * (n * m))
 *         complexité spaciale : O(1) 
 *
 *         c = le nombre de candidats 
 *         n = le nombre de lignes de la matrice utilisée dans le calcul de la distance 
 *         m = le nombre de colonnes de la matrice utilisée dans le calcul de la distance
 */
int get_index_of_smallest_distance_word(char* wrong_word, char** candidates, int nb_candidates);

/**
 * @brief  retourne la correction pour un mot mal orthographié
 * 
 * @param  wrong_word le mot mal orthographié
 * @param  dict un pointeur vers un dictionaire de type Dictionary_t
 * @return un pointeur vers une nouvelle chaîne de caractères contenant le mot corrigé ou NULL en cas d'erreur 
 *
 * @note   complexité temporelle : O(d * (n * m))
 *         complexité spaciale : O(1) 
 *
 *         d = la taille du dictionnaire
 *         n = la taille du mot mal orthographié
 *         m = la taille du mot actuel dans le dictionnaire
 */
char* get_word_correction(char* wrong_word, Dictionary_t* dict);

#endif