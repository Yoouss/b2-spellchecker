#include <stdint.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "common.h"

#ifndef DETECTOR_H
#define DETECTOR_H

#define SEPARATORS " %\t.,;:!?\"()[]{}_/\\|@#$^&*+=~`"

/**
 * Structure représentant une (char*) ligne d'un fichier
 *
 * @wrong_words_indexes : le tableau des indexes (on commence à compter à 0) des mauvais mots dans la ligne
 * @wrong_words_count   : le nombre de mauvais mots dans la ligne (= la taille de wrong_words_indexes)
 * @used_dictionary     : un pointeur vers le dictionnaire utilisé pour corriger la ligne
 * @used_dict_id        : l'id du dictionnaire utilisé pour corriger la ligne
 */
typedef struct line {
    uint32_t* wrong_words_indexes;
    uint32_t wrong_words_count;
    Dictionary_t* used_dictionary;
    uint32_t used_dict_id;
} line_t;

/**
 * Structure utilisée pour stocker plusieurs informations nécessaires à find_candidate_dict_for_line 
 * (mono ou multi-threadée) 
 *
 * @line         : un tableau de char qui représente une ligne du fichier
 * @dicts        : un tableau de dictionnaire(s) de type Dictionary_t
 *
 * @dict_found   : un pointeur vers le dictionaire de type Dictionary_t potentiellement candidat pour cette ligne
 * @errors_count : le nombre d'erreurs dans la ligne avec dict_found
 * @status : indique si la fonction a réussie (0) ou échouée (-1)
 *
 * @start_index  : l'index de départ (dans le tableau de dictionnaires) 
 * @end_index    : l'index de fin (dans le tableau de dictionnaires ) 
 * 
 * @note start_index et end_index sont principalement utilisés pour séparer le travail entre plusieurs threads
 *       en mono-threadée, start_index = 0 et end_index = le nombre de dictionnaires (= taille de dicts)
 */
typedef struct candidate_dict_data {
    char* line;
    Dictionary_t* dicts;

    Dictionary_t* dict_found; 
    uint32_t errors_count;
    int status;

    size_t start_index;
    size_t end_index;
} candidate_dict_data_t;

/**
 * @brief fonction qui libère la mémoire allouée pour une structure line_t
 * 
 * @param line_detection un pointeur vers la structure line_t à libérer
 * 
 * @note   complexité temporelle : O(1)
 *         complexité spaciale : O(1)
 */
void free_line_detection(line_t* line_detection);

/**
 * @brief  fonction qui prend un mot et vérifie si le mot est dans le dictionnaire en recherche binaire : 
 *
 * @param  word un tableau de charactères qui réprésente un mot.
 * @param  dictionary un pointeur vers un dictionaire de type Dictionary_t
 * @return si vrai retourne 1 si c'est faux on retourne 0 et -1 si erreur
 * 
 * @note   complexité temporelle : O(log(n))
 *         complexité spaciale : O(1)
 * 
 *         n = nombre de mots dans le dictionnaire
 */
int word_in_dictionary(char* word, Dictionary_t* dictionary);

/**
 * @brief fonction helper qui remplit un pointeur vers une structure candidate_dict_data_t avec les informations nécessaires. Elle est utilisé pour organiser la proportion de travail dans un contexte multi-threadé, ou bien simplement indiquer les informations nécessaires pour un contexte mono-threadé
 * 
 * @param candidate_dict_data un pointeur vers une ou plusieurs structure candidate_dict_data_t
 * @param active_threads le nombre réels de threads que l'on va utiliser
 *                       (= num_threads si dicts_count >= 2 * num_threads, sinon = 1 -> mono-threadé)
 * @param line un tableau de char qui représente une ligne du fichier
 * @param dicts un tableau de dictionnaire(s) de type Dictionary_t
 * @param dicts_count le nombre de dictionnaires (= taille de dicts)
 * 
 * @note la charge de travail = dicts_count / active_threads (en mono-threadée, active_threads doit être forcé à 1)
 *       la taille de candidate_dict_data = active_threads
 * 
 * @note   complexité temporelle : O(t)
 *         complexité spaciale : O(1)
 *
 *         t = le nombre de threads actifs (active_threads) 
 */
void set_candidate_dict_data(candidate_dict_data_t* candidate_dict_data, int active_threads,
                             char* line, Dictionary_t* dicts, size_t dicts_count);

/**
 * @brief fonction helper de find_candidate_dict_for_line et find_candidate_dict_for_line_thread qui fais le travail de find_candidate_dict_for_line sur un interval renseigné en paramètre
 * 
 * @param candidate_dict_data un pointeur vers une structure candidate_dict_data_t 
 * où est notamment renseigné l'interval de départ (start_index) et d'arrivée (end_index)
 * 
 * @note   complexité temporelle : O(k (l + m log n))
 *         complexité spaciale : O(1) 
 *
 *         k = nombre de dictionnaires (= end_index - start_index)
 *         l = nombre de charactères de la ligne
 *         m = nombre de mots dans la ligne
 *         n = nombre de mots dans le dictionnaire
 */
void find_potential_candidate_dict_for_line(candidate_dict_data_t* candidate_dict_data);

