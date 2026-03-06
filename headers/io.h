#include "common.h"
#include <stdint.h>
#include <stdio.h>
#include <string.h>
#include <stdlib.h>

#include "dict.h"
#include "input.h"

#ifndef IO_H
#define IO_H

typedef struct {
  int detection;
  int correction;
} OutputStreams_t;

/**
 * @brief Open files for writing.
 *
 * @param pathname The path to the file to open.
 * @return OutputStreams_t* The file streams, or NULL on failure.
 */
OutputStreams_t *open_outputs(const char *pathname);

/**
 * @brief Close a file.
 *
 * @param streams The file streams to close.
 */
void close_outputs(OutputStreams_t *streams);

/**
 * @brief Write the result of a bad word detection to the file <output_path>.err
 * following the described format in the README.
 *
 * USE `int open(const char*)` AND `int close(int fd)` TO HANDLE FILES.
 *
 * @param output_stream The file streams to write to. If NULL, write to stdout.
 * @param line_number The line number where the bad words were found.
 * @param dict_index The index of the dictionary used for the bad word
 * detection.
 * @param word_count The total number of bad words in the line.
 * @param word_indices An array of indices where the bad words were found.
 * @return 0 on success, -1 on failure.
 */
int write_detection(OutputStreams_t *output_stream, uint32_t line_number,
                    uint32_t dict_index, uint32_t word_count,
                    uint32_t *word_indices);

/**
 * @brief Write the result of a bad word correction to the file
 * <output_path>.fix following the described format in the README
 *
 * USE `open(const char*)` AND `close(FILE* stream)` TO HANDLE FILES.
 *
 * @param output_streams The file streams to write to. If NULL, write to stdout.
 * @param word_count The total number of bad words in the line.
 * @param corrections An array of suggested corrections for the bad words.
 * @return 0 on success, -1 on failure.
 */
int write_correction(OutputStreams_t *output_streams, uint32_t word_count,
                     char **corrections);

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
int read_input_file(char *input_path, char ***lines, uint32_t **line_sizes,
                    size_t *line_count) {
  *line_count = 0;

  *lines = malloc(sizeof(char *) * INPUT_LINE_COUNT);
  if (!*lines)
    return -1;

  *line_sizes = malloc(sizeof(uint32_t) * INPUT_LINE_COUNT);
  if (!*line_sizes) {
    free(*lines);
    return -1;
  }
  for (size_t i = 0; i < INPUT_LINE_COUNT; i++) {
    (*lines)[i] = strdup(INPUT_DATA[i]);

    if (!(*lines)[i]) {
      for (size_t j = 0; j < i; j++) {
        free((*lines)[j]);
      }
      free(*lines);
      free(*line_sizes);
      return -1;
    }

    (*line_sizes)[i] = INPUT_WORD_COUNTS[i];
  }
  *line_count = INPUT_LINE_COUNT;
  return 0;
}

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
int load_dictionaries(const char *path, Dictionary_t **dicts,
                      size_t *dict_count) {

  *dict_count = DICT_COUNT;

  *dicts = calloc(DICT_COUNT, sizeof(Dictionary_t));
  if (!*dicts)
    return -1;

  for (size_t i = 0; i < DICT_COUNT; i++) {
    const char **source_words = DICTIONARIES[i];
    Dictionary_t *d = &(*dicts)[i];

    d->word_count = DICT_SIZES[i] - 1; // id excluded
    d->id = (uint32_t)strtoul(source_words[0], NULL, 10);
    d->lang = strdup(DICT_LANGS[i]);

    if (!d->lang)
      goto fail;

    d->words = calloc(d->word_count, sizeof(char *));
    if (!d->words)
      goto fail;

    for (size_t j = 1; j < DICT_SIZES[i]; j++) {
      d->words[j - 1] = strdup(source_words[j]);
      if (!d->words[j - 1])
        goto fail;
    }
  }
  return 0;

fail:
  for (size_t i = 0; i < DICT_COUNT; i++) {
    Dictionary_t *d = &(*dicts)[i];
    if (d->words) {
      for (uint32_t j = 0; j < d->word_count; j++) {
        free(d->words[j]);
      }
      free(d->words);
    }
    free(d->lang);
  }
  free(*dicts);
  return -1;
}

#endif // IO_H
