#include "common.h"
#include <stdint.h>
#include <stdio.h>

#include "dict.h"
#include "input.h"

#ifndef ALGO_H
#define ALGO_H


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
 * @brief Calcule la distance de Levenstein d'édition entre deux mots
 * 
 * @param word1 Le premier mot
 * @param word2 Le second mot
 * @return La distance entre les 2 mots
 */
int calculate_distance(char* word1, char* word2);

/**
 * @brief Génère une matrice de distances pour les candidats
 * 
 * @param wrong_word Le mot erroné de base
 * @param candidates La liste des mots candidats (char**)
 * @param nb_candidates Le nombre de candidats dans la liste
 * @return Un pointeur vers un tableau de tableaux d'entiers [[indice, distance], ...] du style liste_mots_candidats= [[0, 4][1, 7][2, 3]]
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
 * @brief Retourne le mot final corrigé après analyse de la matrice triée
 * 
 * @param sorted_matrix La matrice de distances après tri
 * @return un pointeur vers sort_cnadidate_distances[0][0]
 */
char* get_final_correction(int** sorted_matrix, char** candidates);

#endif