/**
 * @brief fonction helper de find_candidate_dict_for_line qui s'occupe d'une partie équitable du travail de find_candidate_dict_for_line en fonction du nombre de threads entrées par l'utilisateur. Le travail efféctué par une thread est égale au nombre de dictionnaires divisé par le nombre de threads actifs
 * 
 * @param args un pointeur void* vers une structure candidate_dict_data_t 
 * 
 * @note   complexité temporelle : O(k (l + m log n))
 *         complexité spaciale : O(1) 
 *
 *         k = nombre de dictionnaires (= end_index - start_index)
 *         l = nombre de charactères de la ligne
 *         m = nombre de mots dans la ligne
 *         n = nombre de mots dans le dictionnaire
 */
void* find_candidate_dict_for_line_thread(void* args);

/**
 * @brief  fonction à parallélisme configurable qui prend une ligne et trouve le dictionnaire candidat qui minimise le nombre d'erreurs 
 *
 * @param  line un tableau de mots qui contiennent des charactères
 * @param  dicts un pointeur vers un ou des dictionnaire(s) de type Dictionary_t
 * @param  dicts_count la taille de dicts
 * @return un pointeur vers le dictionnaire candidat ou NULL en cas d'erreur
 *
 * @note le traitement est mono-threadé si num_threads = 1 ou que dicts_count < 2 * num_threads, sinon il est multi-threadé
 * 
 * @note   complexité temporelle : O(k (l + m log n))
 *         complexité spaciale : O(1)
 *
 *         k = nombre de dictionnaires
 *         l = nombre de charactères de la ligne
 *         m = nombre de mots dans la ligne
 *         n = nombre de mots dans le dictionnaire
 */
Dictionary_t* find_candidate_dict_for_line(char* line, Dictionary_t* dicts, size_t dicts_count);

/**
 * @brief  fonction helper de scan_line_for_errors qui informe sur les nombres de mots n'étant pas dans le dictionnaire 
 *
 * @param  line un tableau de mots qui contiennent des charactères
 * @param  wrong_words_count l'adresse du nombre de mots erronés dans la ligne
 * @param  dictionary un pointeur vers un dictionaire de type Dictionary_t
 * @return -1 en cas d'erreur, 0 sinon (en modifiant le contenu de wrong_words_count)
 * 
 * @note   complexité temporelle : O(l + m log n)
 *         complexité spaciale : O(1)
 * 
 *         l = nombre de charactères de la ligne
 *         m = nombre de mots dans la ligne
 *         n = nombre de mots dans le dictionnaire
 */
int set_wrong_words_count_in_line(char* line, uint32_t* wrong_words_count, Dictionary_t* dictionary);

/**
 * @brief  fonction helper de scan_line_for_errors qui fourni l'index des mauvais mots 
 *
 * @param  line un tableau de mots qui contiennent des charactères
 * @param  wrong_words_count le nombre de mauvais mots
 * @param  dictionary un pointeur vers un dictionaire de type Dictionary_t
 * @return un tableau avec l'index des mauvais mots du tableau, NULL si on a aucun mauvais mots ou en cas d'erreur
 * 
 * @note   complexité temporelle : O(l + m log n)
 *         complexité spaciale : O(j)
 *       
 *         j = nombre de mauvais mots
 *         l = nombre de charactères de la ligne
 *         m = nombre de mots dans la ligne
 *         n = nombre de mots dans le dictionnaire
 */
uint32_t* get_wrong_words_indexes_in_line(char* line, uint32_t wrong_words_count, Dictionary_t* dictionary);

/**
 * @brief  fonction utilisée dans src/main.c qui fourni la liste des mauvais mots  présents dans une ligne
 *
 * @param  line un tableau de mots qui contiennent des charactères
 * @param  wrong_words_indexes tableau contenant l'index des mauvais mots dans une ligne
 * @param  wrong_words_count un nombre non signé qui représente le nombre de mauvais mots
 * @return un tableau avec les mauvais mots ou NULL en cas d'erreur
 * 
 * @note   complexité temporelle : O(l)
 *         complexité spaciale : O(j)
 *       
 *         j = nombre de mauvais mots
 *         l = nombre de charactères de la ligne
 */
char** get_wrong_words_in_line(char* line, uint32_t* wrong_words_indexes, uint32_t wrong_words_count);

/**
 * @brief  fonction qui prend une ligne (ex : du fichier) et informe sur la présence d'erreurs et leurs index 
 *
 * @param  line un tableau de mots qui contiennent des charactères
 * @param  dictionaries un pointeur vers des dictionaire de type Dictionary_t
 * @param  dictionaries_count la taille de dictionaries
 * @return un pointeur vers une structure line_t ou NULL si tous les mots sont dans un des dictionnaires ou en cas d'erreur
 * 
 * @note   complexité temporelle : O(k (l + m log n))
 *         complexité spaciale : O(j)
 *
 *         j = nombre de mauvais mots
 *         k = nombre de dictionnaires
 *         l = nombre de charactères de la ligne
 *         m = nombre de mots dans la ligne
 *         n = nombre de mots dans le dictionnaire
 */
line_t* scan_line_for_errors(char* line, Dictionary_t* dictionaries, size_t dictionaries_count);

#endif