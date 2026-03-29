#include "common.h"
#include <stdint.h>
#include <stdio.h>

#include "dict.h"
#include "input.h"

#ifndef CORRECTOR_H
#define CORRECTOR_H

/**
 * @brief Calcule le code Soundex d'un caractère
 * 
 * @param c Le caractère à encoder
 * @return le code Soundex
 */
char get_soundex_code(char c);

/**
 * @brief Calcule le Soundex d'un mot
 * 
 * @param word Le mot à encoder
 * @return Un pointeur vers une chaîne de caractères (le code Soundex)
 */
char* soundex(char* word);

/**
 * @brief Parcourt le dictionnaire pour trouver les mots ayant le même code Soundex
 * 
 * @param wrong_word Le mot mal orthographié
 * @param dict Le dictionnaire de référence
 * @param result_count Pointeur pour stocker le nombre de candidats trouvés
 * @return Un tableau de mots (char**) contenant les mots candidats ayant le même soundex
 */
char** get_candidate_words(char* wrong_word, Dictionary_t* dict, int* result_count);

/**
 * @brief Calcule le minimum de trois entiers
 * * Utilisée pour déterminer l'opération la moins coûteuse (insertion, suppression ou substitution) dans calculate_distance
 * 
 * @param a Premier entier (coût de suppression)
 * @param b Deuxième entier (coût d'insertion)
 * @param c Troisième entier (coût de substitution)
 * @return La plus petite valeur entre a, b et c
 */
int get_min3(int a, int b, int c);

/**
 * @brief Alloue dynamiquement une matrice d'entiers en mémoire de dimensions n x m
 * 
 * @param n Le nombre de lignes
 * @param m Le nombre de colonnes
 * @return Un double pointeur qui est une matrice n x m ou NULL en cas de problème d'allocation de mémoire 
 */
int** initialize_matrix(int n, int m);

/**
 * @brief Calcule la distance d'édition de Levenshtein entre deux mots
 * * Mesure le nombre minimal d'opérations (insertion, suppression, substitution) pour passer de word1 à word2
 * 
 * @param word1 Le premier mot
 * @param word2 Le second mot
 * @return La distance entre les 2 mots (0 si les mots sont identiques)
 */
int calculate_distance(char* word1, char* word2);

/**
 * @brief Génère une matrice de distances pour les candidats
 * 
 * @param wrong_word Le mot erroné de base
 * @param candidates La liste des mots candidats (char**)
 * @param nb_candidates Le nombre de candidats dans la liste
 * @return Un pointeur vers un tableau de tableaux d'entiers [[indice, distance], ...] du style liste_mots_candidats = [[0, 4][1, 7][2, 3]]
 */
int** get_candidates_distances(char* wrong_word, char** candidates, int nb_candidates);

/**
 * @brief Trie la matrice des distances par ordre croissant de score
 * 
 * @param distance_matrix La matrice générée par get_candidates_distances
 * @param nb_candidates Le nombre de lignes dans la matrice
 */
void sort_candidate_distances(int** distance_matrix, int nb_candidates);

/**
 * @brief Retourne la correction pour un mot mal orthographié
 * 
 * @param wrong_word Le mot à corriger
 * @param dict Un pointeur vers un dictionaire de type Dictionary_t
 * @return Un pointeur vers une nouvelle chaîne de caractères contenant le mot corrigé
 */
char* get_word_correction(char* wrong_word, Dictionary_t* dict);

#endif