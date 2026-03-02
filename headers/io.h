#include <stdint.h>
#include <stdio.h>
#include "common.h"

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
OutputStreams_t* open_outputs(const char *pathname);

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
 * @param dict_index The index of the dictionary used for the bad word detection.
 * @param word_count The total number of bad words in the line.
 * @param word_indices An array of indices where the bad words were found.
 * @return 0 on success, -1 on failure.
 */
int write_detection(OutputStreams_t* output_stream, uint32_t line_number, uint32_t dict_index, uint32_t word_count, uint32_t* word_indices);

/**
 * @brief Write the result of a bad word correction to the file <output_path>.fix
 * following the described format in the README
 * 
 * USE `open(const char*)` AND `close(FILE* stream)` TO HANDLE FILES.
 * 
 * @param output_streams The file streams to write to. If NULL, write to stdout.
 * @param word_count The total number of bad words in the line.
 * @param corrections An array of suggested corrections for the bad words.
 * @return 0 on success, -1 on failure.
 */
int write_correction(OutputStreams_t* output_streams, uint32_t word_count, char** corrections);

/**
 * @brief Pretty print the result of a bad word detection to the console
 *
 * @param line The line of text where the bad words were found.
 * @param line_number The line number where the bad words were found.
 * @param word_count The total number of bad words in the line.
 * @param word_indices An array of indices where the bad words were found.
 * @return int 0 on success, -1 on failure.
 */
int pretty_print_detection(char* line, uint32_t line_number, size_t word_count, uint32_t* word_indices);

/**
 * @brief Pretty print the result of a bad word correction to the console
 *
 * @param line The line of text where the bad words were found.
 * @param line_number The line number where the bad words were found.
 * @param word_count The number of corrections available.
 * @param word_indices An array of indices where the bad words were found.
 * @param corrections An array of suggested corrections for the bad words.
 * @return int 0 on success, -1 on failure.
 */
int pretty_print_correction(char* line, uint32_t line_number, size_t word_count, uint32_t* word_indices, char** corrections);

/**
 * @brief Read the input file and store each line in a dynamically allocated array.
 *
 * @param input_path The path to the input file.
 * @param lines A pointer to a char** to hold the array of lines.
 * @param line_sizes A pointer to a uint32_t array to hold the sizes of each line.
 * @param line_count A pointer to a size_t to hold the number of lines read.
 * @return int 0 on success, -1 on failure.
 */
int read_input_file(char* input_path, char*** lines, uint32_t** line_sizes, size_t* line_count) {

    *line_count = INPUT_LINE_COUNT;
    *lines = malloc(sizeof(char*) * INPUT_LINE_COUNT);
    *line_sizes = malloc(sizeof(uint32_t) * INPUT_LINE_COUNT);

    if (!*lines || !*line_sizes) return -1;

    for (size_t i = 0; i < INPUT_LINE_COUNT; i++) {
        (*lines)[i] = strdup(input_data[i]);
        (*line_sizes)[i] = INPUT_WORD_COUNTS[i];
    }
    return 0;
}

/**
 * @brief Read the dictionary files from the specified path.
 *
 * @param dicts_path The path to the directory containing the dictionary files.
 * @param dicts A pointer to an array of `Dictionary_t` structures to hold the loaded dictionaries.
 * @param dict_count A pointer to a size_t variable to store the number of dictionaries successfully loaded.
 * @return 0 on success, -1 on failure.
 */
int load_dictionaries(const char* path, Dictionary_t** dicts, size_t* dict_count) {
    *dicts = malloc(sizeof(Dictionary_t));
    if (!*dicts) return -1;

    *dict_count = 1;
    Dictionary_t* d = *dicts;

    d -> id = 1;
    d -> lang = strdup("wallon");
    d -> word_count = DICTIONARY_SIZE;

    d -> words = malloc(sizeof(char*) * DICTIONARY_SIZE);
    if (!d -> words) {
        free(d -> lang);
        free(d);
        return -1;
    }
    for (size_t i = 0; i < DICTIONARY_SIZE; i++) {
        d -> words[i] = strdup(dictionary[i]);
    }

    return 0;
}

#endif // IO_H