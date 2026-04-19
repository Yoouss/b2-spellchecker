#include "common.h"
#include <stdint.h>

#ifndef IO_MULTI_H
#define IO_MULTI_H

typedef struct {
    char** lines;
    uint32_t* lines_sizes;
    size_t line_count;

    char* file_map;
    int start_index;
    int end_index;
} read_input_data_t;

int read_input_file_multi(char *input_path, char ***lines, uint32_t **line_sizes, size_t *line_count);

void* read_input_file_thread(void* args);

int load_dictionaries_multi(const char *path, Dictionary_t **dicts, size_t *dict_count);

void* load_dictionaries_thread(void* args);

#endif