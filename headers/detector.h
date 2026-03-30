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
 * @wrong_words_indexes : tableau des indexes (on commence à compter à 0) des mauvais mots dans la ligne
 * @wrong_words_count   : nombre de mauvais mots dans la ligne (= la taille de wrong_words_indexes)
 */

typedef struct line {
    uint32_t* wrong_words_indexes;
    uint32_t wrong_words_count;
} line_t;

/**
 * Structure représentant un (char**) fichier texte
 *
 * @incorrect_lines         : tableau de line_t renseignant sur les indexes et nombres de mauvais mots d'une ligne i
 * @incorrect_lines_indexes : tableau des indexes (on commence à compter à 0) des lignes contenant des fautes dans le fichier
 * @incorrect_lines_count  : nombre de lignes contenant des mots éronnés (= la taille de incorrect_line et incorrect_line_indexes)
 */

typedef struct file {
    line_t* incorrect_lines;
    size_t* incorrect_lines_indexes;
    size_t incorrect_lines_count;
} file_t;

/**
 * @brief  fonction qui prend un mot et vérifie si le mot est dans le dictionnaire en recherche binaire : 
 *
 * @param  word un tableau de charactères qui réprésente un mot.
 * @param  dict un pointeur vers un dictionaire de type Dictionary_t
 * @return si vrai retourne 1 si c'est faux on retourne 0 et -1 si erreur
 * 
 * @note complexité temporelle : O(log(n)), avec n = nombre de mots du dictionnaire
 *       complexité spaciale : O(1)
 */
int word_in_dictionary(char* word, Dictionary_t* dict);

/**
 * @brief  fonction qui prend une ligne et trouve le dictionnaire candidat qui minimise le nombre d'erreurs 
 *
 * @param  line un tableau de mots qui contiennent des charactères
 * @param  dicts un pointeur vers des dictionaire de type Dictionary_t
 * @param  number_of_dictionaries le nombre de dictionaires 
 * @return un pointeur vers le dictionnaire candidat ou NULL en cas d'erreur
 */
Dictionary_t* find_candidate_dict_for_line(char* line, Dictionary_t* dicts, size_t number_of_dictionaries);

/**
 * @brief  fonction qui prend une ligne qui représente une phrase et vérifie si les mots sont dans le dictionnaire  
 *
 * @param  line un tableau de mots qui contiennent des charactères
 * @param  dicts un pointeur vers des dictionaire de type Dictionary_t
 * @param  number_of_dictionaries le nombre de dictionaires 
 * @return retourne un tableau avec à l'index 0 le nombre d'erreurs et leur index dans la line
 *         (NULL si tous les mots sont dans le dictionnaire)
 */
line_t* scan_line_for_wrong_words(char* line, Dictionary_t* dicts, size_t number_of_dictionaries);

/**
 * @brief  fonction helper de scan_line_for_wrong_words qui informe sur les nombres de mots n'étant pas dans le dictionnaire 
 *
 * @param  line un tableau de mots qui contiennent des 
 * @param  wrong_words_count l'adresse du nombre de mots erronés dans la ligne
 * @param  dict un pointeur vers un dictionaire de type Dictionary_t
 * @return -1 en cas d'erreur, 0 sinon (en modifiant le contenu de wrong_words_count)
 */
int set_wrong_words_count_in_line(char* line, uint32_t* wrong_words_count, Dictionary_t* dict);

/**
 * @brief  fonction helper de scan_line_for_wrong_words qui fourni l'index des mauvais mots 
 *
 * @param  line un tableau de mots qui contiennent des charactères
 * @param  numberOfBadWords le nombre de mauvais mots
 * @param  dict un pointeur vers un dictionaire de type Dictionary_t
 * @return NULL en cas d'erreur, un tableau avec le nombre de mauvais mots à l'index 0 et leur index aux index du tableau suivants
 */
uint32_t* get_wrong_words_indexes_in_line(char* line, uint32_t numberOfBadWords, Dictionary_t* dict);

/**
 * @brief  fonction utilisée dans src/main.c qui fourni la listes des mauvais mots 
 *
 * @param  line un tableau de mots qui contiennent des charactères
 * @param  indexesOfBadWords tableau ayant le nombre de mauvais mots à l'index 0 et les mauvais mots à la suite
 * @return NULL en cas d'erreur, un tableau avec les mauvais mots
 */
char** get_bad_words_in_line(char* line, uint32_t* indexesOfBadWords);

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
file_t* scan_file_for_wrong_words(char* filename, Dictionary_t* dicts, size_t number_of_dictionaries);

/**
 * @brief Function that frees the matrix's memory
 *
 * @param matrix The matrix
 * @param matrixLength The number of rows the matrix has
 */
static inline void free_matrix(uint32_t** matrix, int matrixLength) {
    for (int i = 0; i < matrixLength; i++) {
        if (matrix[i] != NULL) free(matrix[i]);
    }

    free(matrix);
}

#endif