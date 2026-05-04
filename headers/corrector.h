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
 * Structure utilisée pour stocker plusieurs informations nécessaires à set_words_correction 
 * (mono ou multi-threadée) 
 *
 * @wrong_words     : un tableau de char* avec chaque mauvais mots détectés dans une ligne 
 * @corrections     : un tableau de char* où sera stocké la correction pour chaque mauvais mots
 * @used_dictionary : un pointeur vers le dictionaire de type Dictionary_t utilisé pour corriger la ligne
 * 
 * @start_index     : l'index de départ (dans le tableau de mauvais mots et de corrections) 
 * @end_index       : l'index de fin (dans le tableau de mauvais mots et de corrections) 
 * 
 * @note start_index et end_index sont principalement utilisés pour séparer le travail entre plusieurs threads
 *       en mono-threadée, start_index = 0 et end_index = le nombre de mauvais mots (= taille de wrong_words et corrections)
 */
typedef struct words_correction_data {
    char** wrong_words;
    char** corrections;
    Dictionary_t* used_dictionary;

    size_t start_index;
    size_t end_index;
} words_correction_data_t;

/**
 * @brief fonction qui libère la mémoire allouée pour un tableau de mots (représentées par des char*)
 * 
 * @param array_of_words le tableau de mots à libérer
 * @param array_length le nombre de mots à libérer
 * 
 * @note   complexité temporelle : O(n)
 *         complexité spaciale : O(1)
 * 
 *         n = le nombre de mots à libérer
 */
void free_array_of_words(char** array_of_words, size_t array_length);

/**
 * @brief  calcule le nombre de trigrammes (= mot de trois lettres) de word1 présents dans word2
 * *       fonction utilisée par get_candidate_words pour réduire le nombre de mots candidats du dictionnaire
 * 
 * @param  word1 le premier mot
 * @param  word2 le deuxième mot
 * @return le nombre de trigrammes présents dans les deux mots ou 0 si un des deux mots est trop petit
 *
 * @note   complexité temporelle : O(n * m * min(n, m))
 *         complexité spaciale : O(1)
 * 
 *         n = la taille de word1
 *         m = la taille de word2
 */
int count_number_of_shared_trigrammes(char* word1, char* word2);

/**
 * @brief  retourne un tableau de mots candidats qui a la taille du nombre de mots candidats
 * *       fonction utilisée dans get_candidate_words pour réduire la complexité spatiale 
 * 
 * @param  candidate_words 
 * @param  number_of_candidates
 * @return un tableau contenant les mots candidats ou NULL en cas d'erreur
 * 
 * @note   complexité temporelle : O(c)
 *         complexité spaciale : O(c)
 * 
 *         c = le nombre de mots candidats           
 */
char** get_final_candidates_list(char** candidate_words, int number_of_candidates);

/**
 * @brief  retourne un tableau de mots candidats qui ont au moins 2 trigrammes en commun (ou 1 si le mot mal ortographié est court) avec le mot mal ortographié
 * 
 * @param  wrong_word le mot mal orthographié
 * @param  dict le dictionnaire utilisé
 * @param  result_count un pointeur vers un entier qui sera le nombre de mots candidats après l'appel à get_candidate_words
 * @return un tableau avec les mots candidats ou NULL en cas d'erreur
 * 
 * @note   complexité temporelle : O(d * (n * m * min(n, m)))
 *         complexité spaciale : O(c)
 * 
 *         d = le nombre de mots dans le dictionnaire
 *         c = le nombre de mots candidats 
 *         n = la taille du mot mal orthographié
 *         m = la taille du plus grand mot dans le dictionnaire
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
 * @note   complexité temporelle : O(d * (n * m * min(n, m)))
 *         complexité spaciale : O(p) 
 *
 *         d = le nombre de mots dans le dictionnaire
 *         n = la taille du mot mal orthographié
 *         m = la taille du plus grand mot dans le dictionnaire
 *         p = la taille du mot candidat
 */
char* get_word_correction(char* wrong_word, Dictionary_t* dict);

