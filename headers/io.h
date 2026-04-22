#include "common.h"
#include <stdint.h>

#ifndef IO_H
#define IO_H

/**
 * @brief Manages file descriptors of output files `.fix` and `.err`.
 * @note These are raw file descriptors that must be opened with `open()`.
 */
typedef struct {
  int detection;
  int correction;
} OutputStreams_t;

/**
 * @brief Open files for writing.
 *
 * Use `int open(const char*, mode?)`, not `fopen`.
 *
 * @param pathname The path to the file to open.
 * @return OutputStreams_t* The file streams, or NULL on failure.
 */
OutputStreams_t *open_outputs(const char *pathname);

/**
 * @brief Close a file.
 *
 * Use `int close(int fd)` to handle files, not `fclose`.
 *
 * @param streams The file streams to close.
 */
void close_outputs(OutputStreams_t *streams);

/**
 * @brief Write the result of a bad word detection to the file <output_path>.err
 * following the described format in the README.
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

#endif
