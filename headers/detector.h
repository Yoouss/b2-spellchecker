#include <stdint.h>
#include <stdio.h>
#include <string.h>
#include "common.h"

#define SEPARATORS " %\t.,;:!?\"()[]{}_/\\|@#$^&*+=~`"

/**
 * @brief  fonction qui prend un mot et vérifie si le mot est dans le dictionnaire : 
 *
 * @param  word un tableau de charactères qui réprésente un mot.
 * @param  dict un pointeur vers un dictionaire de type Dictionary_t
 * @return si vrai returne 1 si c'est faux on retourne 0 et -1 si erreur
 */
short word_in_dictionary(char* word, Dictionary_t* dict);

/**
 * @brief  fonction qui prend une ligne et trouve le dictionnaire candidat qui minimise le nombre d'erreurs 
 *
 * @param  line un tableau de mots qui contiennent des charactères
 * @param  dicts un pointeur vers des dictionaire de type Dictionary_t
 * @param  number_of_dictionaries le nombre de dictionaires 
 * @return un pointeur vers le dictionnaire candidat ou NULL en cas d'erreur
 */
Dictionary_t* find_candidate_dict_for_line(char* line, Dictionary_t* dicts, uint16_t number_of_dictionaries);

/**
 * @brief  fonction qui prend une ligne qui représente une phrase et vérifie si les mots sont dans le dictionnaire  
 *
 * @param  line un tableau de mots qui contiennent des charactères
 * @param  dicts un pointeur vers des dictionaire de type Dictionary_t
 * @param  number_of_dictionaries le nombre de dictionaires 
 * @return retourne un tableau avec à l'index 0 le nombre d'erreurs et leur index dans la line
 *         (NULL si tous les mots sont dans le dictionnaire)
 */
int* words_in_line(char* line, Dictionary_t* dicts, uint16_t number_of_dictionaries);

/**
 * @brief  fonction helper de words_in_line qui informe sur les nombres de mots n'étant pas dans le dictionnaire 
 *
 * @param  line un tableau de mots qui contiennent des charactères
 * @param  dict un pointeur vers un dictionaire de type Dictionary_t
 * @return -1 en cas d'erreur, les nombres de mauvais mots sinon
 */
int32_t number_of_bad_words_in_line(char* line, Dictionary_t* dict);

/**
 * @brief  fonction helper de words_in_line qui fourni l'index des mauvais mots 
 *
 * @param  line un tableau de mots qui contiennent des charactères
 * @param  dict un pointeur vers un dictionaire de type Dictionary_t
 * @return NULL en cas d'erreur, un tableau avec le nombre de mauvais mots à l'index 0 et leur index aux index du tableau suivants
 */
int32_t* get_indexes_of_bad_words_in_line(char* line, int32_t numberOfBadWords, Dictionary_t* dict);

/**
 * @brief  fonction qui prend un fichier et vérifie si les tous mots sont dans le dictionnaire  
 *
 * @param  filename le nom du fichier du dossier inputs
 * @param  dicts un pointeur vers des dictionaire de type Dictionary_t
 * @param  number_of_dictionaries le nombre de dictionaires 
 * @return NULL en cas d'erreur, retourne un matrice contenant les tableaux des 
 *         positions des mots qui ne figurent pas dans le dico sinon
 *         (index 0 = longueur du tableau = nombre de mauvais mots à cette ligne du fichier -> index de la ligne de la matrice)
 */
int32_t** words_in_file(char* filename, Dictionary_t* dicts, uint16_t number_of_dictionaries);

/**
 * @brief  O(log(n)) search of a word in a given dictionary
 *
 * @param word An array of characters representing a word
 * @param dict a pointer to a Dictionary_t dictionnary
 * @return 1 if found, -1 if an error occured and 0 otherwise 
 */
static inline int binary_search_word_in_dictonary(char* targetWord, Dictionary_t* dict) {
    if (targetWord == NULL || dict == NULL) return -1;

    char** words = dict->words;
    int32_t left = 0;
    int32_t right = dict->word_count - 1;

    while (left <= right) {
        int32_t middle = (left + right) / 2;
        char* word = words[middle];

        short comparison = strcmp(word, targetWord); // Is word smaller or greater than TargetWord ?

        if (comparison < 0) {
            left = middle + 1;
        }

        else if (comparison > 0) {
            right = middle - 1;
        }

        else {
            return 1;
        }
    }

    return 0;
}