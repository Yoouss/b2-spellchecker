#include "common.h"
#include <stdint.h>
#include <stdio.h>
#include "dict.h"
#include "input.h"

#ifndef FILE_HANDLER_H
#define FILE_HANDLER_H

/**
 * @brief Projette un fichier en mémoire pour une lecture
 * * Utilisée par read_input_file pour obtenir un accès direct au contenu du fichier
 * 
 * @param path Le chemin vers le fichier à
 * @param file_size Pointeur pour stocker la taille totale du fichier
 * @return Un pointeur vers la zone mémoire
 */
char* map_file(const char* path, size_t* file_size);

/**
 * @brief Compte le nombre de lignes présentes dans une zone mémoire
 * * Utilisée par read_input_file pour pré-allouer les tableaux de lignes
 * 
 * @param map Pointeur vers le début de la zone mémoire
 * @param size Taille de la zone mémoire
 * @return Le nombre total de lignes
 */
size_t count_lines(const char* map, size_t size);

/**
 * @brief libère la mémoire allouée pour un tableau de chaînes de caractères
 * * Utilisée par read_input_file pour pour liberer la mémoire en cas d'erreur ou de fin d'usage
 * 
 * @param lines Le tableau de lignes à libérer
 * @param sizes le tableau des tailles correspondantes à libérer
 * @param count Le nombre de lignes à libérer
 */
void clean_lines(char** lines, uint32_t* sizes, size_t count);

/**
 * @brief Extrait le nom de la langue à partir du nom du fichier dictionnaire
 * * Utilisée par load_single_dictionary pour pour identifier la langue
 * 
 * @param filepath Le chemin complet ou le nom du fichier
 * @return le code de la langue
 */
char* get_language(const char* filepath);

/**
 * @brief Charge un seul dictionnaire
 * * Utilisée par load_dictionaries
 * 
 * @param dict Pointeur vers la structure à remplir
 * @param filepath Le chemin vers le fichier dictionnaire
 * @param id l'identifiant à assigner à ce dictionnaire
 * @return 0 en cas de succès, -1 en cas d'échec de lecture
 */
int load_single_dictionary(Dictionary_t *dict, const char *filepath, uint32_t id);

/**
 * @brief Read the input file and store each line in a dynamically allocated
 * array.
 *
 * @param input_path The path to the input file.
 * @param lines A pointer to a char** to hold the array of lines.
 * @param line_sizes A pointer to a uint32_t array to hold the sizes of each
 * line.
 * @param line_count A pointer to a size_t to hold the number of lines read.
 * @return int 0 on success, -1 on failure.
 */
int read_input_file(char *input_path, char ***lines, uint32_t **line_sizes, size_t *line_count);

/**
 * @brief Read the dictionary files from the specified path.
 *
 * @param dicts_path The path to the directory containing the dictionary files.
 * @param dicts A pointer to an array of `Dictionary_t` structures to hold the
 * loaded dictionaries.
 * @param dict_count A pointer to a size_t variable to store the number of
 * dictionaries successfully loaded.
 * @return 0 on success, -1 on failure.
 */
int load_dictionaries(const char *path, Dictionary_t **dicts, size_t *dict_count);


#endif