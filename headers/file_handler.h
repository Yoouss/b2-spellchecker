#include "common.h"
#include <stdint.h>
#include <stdio.h>
#include "dict.h"
#include "input.h"

#ifndef FILE_HANDLER_H
#define FILE_HANDLER_H

/**
 * Structure utilisée pour stocker plusieurs informations nécessaires à read_input_file 
 * (mono ou multi-threadée) 
 *
 * @lines       : un tableau de char* représentant chaque ligne d'un fichier
 * @lines_sizes : un tableau contenant le nombre de caractères pour chaque ligne d'un fichier
 * @line_count  : le nombre de lignes d'un fichier (= taille de lines et lines_sizes)
 * 
 * @mapped_file : un pointeur vers la zone mémoire mmap contenant le fichier
 * @start_index : l'index de départ (dans le fichier) 
 * @end_index   : l'index de fin (dans le fichier)
 * 
 * @note start_index et end_index sont principalement utilisés pour séparer le travail entre plusieurs threads
 *       en mono-threadée, start_index = 0 et end_index = la taille du fichier mappé
 */
typedef struct read_input_data {
    char** lines;
    uint32_t* lines_sizes;
    size_t line_count;

    char* mapped_file;
    size_t start_index;
    size_t end_index;
} read_input_data_t;

/**
 * @brief fonction qui libère la mémoire allouée pour un tableau des lignes d'un fichier (représentées par des char*)
 * 
 * @param lines le tableau de lignes à libérer
 * @param line_count le nombre de lignes à libérer
 * 
 * @note   complexité temporelle : O(n)
 *         complexité spaciale : O(1)
 * 
 *         n = le nombre de lignes à libérer
 */
void free_lines(char** lines, size_t line_count);

/**
 * @brief fonction helper qui projette un fichier en mémoire
 * 
 * @param file_path le chemin vers le fichier à mapper en mémoire
 * @param file_size un pointeur où sera stocké la taille fichier
 * @return un pointeur vers la zone mémoire contenant le fichier, ou NULL soit en cas d'erreur, soit si le fichier est vide
 * 
 * @note le pointeur retourné doit être libéré plus tard avec munmap()
 * 
 * @note   complexité temporelle : O(1)
 *         complexité spaciale : O(1)
 */
char* map_file(const char* file_path, size_t* file_size);

/**
 * @brief fonction helper qui compte le nombre de lignes présentes dans un fichier mappé en mémoire
 * 
 * @param mapped_file un pointeur vers le fichier mappé en mémoire
 * @param file_size la taille du fichier 
 * @param line_count un pointeur où sera stocké le nombre de lignes du fichier
 * 
 * @note   complexité temporelle : O(m)
 *         complexité spaciale : O(1)
 *
 *         m = le nombre de caractères du fichier (file_size)
 */
void set_line_count(const char* mapped_file, size_t file_size, size_t* line_count);

/**
 * @brief fonction helper qui récupère le nombre de charactères de chaque lignes 
 * 
 * @param mapped_file un pointeur vers le fichier mappé en mémoire
 * @param file_size la taille du fichier 
 * @param uint32_t le pointeur d'un tableau où sera stocké le nombre de charactères de chaque ligne 
 * @param line_count le nombre de lignes du fichier
 * 
 * @return -1 en cas d'erreur, 0 sinon (en modifiant le contenu de lines_sizes)
 * 
 * @note   complexité temporelle : O(m)
 *         complexité spaciale : O(n)
 *
 *         m = le nombre de caractères du fichier
 *         n = le nombre de lignes du fichier 
 */
int set_lines_sizes(const char* mapped_file, size_t file_size, uint32_t** lines_sizes, size_t line_count);

/**
 * @brief fonction helper qui alloue la mémoire nécessaire pour chaque ligne d'un fichier
 * 
 * @param lines le pointeur d'un tableau de string où sera alloué la mémoire nécessaire pour chaque ligne d'un fichier
 * @param lines_sizes un tableau contenant le nombre de caractères de chaque ligne 
 * @param line_count le nombre de lignes d'un fichier (= taille de lines et lines_sizes)
 * 
 * @return -1 en cas d'erreur, 0 sinon (en modifiant le contenu de lines)
 * 
 * @note   complexité temporelle : O(n)
 *         complexité spaciale : O(m)
 * 
 *         m = le nombre de caractères du fichier
 *         n = le nombre de lignes du fichier 
 */
