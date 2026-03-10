#include "pretty_print.h"
#include "common.h"
#include <stdlib.h>
#include <string.h>

void print_iso8859_to_utf8(const char *input) {
  unsigned char c;
  while ((c = *input++)) {
    if (c < 0x80) {
      putchar(c);
    } else {
      putchar(0xC0 | (c >> 6));
      putchar(0x80 | (c & 0x3F));
    }
  }
}

int pretty_print_detection(char *line, uint32_t line_number, size_t word_count,
                           uint32_t *word_indices) {
  if (line == NULL || word_indices == NULL) {
    return -1;
  }

  char *line_cpy = strdup(line);
  if (line_cpy == NULL) {
    return -1;
  }

  size_t bad_word_index = 0;
  size_t i = 0;
  char *token;
  char *saveptr;

  printf("%d: ", line_number);
  token = strtok_r(line_cpy, " \n", &saveptr);
  while (token != NULL) {
    if (bad_word_index < word_count && i == word_indices[bad_word_index]) {
      printf(RED);
      print_iso8859_to_utf8(token);
      printf(" " RESET);
      bad_word_index++;
    } else {
      printf(GREEN);
      print_iso8859_to_utf8(token);
      printf(" " RESET);
    }
    i++;
    token = strtok_r(NULL, " \n", &saveptr);
  }

  printf("\n");
  free(line_cpy);
  return 0;
}

int pretty_print_correction(char *line, uint32_t line_number,
                            size_t correction_count, uint32_t *word_indices,
                            char **corrections) {
  if (line == NULL) {
    return -1;
  }

  char *line_cpy = strdup(line);
  if (line_cpy == NULL) {
    return -1;
  }

  size_t bad_word_index = 0;
  size_t i = 0;
  char *token;
  char *saveptr;

  printf("%d: ", line_number);
  token = strtok_r(line_cpy, " \n", &saveptr);
  while (token != NULL) {
    if (bad_word_index < correction_count &&
        i == word_indices[bad_word_index]) {
      printf("(" RED);
      print_iso8859_to_utf8(token);
      printf(RESET " → " GREEN);
      print_iso8859_to_utf8(corrections[bad_word_index++]);
      printf(RESET ")  ");
    } else {
      print_iso8859_to_utf8(token);
      printf(" ");
    }
    i++;
    token = strtok_r(NULL, " \n", &saveptr);
  }

  printf("\n");
  free(line_cpy);
  return 0;
}