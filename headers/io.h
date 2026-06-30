#include <common.h>
#include <stdint.h>

#ifndef IO_H
#define IO_H

/**
 * @brief S'occupe des files descriptors des fichiers de sortie `.fix` et `.err`.
 * @note Ce sont des files descriptors qui doivent être ouverts avec `open()`.
 */
typedef struct {
  int detection;
  int correction;
} OutputStreams_t;

/**
 * @brief Ouvre le fichier pour écriture.
 *
 * @param pathname Le chemin vers le fichier à ouvrir.
 * @return OutputStreams_t* Le stream de fichiers, ou NULL en cas d'erreur.
 */
OutputStreams_t *open_outputs(const char *pathname);

/**
 * @brief Ferme un fichier.
 *
 * @param streams Le streams des fichiers à fermer.
 */
void close_outputs(OutputStreams_t *streams);

/**
 * @brief Ecrit le résultat de la détection d'un mauvais mot dans le fichier
 * <output_path>.err suivant le format décrit dans le README.
 *
 * UTILISEZ `int open(const char*)` ET `int close(int fd)` POUR MANIPULER LES FICHIERS.
 *
 * @param output_stream Le stream du fichier sur lequel on doit écrire. Si NULL, écrire sur stdout.
 * @param line_number Le numéro de la ligne où les mauvais ont été trouvés.
 * @param dict_index L'index du dictionnaire utilisé pour la détection des mauvais mots.
 * @param word_count Le nombre total de mauvais mots dans la ligne.
 * @param word_indices Un tableau des indices où les mauvais mots ont été trouvés. 
 * @return 0 en cas de succèss, -1 en cas d'échec.
 */
int write_detection(OutputStreams_t *output_stream, uint32_t line_number,
                    uint32_t dict_index, uint32_t word_count,
                    uint32_t *word_indices);

/**
 * @brief Écrit le résultat de la détection dans <output_path>.err pour tout le fichier input.
 *
 * @param output_stream Un stream de fichier contenant les files decriptors des deux fichiers <output_path>.err et <output_path>.fix.
 * @param dicts Un pointeur vers des dictionaire de type Dictionary_t.
 * @param dicts_count Le nombre de dictionaire.
 * @param lines Un double pointeur sur des lignes du fichier contenant des mots. 
 * @param line_count Le nombre de ligne du fichier.
 * @return 0 en cas de succèss, -1 en cas d'échec.
 */
int write_all_detection(OutputStreams_t *output_stream, Dictionary_t* dicts, size_t dicts_count, char** lines, size_t line_count);

/**
 * @brief Ecrit le résultat de la correction d'un mauvais mot dans le fichier
 * <output_path>.fix suivant le format décrit dans le README
 *
 * UTILISEZ `open(const char*)` ET `close(FILE* stream)` POUR MANIPULER LES FICHIERS.
 *
 * @param output_streams Le stream du fichier sur lequel on doit écrire. Si NULL, écrire sur stdout.
 * @param word_count Le nombre total de mauvais mots dans la ligne.
 * @param corrections Un tableau des suggestions des corrections pour les mauvais mots.
 * @return 0 en cas de succèss, -1 en cas d'échec.
 */
int write_correction(OutputStreams_t *output_streams, uint32_t word_count, char **corrections);

/**
 * @brief Écrit le résultat de la détection dans <output_path>.err et de la correction dans <output_path>.fix pour tout le fichier input.
 *
 * @param output_stream Un stream de fichier contenant les files decriptors des deux fichiers <output_path>.err et <output_path>.fix.
 * @param dicts Un pointeur vers des dictionaire de type Dictionary_t.
 * @param dicts_count Le nombre de dictionaire.
 * @param lines Un double pointeur sur des lignes du fichier contenant des mots. 
 * @param line_count Le nombre de ligne du fichier.
 * @return 0 en cas de succèss, -1 en cas d'échec.
 */
int write_all_detection_and_correction(OutputStreams_t *output_stream, Dictionary_t* dicts, size_t dicts_count, char** lines, size_t line_count);

#endif
