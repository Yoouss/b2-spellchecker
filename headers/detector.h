#include "common.h"
#include <stdint.h>
#include <stdio.h>

#include "dict.h"
#include "input.h"

#ifndef IO_H
#define IO_H


/**
 * @brief  fonction qui prend un mot et vérifie si le mot est dans le dictionnaire : 
 *
 * @param  word un tableau de charactères qui réprésente un mot.
 * @param  dict un pointeur vers un dictionaire de type Dictionary_t
 * @return si vrai returne 1 si c'est faux on retourne 0 et -1 si erreur
 */
int word_in_dictionary(char* word, Dictionary_t* dict);

/**
 * @brief  fonction qui prend une ligne qui représente une phrase et vérifie si les mots sont dans le dictionnaire  
 *
 * @param  line un tableau de mots qui contiennent des charactères
 * @param  int longueur de la ligne
 * @param  dict un pointeur vers un dictionaire de type Dictionary_t
 * @return retourne un tableau contenant les positions des mots qui ne figurent pas dans le dico 
 */
int* words_in_line(char** line, int lenght, Dictionary_t* dict );


/**
 * @brief  fonction qui prend un fichier et vérifie si les tous mots sont dans le dictionnaire  
 *
 * @param  file un tableau de lignes qui contient des mots qui contiennent des charactères
 * @param  int longueur du fichier
 * @param  dict un pointeur vers un dictionaire de type Dictionary_t
 * @return retourne un tableau contenant les positions des mots qui ne figurent pas dans le dico 
 */
int** words_in_file(char** file, int length, Dictionary_t* dict );

