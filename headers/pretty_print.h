#ifndef PRETTY_PRINT_H
#define PRETTY_PRINT_H

#include <stdint.h>
#include <stdio.h>
/**
 * @brief Pretty print the result of a bad word detection to the console
 *
 * @param line The line of text where the bad words were found.
 * @param line_number The line number where the bad words were found.
 * @param word_count The total number of bad words in the line.
 * @param word_indices An array of indices where the bad words were found.
 * @return int 0 on success, -1 on failure.
 */
int pretty_print_detection(char *line, uint32_t line_number, size_t word_count,
                           uint32_t *word_indices);

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
int pretty_print_correction(char *line, uint32_t line_number, size_t word_count,
                            uint32_t *word_indices, char **corrections);

#endif