int set_lines(char*** lines, uint32_t* lines_sizes, size_t line_count);

/**
 * @brief fonction helper qui remplit un pointeur vers une structure read_input_data avec les informations nécessaires. Elle est utilisé pour organiser la proportion de travail dans un contexte multi-threadé, ou bien simplement indiquer les informations nécessaires pour un contexte mono-threadé
 * 
 * @param read_input_data un pointeur vers une ou plusieurs structure read_input_data_t
 * @param lines le pointeur d'un tableau de string où sera alloué la mémoire nécessaire pour chaque ligne d'un fichier
 * @param lines_sizes un tableau contenant le nombre de caractères de chaque ligne 
 * @param line_count le nombre de lignes d'un fichier (= taille de lines et lines_sizes)
 * @param mapped_file un pointeur vers le fichier mappé en mémoire
 * @param file_size la taille du fichier 
 * @param chunk le nombre de caractères minimum qui doit être traité pour chaque structure read_input_data
 * 
 * @note chunk = file_size / num_threads (en mono-threadée, num_threads doit être forcé à 1)
 *       la taille de read_input_data = num_threads
 * 
 * @note   complexité temporelle : O(t)
 *         complexité spaciale : O(1)
 *
 *         t = le nombre de threads 
 */
void set_read_input_data(read_input_data_t* read_input_data, char** lines, uint32_t* lines_sizes, 
                         size_t line_count, char* mapped_file, size_t file_size, size_t chunk);

/**
 * @brief fonction helper de read_chunk_of_input_file qui renseigne sur l'index de la ligne de départ et de l'index de départ dans cette ligne
 * 
 * @param start_line_index un pointeur où sera stocké l'index de la ligne de départ
 * @param file_offset un pointeur où sera stocké l'index de départ dans start_line_index
 * @param read_input_data un pointeur vers une structure read_input_data_t
 * 
 * @note   complexité temporelle : O(n)
 *         complexité spaciale : O(1)
 *
 *         n = le nombre de lignes du fichier 
 */
void set_start_line_index_and_file_offset(size_t* start_line_index, size_t* file_offset, read_input_data_t* read_input_data);

/**
 * @brief fonction helper de read_input_file et read_input_file_thread qui fais le travail de read_input_file sur un interval renseigné en paramètre
 * 
 * @param read_input_data un pointeur vers une structure read_input_data_t 
 * où est notamment renseigné l'interval de départ (start_index) et d'arrivée (end_index)
 * 
 * @note   complexité temporelle : O(c)
 *         complexité spaciale : O(1)
 *
 *         c = le nombre de caractères à traiter (end_index - start_index)
 */
void read_chunk_of_input_file(read_input_data_t* read_input_data);

/**
 * @brief fonction helper de read_input_file qui s'occupe d'une partie équitable du travail de read_input_file en fonction du nombre de threads entrées par l'utilisateur. Le travail efféctué par une thread est égale à la taille du fichier divisé par le nombre de threads
 * 
 * @param args un pointeur void* vers une structure read_input_data_t 
 * 
 * @note   complexité temporelle : O(c)
 *         complexité spaciale : O(1)
 *
 *         c = le nombre de caractères à traiter (end_index - start_index)
 */
void* read_input_file_thread(void* args);

/**
 * @brief fonction à parallélisme configurable qui lie un fichier et stocke chaque ligne dans un tableau dynamiquement alloué
 *
 * @param input_path le chemin vers le fichier d'entrée
 * @param lines un pointeur vers un char** pour stocker le tableau de lignes
 * @param line_sizes un pointeur vers un tableau uint32_t pour stocker la taille de chaque lignes
 * @param line_count un pointeur vers un size_t pour stocker le nombre de lignes lues 
 * 
 * @return -1 en cas d'erreur, 0 en cas de succès
 * 
 * @note le traitement est mono-threadé si num_threads = 1, sinon il est multi-threadé
 * 
 * @note   complexité temporelle : O(m)
 *         complexité spaciale : O(m + n)
 *
 *         m = le nombre de caractères du fichier (file_size)
 *         n = le nombre de lignes du fichier
 */
int read_input_file(char *input_path, char ***lines, uint32_t **line_sizes, size_t *line_count);

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