/**
 * @brief fonction helper qui remplit un pointeur vers une structure words_correction_data_t avec les informations nécessaires. Elle est utilisé pour organiser la proportion de travail dans un contexte multi-threadé, ou bien simplement indiquer les informations nécessaires pour un contexte mono-threadé
 * 
 * @param words_correction_data un pointeur vers une ou plusieurs structure read_input_data_t
 * @param active_threads le nombre réels de threads que l'on va utiliser
 *                       (= num_threads si wrong_words_count >= 2 * num_threads, sinon = 1 -> mono-threadé)
 * @param wrong_words un tableau de char* avec chaque mauvais mots détectés dans une ligne 
 * @param corrections un tableau de char* où sera stocké la correction pour chaque mauvais mots
 * @param used_dictionary un pointeur vers le dictionaire de type Dictionary_t utilisé pour corriger la ligne
 * @param wrong_words_count le nombre de mauvais mots (= taille de wrong_words et corrections)
 * 
 * @note la charge de travail = wrong_words_count / active_threads (en mono-threadée, active_threads doit être forcé à 1)
 *       la taille de read_input_data = active_threads
 * 
 * @note   complexité temporelle : O(t)
 *         complexité spaciale : O(1)
 *
 *         t = le nombre de threads actifs (active_threads) 
 */
void set_words_correction_data(words_correction_data_t* words_correction_data, int active_threads, char** wrong_words, 
                               char** corrections, Dictionary_t* used_dictionary, size_t wrong_words_count);

/**
 * @brief fonction helper de set_words_correction et set_words_correction_thread qui fais le travail de set_words_correction sur un interval renseigné en paramètre
 * 
 * @param words_correction_data un pointeur vers une structure words_correction_data_t 
 * où est notamment renseigné l'interval de départ (start_index) et d'arrivée (end_index)
 * 
 * @note   complexité temporelle : O(i * (d * (n * m * min(n, m))))
 *         complexité spaciale : O(1) 
 *
 *         d = le nombre de mots dans le dictionnaire
 *         i = le nombre de mauvais mots à traiter (end_index - start_index)
 *         n = la taille du plus grand mot mal orthographié
 *         m = la taille du plus grand mot dans le dictionnaire
 */
void set_some_corrections(words_correction_data_t* words_correction_data);

/**
 * @brief fonction helper de set_words_correction qui s'occupe d'une partie équitable du travail de set_words_correction en fonction du nombre de threads entrées par l'utilisateur. Le travail efféctué par une thread est égale au nombre de mauvais mots divisé par le nombre de threads actifs
 * 
 * @param args un pointeur void* vers une structure words_correction_data_t 
 * 
 * @note   complexité temporelle : O(i * (d * (n * m * min(n, m))))
 *         complexité spaciale : O(1) 
 *
 *         d = le nombre de mots dans le dictionnaire
 *         i = le nombre de mauvais mots à traiter (end_index - start_index)
 *         n = la taille du plus grand mot mal orthographié
 *         m = la taille du plus grand mot dans le dictionnaire
 */
void* set_words_correction_thread(void* args);

/**
 * @brief fonction à parallélisme configurable qui stocke la correction de mauvais mots dans un pointeur de tableau de char*
 *
 * @param wrong_words un tableau de char* avec chaque mauvais mots détectés dans une ligne 
 * @param corrections un pointeur vers le tableau de char* où sera stocké la correction pour chaque mauvais mots
 * @param wrong_words_count le nombre de mauvais mots (= taille de wrong_words et corrections)
 * @param dict un pointeur vers le dictionaire de type Dictionary_t utilisé pour corriger la ligne
 * 
 * @return -1 en cas d'erreur, 0 en cas de succès
 * 
 * @note le traitement est mono-threadé si num_threads = 1 ou que wrong_words_count < 2 * num_threads, sinon il est multi-threadé
 * 
 * @note   complexité temporelle : O(i * (d * (n * m * min(n, m))))
 *         complexité spaciale : O(1) 
 *
 *         d = le nombre de mots dans le dictionnaire
 *         i = le nombre de mauvais mots à traiter
 *         n = la taille du plus grand mot mal orthographié 
 *         m = la taille du plus grand mot dans le dictionnaire
 */
int set_words_correction(char** wrong_words, char*** corrections, uint32_t wrong_words_count, Dictionary_t* dict);

#endif