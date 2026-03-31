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
 * @incorrect_lines_indexes : tableau des indexes (on commence à compter à 0) des lignes contenant des erreurs dans le fichier
 * @incorrect_lines_count   : nombre de lignes contenant des erreurs (= la taille de incorrect_lines et incorrect_lines_indexes)
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
 * @brief  fonction qui prend une ligne et trouve le dictionnaire candidat qui minimise le nombre d'erreurs 
 *
 * @param  line un tableau de mots qui contiennent des charactères
 * @param  dictionaries un pointeur vers des dictionaire de type Dictionary_t
 * @param  dictionaries_count la taille de dictionaries
 * @return un pointeur vers le dictionnaire candidat ou NULL en cas d'erreur
 * 
 * @note   complexité temporelle : O(k (l + m log n))
 *         complexité spaciale : O(1)
 *
 *         k = nombre de dictionnaires
 *         l = nombre de charactères de la ligne
 *         m = nombre de mots dans la ligne
 *         n = nombre de mots dans le dictionnaire
 */
Dictionary_t* find_candidate_dict_for_line(char* line, Dictionary_t* dictionaries, size_t dictionaries_count);

/**
 * @brief  fonction helper de scan_line_for_errors qui informe sur les nombres de mots n'étant pas dans le dictionnaire 
 *
 * @param  line un tableau de mots qui contiennent des 
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
 * @brief  TODO : fonction (bientôt) utilisée dans src/main.c qui fourni la liste des mauvais mots 
 *
 * @param  line un tableau de mots qui contiennent des charactères
 * @param  wrong_words_indexes tableau contenant l'index des mauvais mots dans une ligne
 * @return un tableau avec les mauvais mots ou NULL en cas d'erreur
 * 
 * @note   complexité temporelle : TODO
 *         complexité spaciale : TODO
 *       
 *         TODO
 */
char** get_wrong_words_in_line(char* line, uint32_t* wrong_words_indexes);

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

/**
 * @brief  fonction qui prend le chemin vers un fichier.txt et informe sur la présence de lignes erronées, leur nombre, 
 *         leurs index dans le fichier et, pour chaque ligne érronée, leurs erreurs et leur index 
 *
 * @param  filename le nom d'un fichier (format txt) dans le dossier inputs
 * @param  dictionaries un pointeur vers des dictionaire de type Dictionary_t
 * @param  dictionaries_count la taille de dictionaries
 * @return un pointer vers une structure file_t ou NULL si aucune erreur est detectée ou en cas d'erreur
 * 
 * @note complexité temporelle : O(k (l + m log n))
 *       complexité spaciale : O(m + j)
 * 
 *       j = nombre de mauvais mots du fichier
 *       k = nombre de dictionnaires
 *       l = nombre de charactères dans le fichier
 *       m = nombre de mots dans le fichier
 *       n = nombre de mots dans le dictionnaire
 */
file_t* scan_file_for_errors(char* filename, Dictionary_t* dictionaries, size_t dictionaries_count);

#endif