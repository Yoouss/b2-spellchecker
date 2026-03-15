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
 * @brief  fonction qui prend une ligne qui représente une phrase et vérifie si les mots sont dans le dictionnaire  
 *
 * @param  line un tableau de mots qui contiennent des charactères
 * @param  dict un pointeur vers un dictionaire de type Dictionary_t
 * @return retourne un tableau contenant les positions des mots qui ne figurent pas dans le dico 
 *         (NULL si tous les mots sont dans le dictionnaire)
 */
int* words_in_line(char* line, Dictionary_t* dict);

/**
 * @brief  fonction helper de words_in_line qui informe sur les nombres de mots n'étant pas dans le dictionnaire 
 *
 * @param  line un tableau de mots qui contiennent des charactères
 * @param  dict un pointeur vers un dictionaire de type Dictionary_t
 * @return -1 en cas d'erreur, les nombres de mauvais mots sinon
 */
uint32_t numbers_of_bad_words_in_line(char* line, Dictionary_t* dict);

/**
 * @brief  fonction helper de words_in_line qui fourni l'index des mauvais mots 
 *
 * @param  line un tableau de mots qui contiennent des charactères
 * @param  dict un pointeur vers un dictionaire de type Dictionary_t
 * @return NULL en cas d'erreur, un tableau des index des mauvais mots sinon
 */
int* get_indexes_of_bad_words_in_line(char* line, uint32_t numberOfBadWords, Dictionary_t* dict);

/**
 * @brief  fonction qui prend un fichier et vérifie si les tous mots sont dans le dictionnaire  
 *
 * @param  filename le nom du fichier du dossier inputs
 * @param  dict un pointeur vers un dictionaire de type Dictionary_t
 * @return retourne un tableau contenant les positions des mots qui ne figurent pas dans le dico 
 */
int** words_in_file(char* filename, Dictionary_t* dict);

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